#include "MUQ/SamplingAlgorithms/SLMCMC.h"
#include "MUQ/SamplingAlgorithms/GreedyMLMCMC.h"
#include "MUQ/SamplingAlgorithms/MIMCMC.h"

#include "MUQ/Modeling/Distributions/Gaussian.h"
#include "MUQ/Modeling/Distributions/Density.h"

#include "MUQ/SamplingAlgorithms/MHKernel.h"
#include "MUQ/SamplingAlgorithms/MHProposal.h"
#include "MUQ/SamplingAlgorithms/CrankNicolsonProposal.h"
#include "MUQ/SamplingAlgorithms/SamplingProblem.h"
#include "MUQ/SamplingAlgorithms/SubsamplingMIProposal.h"

#include "MUQ/SamplingAlgorithms/MIComponentFactory.h"

#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;
using namespace muq::Modeling;
using namespace muq::SamplingAlgorithms;
using namespace muq::Utilities;

#include "MUQ/SamplingAlgorithms/ParallelMIMCMCWorker.h"
#include "MUQ/SamplingAlgorithms/ParallelFixedSamplesMIMCMC.h"

#include "Problem.h"


int main(int argc, char **argv){

  MPI_Init(&argc, &argv);

  pt::ptree pt;
  pt.put("NumSamples_0", 1e3);
  pt.put("NumSamples_1", 5e2);
  pt.put("NumSamples_2", 1e2);
  pt.put("NumSamples_3", 1e2);
  pt.put("MCMC.BurnIn", 100);
  pt.put("MLMCMC.Subsampling", 1000);

  auto comm = std::make_shared<parcer::Communicator>();

  for (int subsampling : {0, 5, 10, 25, 100}) {
    if (comm->GetRank() == 0)
      std::cout << "Running with subsampling " << subsampling << std::endl;
    pt.put("MLMCMC.Subsampling", subsampling);

    auto componentFactory = std::make_shared<MyMIComponentFactory>(pt);
    StaticLoadBalancingMIMCMC parallelMIMCMC (pt, componentFactory);

    if (comm->GetRank() == 0) {
      parallelMIMCMC.Run();
      Eigen::VectorXd meanQOI = parallelMIMCMC.MeanQOI();
      std::cout << "mean QOI: " << meanQOI.transpose() << std::endl;
    }
    parallelMIMCMC.Finalize();
  }

  MPI_Finalize();
}
