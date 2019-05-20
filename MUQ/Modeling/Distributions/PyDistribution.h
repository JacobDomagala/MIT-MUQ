#ifndef PYDISTRIBUTION_H_
#define PYDISTRIBUTION_H_

#include "MUQ/Modeling/Distributions/Distribution.h"

namespace muq {
  namespace Modeling {
    class PyDistribution : public Distribution {
    public:
      PyDistribution(unsigned int varSizeIn, Eigen::VectorXi const& hyperSizesIn = Eigen::VectorXi());

    protected:

      virtual Eigen::VectorXd SampleImpl(ref_vector<Eigen::VectorXd> const& inputs) override;

      virtual Eigen::VectorXd SampleImpl(std::vector<Eigen::VectorXd> const& inputs) = 0;

      virtual double LogDensityImpl(ref_vector<Eigen::VectorXd> const& inputs) override;

      virtual double LogDensityImpl(std::vector<Eigen::VectorXd> const& inputs) = 0;

    private:

      std::vector<Eigen::VectorXd> ToStdVec(ref_vector<Eigen::VectorXd> const& input);
    };
  } // namespace Modeling
} // namespace muq

#endif
