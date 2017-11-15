#ifndef MHPROPOSAL_H_
#define MHPROPOSAL_H_

#include "MUQ/Modeling/Distributions/Gaussian.h"

#include "MUQ/SamplingAlgorithms/MCMCProposal.h"

namespace muq {
  namespace SamplingAlgorithms {

    class MHProposal : public MCMCProposal {
    public:

      MHProposal(boost::property_tree::ptree const& pt);

      ~MHProposal();
      
    private:

      /// Propose a new point
      virtual boost::any SampleImpl(muq::Modeling::ref_vector<boost::any> const& inputs) override;

      /// Evaluate the proposal distribution
      virtual double LogDensityImpl(muq::Modeling::ref_vector<boost::any> const& inputs) override;

      /// The proposal distribution
      std::shared_ptr<muq::Modeling::Gaussian> proposal;

      /// An any algebra
      std::shared_ptr<muq::Utilities::AnyAlgebra> algebra;

    };
    
  } // namespace SamplingAlgoirthms
} // namespace muq 

#endif
