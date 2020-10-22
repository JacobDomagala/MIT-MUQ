#ifndef PARALLELFIXEDSAMPLESMIMCMC_H_
#define PARALLELFIXEDSAMPLESMIMCMC_H_

#if MUQ_HAS_MPI

#if !MUQ_HAS_PARCER
#error
#endif

#include "spdlog/spdlog.h"
#include <boost/property_tree/ptree.hpp>

#include "MUQ/SamplingAlgorithms/ParallelMIMCMCWorker.h"
#include "MUQ/SamplingAlgorithms/ParallelizableMIComponentFactory.h"
#include "MUQ/SamplingAlgorithms/SamplingAlgorithm.h"

namespace muq {
  namespace SamplingAlgorithms {

    class StaticLoadBalancer {
    public:
      struct WorkerAssignment {
        int numGroups;
        int numWorkersPerGroup;
      };
      virtual void setup(std::shared_ptr<ParallelizableMIComponentFactory> componentFactory, uint availableRanks) = 0;
      virtual int numCollectors(std::shared_ptr<MultiIndex> modelIndex) = 0;
      virtual WorkerAssignment numWorkers(std::shared_ptr<MultiIndex> modelIndex) = 0;
    };

    class RoundRobinStaticLoadBalancer : public StaticLoadBalancer {
    public:
      void setup(std::shared_ptr<ParallelizableMIComponentFactory> componentFactory, uint availableRanks) override {
        ranks_remaining = availableRanks;
        spdlog::info("Balancing load across {} ranks", availableRanks);
        auto indices = MultiIndexFactory::CreateFullTensor(componentFactory->FinestIndex()->GetVector());
        models_remaining = indices->Size();
      }

      int numCollectors(std::shared_ptr<MultiIndex> modelIndex) override {
        ranks_remaining--;
        return 1;
      }
      WorkerAssignment numWorkers(std::shared_ptr<MultiIndex> modelIndex) override {
        WorkerAssignment assignment;
        assignment.numWorkersPerGroup = 1;
        assignment.numGroups = ranks_remaining / models_remaining;

        spdlog::debug("Of {}, assigning {} to model {}", ranks_remaining, assignment.numGroups * assignment.numWorkersPerGroup, *modelIndex);

        assert (assignment.numGroups * assignment.numWorkersPerGroup > 0);

        models_remaining--;
        ranks_remaining -= assignment.numGroups * assignment.numWorkersPerGroup;

        return assignment;
      }
    private:
      uint ranks_remaining;
      uint models_remaining;

    };

    class StaticLoadBalancingMIMCMC : public SamplingAlgorithm {
    public:
      StaticLoadBalancingMIMCMC (pt::ptree pt,
                                 std::shared_ptr<ParallelizableMIComponentFactory> componentFactory,
                                 std::shared_ptr<StaticLoadBalancer> loadBalancing = std::make_shared<RoundRobinStaticLoadBalancer>(),
                                 std::shared_ptr<parcer::Communicator> comm = std::make_shared<parcer::Communicator>(),
                                 std::shared_ptr<muq::Utilities::OTF2TracerBase> tracer = std::make_shared<OTF2TracerDummy>())
       : SamplingAlgorithm(nullptr),
         pt(pt),
         comm(comm),
         componentFactory(componentFactory),
         phonebookClient(std::make_shared<PhonebookClient>(comm, phonebookRank)),
         workerClient(comm, phonebookClient, rootRank) {

        spdlog::debug("Rank: {}", comm->GetRank());

        if (comm->GetRank() == rootRank) {

          auto comm_self = std::make_shared<parcer::Communicator>(MPI_COMM_SELF);
          componentFactory->SetComm(comm_self);

          auto indices = MultiIndexFactory::CreateFullTensor(componentFactory->FinestIndex()->GetVector());

          assert(comm->GetSize() - 2 >= 0);
          loadBalancing->setup(componentFactory, comm->GetSize() - 2);

          int rank = 2;

          // Assign collectors
          spdlog::trace("Assigning collectors");
          for (int i = 0; i < indices->Size(); i++) {
            std::shared_ptr<MultiIndex> index = (*indices)[i];
            std::vector<int> collectorRanks;
            int numCollectors = loadBalancing->numCollectors(index);
            for (int r = 0; r < numCollectors; r++) {
              collectorRanks.push_back(rank);
              rank++;
            }
            collectorClients.push_back(CollectorClient(comm, collectorRanks, index));
          }

          // Assign workers
          spdlog::trace("Assigning workers");
          for (int i = 0; i < indices->Size(); i++) {

            std::shared_ptr<MultiIndex> index = (*indices)[i];
            StaticLoadBalancer::WorkerAssignment assignment = loadBalancing->numWorkers(index);

            for (int group = 0; group < assignment.numGroups; group++) {
              std::vector<int> groupRanks;
              for (int r = 0; r < assignment.numWorkersPerGroup; r++) {
                groupRanks.push_back(rank);
                rank++;
              }
              workerClient.assignGroup(groupRanks, index);
            }

            assert (rank <= comm->GetSize());
          }


        } else if (comm->GetRank() == phonebookRank) {
          PhonebookServer phonebook(comm, tracer);
          phonebook.Run();
        } else {
          auto phonebookClient = std::make_shared<PhonebookClient>(comm, phonebookRank);
          WorkerServer worker(pt, comm, phonebookClient, rootRank, componentFactory, tracer);
        }

      }

      virtual std::shared_ptr<SampleCollection> GetSamples() const override { return nullptr; };
      virtual std::shared_ptr<SampleCollection> GetQOIs() const override { return nullptr; };

      Eigen::VectorXd MeanQOI() {
        if (comm->GetRank() != rootRank) {
          return Eigen::VectorXd::Zero(1);
        }

        for (CollectorClient& client : collectorClients) {
          client.ComputeMeans();
        }

        while (true) {
          MPI_Status status;
          //timer_idle.start();
          ControlFlag command = comm->Recv<ControlFlag>(MPI_ANY_SOURCE, ControlTag, &status);
          //timer_idle.stop();

          for (CollectorClient& client : collectorClients) {
            if (client.Receive(command, status))
              break;
          }

          /*if (!command_handled) {
           *    std::cerr << "Unexpected command!" << std::*endl;
           *    exit(43);
        }*/


          bool isComputingMeans = false;
          for (CollectorClient& client : collectorClients) {
            isComputingMeans = isComputingMeans || client.IsComputingMeans();
          }
          if (!isComputingMeans)
            break;
        }
        std::cout << "Computing means completed" << std::endl;
        //std::cout << "Root process " << comm->GetRank() << " idle time:\t" << timer_idle.elapsed() << " of:\t" << timer_full.elapsed() << std::endl;


        Eigen::VectorXd mean_box = collectorClients[0].GetQOIMean();
        mean_box.setZero();
        Eigen::VectorXd mean = mean_box;

        for (CollectorClient& client : collectorClients) {
          mean_box = client.GetQOIMean();
          mean += mean_box;
          //std::cout << "Mean level:\t" << mean_box.transpose() << " adding up to:\t" << mean.transpose() << std::endl;
        }
        return mean;
      }

      virtual std::shared_ptr<SampleCollection> GetSamples() {
        return nullptr;
      }
      virtual std::shared_ptr<SampleCollection> GetQOIs() {
        return nullptr;
      }

      void Finalize() {
        if (comm->GetRank() == rootRank) {
          phonebookClient->SchedulingStop();
          std::cout << "Starting unassign sequence" << std::endl;
          for (CollectorClient& client : collectorClients) {
            client.Unassign();
          }
          workerClient.UnassignAll();
          std::cout << "Finished unassign sequence" << std::endl;

          workerClient.Finalize();
          std::cout << "Rank " << comm->GetRank() << " quit" << std::endl;
        }
      }

      void RequestSamples(std::shared_ptr<MultiIndex> index, int numSamples) {
        if (comm->GetRank() != rootRank) {
          return;
        }
        // TODO: Divide work between workers
        for (CollectorClient& client : collectorClients) {
          client.GetModelIndex();
          if (client.GetModelIndex() == index) {
            client.CollectSamples(numSamples);
            return;
          }
        }
        std::cerr << "Requested samples from nonexisting collector!" << std::endl;
      }

      void RequestSamplesAll(int numSamples) {
        if (comm->GetRank() != rootRank) {
          return;
        }
        // TODO: Get indices from collectors, then request samples for each index
        for (CollectorClient& client : collectorClients) {
          client.CollectSamples(numSamples);
        }
      }

      //bool did_reassign = false;
      void RunSamples() {
        if (comm->GetRank() != rootRank) {
          return;
        }
        //Dune::Timer timer_idle;
        //Dune::Timer timer_full;
        while (true) {
          MPI_Status status;
          //timer_idle.start();
          ControlFlag command = comm->Recv<ControlFlag>(MPI_ANY_SOURCE, ControlTag, &status);
          //timer_idle.stop();

          bool command_handled = false;
          for (CollectorClient& client : collectorClients) {
            if (client.Receive(command, status)) {
              command_handled = true;
              break;
            }
          }

          if (!command_handled) {
            if (command == ControlFlag::SCHEDULING_NEEDED) {
              spdlog::debug("SCHEDULING_NEEDED entered!");
              // TODO: Phonebook client receive analog zu CollectorClient statt manuellem Empangen!
              auto idle_index = std::make_shared<MultiIndex>(comm->Recv<MultiIndex>(status.MPI_SOURCE, ControlTag));
              int rescheduleRank = comm->Recv<int>(status.MPI_SOURCE, ControlTag);
              auto busy_index = std::make_shared<MultiIndex>(comm->Recv<MultiIndex>(status.MPI_SOURCE, ControlTag));

              //if (!did_reassign) {
              spdlog::debug("SCHEDULING_NEEDED Unassigning {}!", rescheduleRank);
              workerClient.UnassignGroup(idle_index, rescheduleRank);
              // TODO: Unterstütze Gruppen von workers!!

              std::vector<int> subgroup(0);
              subgroup.push_back(rescheduleRank);
              workerClient.assignGroup(subgroup, busy_index);

              phonebookClient->SchedulingDone();
              spdlog::debug("SCHEDULING_NEEDED left!");
              //did_reassign=true;
              //}
              command_handled = true;
            }
          }


          if (!command_handled) {
            std::cerr << "Unexpected command!" << std::endl;
            exit(43);
          }

          bool isSampling = false;
          for (CollectorClient& client : collectorClients) {
            isSampling = isSampling || client.IsSampling();
          }
          if (!isSampling)
            break;
        }
        spdlog::debug("Sampling completed");
      }

      void WriteToFile(std::string filename) {
        if (comm->GetRank() != rootRank) {
          return;
        }
        for (CollectorClient& client : collectorClients) {
          client.WriteToFile(filename);
        }
      }


    protected:
      virtual std::shared_ptr<SampleCollection> RunImpl(std::vector<Eigen::VectorXd> const& x0) {

        for (CollectorClient& client : collectorClients) {
          int numSamples = pt.get<int>("NumSamples" + multiindexToConfigString(client.GetModelIndex()));
          client.CollectSamples(numSamples);
        }

        RunSamples();
        return nullptr;
      }

    private:

      std::string multiindexToConfigString (std::shared_ptr<MultiIndex> index) {
        std::stringstream strs;
        for (int i = 0; i < index->GetLength(); i++) {
          strs << "_" << index->GetValue(i);
        }
        return strs.str();
      }

      const int rootRank = 0;
      const int phonebookRank = 1;
      pt::ptree pt;
      std::shared_ptr<parcer::Communicator> comm;
      std::shared_ptr<ParallelizableMIComponentFactory> componentFactory;
      std::shared_ptr<PhonebookClient> phonebookClient;
      std::vector<CollectorClient> collectorClients;
      WorkerClient workerClient;
    };
  }
}

#endif

#endif
