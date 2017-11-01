#ifndef ANYALGEBRA_H_
#define ANYALGEBRA_H_

#include <iostream>
#include <assert.h>

#include "boost/none.hpp"
#include "boost/any.hpp"

#include <Eigen/Core>

#include "MUQ/config.h"

#if MUQ_HAS_SUNDIALS==1
// Sundials includes
#include <nvector/nvector_serial.h>
#include <sundials/sundials_dense.h> // definitions DlsMat DENSE_ELEM
#endif

namespace muq {
  namespace Modeling {
    /// Implement a generic way to do algebric operations on boost::any's
    class AnyAlgebra {
    public:

      /// Default constructor
      AnyAlgebra();

      /// The dimension of a vector
      /**
	 @param[in] vec We need the size/dimension of this vector
	 \return The dimension of the input vector
       */
      unsigned int VectorDimensionBase(boost::any const& vec) const;

      /// The norm of an object
      /**
	 @param[in] obj We need the norm of this object
	 \return The norm
       */
      double NormBase(boost::any const& obj) const;

      /// Access an element of a vector
      /**
	 The return type is whatever the elements of the vector are (doubles, ints, ect ...)
	 @param[in] i We want to access the \f$i^{th}\f$ element of a vector
	 @param[in] vec The vector whose data we want to access
	 \return The \f$i^{th}\f$ element of the vector
       */
      boost::any AccessElementBase(unsigned int i, boost::any const& vec) const;

      /// Compute a zero vector
      /** 
	  @param[in] type We need a zero object of this type
	  @param[in] size The size of the vector 
       */
      boost::any ZeroVectorBase(std::string const& type, unsigned int const size) const;

      /// Determine if an object is the zero object
      /**
	 @param[in] obj An input object
	 \return true: if vec is the zero object, false: if vec is not the zero object
       */
      bool IsZeroBase(boost::any const& obj) const;

      /// Compute an identity object for boost::any
      /**
	 For example, if the underlying type is a double this would return 1.0, if the underlying type is an Eigen::VectorXd this would return Eigen::MatrixXd::Zero(N).
	 @param[in] in The input --- its type and size determines the return type and size
	 \return An identity of some type
       */
      boost::any IdentityBase(std::reference_wrapper<const boost::any> const& in) const;
      
      /// Add two objects together
      /**
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The addition of in0 and in1 (in0+in1)
       */
      boost::any AddBase(std::reference_wrapper<const boost::any> const& in0, std::reference_wrapper<const boost::any> const& in1) const;

      /// Subtract one object from another
      /**
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The subtraction of in0 and in1 (in0-in1)
       */
      boost::any SubtractBase(std::reference_wrapper<const boost::any> const& in0, std::reference_wrapper<const boost::any> const& in1) const;

      /// Multiply two objects 
      /**
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The multiplication of in0 and in1 (in0*in1) --- order matters!
       */
      boost::any MultiplyBase(std::reference_wrapper<const boost::any> const& in0, std::reference_wrapper<const boost::any> const& in1) const;

    private:

      /// The norm of an object
      /**
	 @param[in] obj We need the norm of this object
	 \return The norm
       */
      virtual double Norm(boost::any const& obj) const;

      /// The dimension of a vector
      /**
	 MUQ automatically checks for some common input types.  However, the user may need to overload this function for special types.
	 @param[in] vec We need the size/dimension of this vector
	 \return The dimension of the input vector
       */
      virtual unsigned int VectorDimension(boost::any const& vec) const;

      /// Access an element of a vector
      /**
	 MUQ automatically checks for some common input types.  However, the user may need to overload this function for special types.
	 @param[in] i We want to access the \f$i^{th}\f$ element of a vector
	 @param[in] vec The vector whose data we want to access
	 \return The \f$i^{th}\f$ element of the vector
       */
      virtual boost::any AccessElement(unsigned int i, boost::any const& vec) const;

      /// Compute an identity object for boost::any
      /**
	 MUQ automatically checks for some common input types.  However, the user may need to overload this function for special types.
	 @param[in] in The input --- its type and size determines the return type and size
	 \return An identity of some type
       */
      virtual boost::any Identity(std::reference_wrapper<const boost::any> const& in) const;

      /// Add two objects together
      /**
	 MUQ automatically checks for some common pairs.  However, the user may need to overload this function for special types.
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The addition of in0 and in1 (in0+in1)
       */
      virtual boost::any Add(std::reference_wrapper<const boost::any> const& in0, std::reference_wrapper<const boost::any> const& in1) const;


      /// Add two objects together
      /**
	 MUQ automatically checks for some common pairs.  However, the user may need to overload this function for special types.
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The subtraction of in0 and in1 (in0-in1)
       */
      virtual boost::any Subtract(std::reference_wrapper<const boost::any> const& in0, std::reference_wrapper<const boost::any> const& in1) const;

/// Multiply two objects 
      /**
	 MUQ automatically checks for some common pairs.  However, the user may need to overload this function for special types.
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The multiplication of in0 and in1 (in0*in1) --- order matters!
       */
      boost::any Multiply(std::reference_wrapper<const boost::any> const& in, std::reference_wrapper<const boost::any> const& out) const;

      /// Compute a zero object for boost::any
      /** 
	  @param[in] type We need a zero object of this type
	  @param[in] size The size of the vector 
       */
      virtual boost::any ZeroVector(std::string const& type, unsigned int const size) const;

      /// Determine if an object is the zero object
      /**
	 @param[in] obj An input object
	 \return true: if vec is the zero object, false: if vec is not the zero object
       */
      virtual bool IsZero(boost::any const& obj) const;

      /// The name of a (2 dimensional) Eigen::Vector2d
      const std::string eigenVec2Type = typeid(Eigen::Vector2d).name();

      /// The name of a (3 dimensional) Eigen::Vector3d
      const std::string eigenVec3Type = typeid(Eigen::Vector3d).name();

      /// The name of a (4 dimensional) Eigen::Vector4d
      const std::string eigenVec4Type = typeid(Eigen::Vector4d).name();

      /// The name of a Eigen::VectorXd
      const std::string eigenVecType = typeid(Eigen::VectorXd).name();

      /// The name of a Eigen::MatrixXd
      const std::string eigenMatType = typeid(Eigen::MatrixXd).name();

      /// The name of a double
      const std::string doubleType = typeid(double).name();

      /// The name of a boost::none
      const std::string noneType = typeid(boost::none).name();

#if MUQ_HAS_SUNDIALS==1
      /// The name of a N_Vector
      const std::string N_VectorType = typeid(N_Vector).name();
#endif
    };
  } // namespace Modeling
} // namespace muq

#endif
