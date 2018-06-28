#ifndef SAMPLINGSTATE_H_
#define SAMPLINGSTATE_H_

#include <iostream>
#include <unordered_map>
#include <vector>

#include <Eigen/Core>
#include <boost/any.hpp>

namespace muq {
  namespace SamplingAlgorithms {

    /**
    @ingroup SamplingAlgorithms
    @class SamplingState
    @brief Each state is one sample generated by a sampling algorithm
    @details A SamplingState includes a vector holding the location of the state as
             well as a std::map of metadata.  The metadata can include density
             evaluations and other information collected by the sampling algorithm.
     */
    class SamplingState {
    public:

      SamplingState(Eigen::VectorXd const& stateIn, double weight = 1.0);
      SamplingState(std::vector<Eigen::VectorXd> const& stateIn, double weight = 1.0);

      virtual ~SamplingState() = default;

      /// The state variables
      const std::vector<Eigen::VectorXd> state;

      /// The weight of this state
      double weight;

      /// Checks to see if the meta map contains a particular key
      bool HasMeta(std::string const& metaKey);

      /// The total number of parameters in the state, i.e., the sum of state[i].size()
      int TotalDim() const;

      /// A map containing extra information like the target density, run time, forward model output, etc...
      std::unordered_map<std::string, boost::any> meta;

    private:

    };

  } // namespace SamplingAlgoritms
} // namespace muq

#endif
