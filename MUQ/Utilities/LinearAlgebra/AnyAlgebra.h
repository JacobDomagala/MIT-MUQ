#ifndef ANYALGEBRA_H_
#define ANYALGEBRA_H_

#include <iostream>

#include "MUQ/config.h"

#include "MUQ/Utilities/LinearAlgebra/ScalarAlgebra.h"
#include "MUQ/Utilities/LinearAlgebra/EigenVectorAlgebra.h"

#if MUQ_HAS_SUNDIALS==1
// Sundials includes
#include <nvector/nvector_serial.h>
#include <sundials/sundials_dense.h> // definitions DlsMat DENSE_ELEM
#endif

namespace muq {
  namespace Utilities {
    /// Implement a generic way to do algebric operations on boost::any's
    class AnyAlgebra {
    public:

      /// Default constructor
      AnyAlgebra();

      /// The size of an object
      /**
	 For vectors/matrices, return the number of elements.
	 
	 For matrices, the default behavior is to return the total number of elements.  For dim=0, return the number of rows, for dim=1 return the number of colums
	 @param[in] obj We need the size of this object
	 @param[in] dim The dimension For matrices: for dim=-1 (default) return the total number of elements, for dim=0, return the number of rows, for dim=1, return the number of colums
	 \return The size
       */
      unsigned int Size(boost::any const& obj, int const dim=-1) const;

      /// The norm of an object
      /**
	 @param[in] obj We need the norm of this object
	 \return The norm
       */
      double Norm(boost::any const& obj) const;

      /// The inner product between two vectors
      /**
	 @param[in] vec1 The first vector
	 @param[in] vec2 The second vector
	 \return The inner product
       */
      double InnerProduct(boost::any const& vec1, boost::any const& vec2) const;

      /// Access an element of a vector/matrix
      /**
	 The return type is whatever the elements of the vector/matrix are (doubles, ints, ect ...)
	 @param[in] obj The vector/matrix whose data we want to access
	 @param[in] i We want to access the \f$i^{th}\f$ element/row of the vector/matrix (defaults to 0)
	 @param[in] j We want to access the \f$j^{th}\f$ col of the matrix (defaults to 0)
	 \return The \f$(i,j)^{th}}\f$ element of the vector/matrix
       */
      boost::any AccessElement(boost::any const& obj, unsigned int const i = 0, unsigned int const j = 0) const;

      /// Compute a zero vector
      /** 
	  @param[in] type We need a zero object of this type
	  @param[in] rows The size of the vector (defaults to 0 because some types have implied sizes (e.g., Eigen::Vector2d)) or number of rows of the matrix
	  @param[in] cols The number of columns in the matrix (defaults to 0 but, again, some types imply a size)
       */
      boost::any Zero(std::type_info const& type, unsigned int const rows = 0, unsigned int const cols = 0) const;

      /// Determine if an object is the zero object
      /**
	 @param[in] obj An input object
	 \return true: if obj is the zero object, false: if obj is not the zero object
       */
      bool IsZero(boost::any const& obj) const;

      /// Compute an identity object
      /**
	 If the input type is a vector (e.g., Eigen::Vector), return an identity matrix of corresponding type (e.g., Eigen::Matrix)
	 @param[in] type The type---return an identity of this type
	 @param[in] rows The number of rows (e.g., for a matrix) defaults to 0 since some types imply the size
	 @param[in] cols The number of columns (e.g., for a matrix) defaults to 0 since some types imply the size
	 \return An identity of some type
       */
      boost::any Identity(std::type_info const& type, unsigned int const rows=0, unsigned int const cols=0) const;
      
      /// Add two objects together
      /**
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The addition of in0 and in1 (in0+in1)
       */
      boost::any Add(boost::any const& in0, boost::any const& in1) const;

      /// Subtract two objects 
      /**
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The subtraction of in0 and in1 (in0-in1)
       */
      boost::any Subtract(boost::any const& in0, boost::any const& in1) const;

      /// Multiply two objects 
      /**
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The multiplication of in0 and in1 (in0-in1)
       */
      boost::any Multiply(boost::any const& in0, boost::any const& in1) const;

      /// Apply the inverse of a matrix
      /**
	 If the input is a vector, treat is as the diagonal of a matrix
	 @param[in] A We are applying the inverse of this matrix
	 @param[in] x We are applying the inverse to this vector
	 \return The result \f$y=A^{-1} x\f$
       */
      boost::any ApplyInverse(boost::any const& A, boost::any const& x) const;

      /// Apply a matrix (mat-vec)
      /**
	 If the input is a vector, treat is as the diagonal of a matrix
	 @param[in] A We are applying this matrix
	 @param[in] x We are applying the matrix to this vector
	 \return The result \f$y=A x\f$
       */
      boost::any Apply(boost::any const& A, boost::any const& x) const;

      /// The inverse
      /**
	 @param[in] obj We need the inverse of this object
	 \return The inverse
       */
      boost::any Inverse(boost::any const& obj) const;

    private:

      /// Determine if an Eigen::Matrix is zero 
      /**
	 @param[in] obj An input matrix
	 \return true: if obj is zero, false: if obj is not zero
       */
      bool IsEigenMatrixZero(boost::any const& obj) const;

      /// Determine if an Eigen::Matrix is zero 
      /**
	 @param[in] obj An input matrix
	 \return true: if obj is zero, false: if obj is not zero
       */
      template<typename EigenType>
	inline bool IsEigMatZero(boost::any const& obj) const {
	const EigenType& v = boost::any_cast<EigenType const&>(obj);

	return (v.array()==EigenType::Zero(v.rows(), v.cols()).array()).all();
      }
      
      /// Is a boost::any an Eigen::Matrix type?
      /**
	 @param[in] obj_type We want to know if this object type is a Eigen::Matrix
	 \return true: it is an Eigen::Matrix2d, Eigen::Matrix3d, Eigen::Matrix4d, or Eigen::MatrixXd, false: it is not an Eigen::Matrixn type
       */
      bool IsEigenMatrix(std::type_info const& obj_type) const;

#if MUQ_HAS_SUNDIALS==1
      /// Is a boost::any an N_Vector type?
      /**
	 @param[in] obj_type We want to know if this object type is a N_Vector type
	 \return true: it is an N_Vector, false: it is not an N_Vector type
       */
      bool IsSundialsVector(std::type_info const& obj) const;
#endif

      /// The norm of an Eigen::Vector or Eigen::Matrix
      /**
	 @param[in] obj We will get the norm of this vector/matrix
	 \return The norm
       */
      template<typename EigenType>
	inline double EigenNorm(boost::any const& obj) const {
	const EigenType& x = boost::any_cast<EigenType const&>(obj);
	return x.norm();
      }

      /// The norm of an Eigen::Matrix
      /**
	 @param[in] mat We will get the norm of this matrix
	 \return The norm
       */
      double EigenMatrixNorm(boost::any const& mat) const;

      /// The size of an Eigen::Matrix
      /**
	 @param[in] mat We will get the size of this matrix
	 @param[in] dim The dimension For matrices: for dim=-1 return the total number of elements, for dim=0, return the number of rows, for dim=1, return the number of colums
	 \return The size
       */
      unsigned int EigenMatrixSize(boost::any const& mat, int const dim) const;

#if MUQ_HAS_SUNDIALS==1
      /// The size of an N_Vector
      /**
	 @param[in] vec We will get the size of this vector
	 \return The size
       */
      unsigned int SundialsVectorSize(boost::any const& vec) const;
#endif

      /// The size of an object (implemented by a child for non standard types)
      /**
	 For vectors/matrices, return the number of elements.
	 @param[in] obj We need the size of this object
	 \return The size
       */
      virtual unsigned int SizeImpl(boost::any const& obj) const;
      
      /// The norm of an object
      /**
	 @param[in] obj We need the norm of this object
	 \return The norm
       */
      virtual double NormImpl(boost::any const& obj) const;
            
      /// The inner product between two vectors
      /**
	 @param[in] vec1 The first vector
	 @param[in] vec2 The second vector
	 \return The inner product
       */
      virtual double InnerProductImpl(boost::any const& vec1, boost::any const& vec2) const;      

#if MUQ_HAS_SUNDIALS==1
      /// Access an element of a Sundials vector
      /**
	 @param[in] vec The vector whose data we want to access
	 @param[in] i We want to access the \f$i^{th}\f$ element of the vector
	 \return The \f$i^{th}\f$ element of the vector
       */
      boost::any AccessSundialsVector(N_Vector const& obj, unsigned int const i) const;
#endif

      /// Access an element of an Eigen::Matrix
      /**
	 The return type is whatever the elements of the vector are (doubles, ints, ect ...)
	 @param[in] mat The matrix whose data we want to access
	 @param[in] i We want to access the \f$i^{th}\f$ row of the vector
	 @param[in] j We want to access the \f$j^{th}\f$ col of the vector
	 \return The \f$(i,j)^{th}\f$ element of the vector
       */
      boost::any AccessEigenMatrix(boost::any const& mat, unsigned int const i, unsigned int const j) const;

      /// Access an element of an Eigen::Matrix
      /**
	 The return type is whatever the elements of the vector are (doubles, ints, ect ...)
	 @param[in] mat The matrix whose data we want to access
	 @param[in] i We want to access the \f$i^{th}\f$ row of the vector
	 @param[in] j We want to access the \f$j^{th}\f$ col of the vector
	 \return The \f$(i,j)^{th}\f$ element of the vector
       */
      template<typename mattype>
	inline boost::any AccessEigenMat(boost::any const& mat, unsigned int const i, unsigned int const j) const {
	// get a constant reference to the matrix
	const mattype& matref = boost::any_cast<const mattype&>(mat);
	
	// check the size
	assert(i<matref.rows());
	assert(j<matref.cols());
	
	// return ith element
	return matref(i,j);
      }

      /// Access an element of a vector
      /**
	 MUQ automatically checks for some common input types.  However, the user may need to overload this function for special types.
	 @param[in] vec The vector whose data we want to access
	 @param[in] i We want to access the \f$i^{th}\f$ element/row
	 \return The \f$i^{th}\f$ element/row of the vector
       */
      virtual boost::any AccessElementImpl(boost::any const& vec, unsigned int const i) const;

      /// Compute an identity Eigen::Matrix
      /**
	 @param[in] type The type---return an identity of this type
	 @param[in] rows The number of rows (e.g., for a matrix)
	 @param[in] cols The number of columns (e.g., for a matrix) 
	 \return An identity of some type
       */
      boost::any EigenMatrixIdentity(std::type_info const& type, unsigned int const rows, unsigned int const cols) const;

      /// Compute an identity object 
      /**
	 @param[in] type The type---return an identity of this type
	 @param[in] rows The number of rows (e.g., for a matrix)
	 @param[in] cols The number of columns (e.g., for a matrix) 
	 \return An identity of some type
       */
      virtual boost::any IdentityImpl(std::type_info const& type, unsigned int const rows, unsigned int const cols) const;
      
      /// Add two Eigen::Matrices together
      /**
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The addition of in0 and in1 (in0+in1)
       */
      boost::any AddEigenMatrix(boost::any const& in0, boost::any const& in1) const;

      /// Add two Eigen::Matrices together
      /**
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The addition of in0 and in1 (in0+in1)
       */
      template<typename type0, typename type1>
	inline boost::any AddEigenMatrix(boost::any const& in0, boost::any const& in1) const {
	const type0& x0 = boost::any_cast<type0 const&>(in0);
	const type1& x1 = boost::any_cast<type1 const&>(in1);
	assert(x0.rows()==x1.rows());
	assert(x0.cols()==x1.cols());

	return (type0)(x0+x1);
      }

      /// Add two objects together
      /**
	 MUQ automatically checks for some common pairs.  However, the user may need to overload this function for special types.
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The addition of in0 and in1 (in0+in1)
       */
      virtual boost::any AddImpl(boost::any const& in0, boost::any const& in1) const;
      
      /// Subtract two Eigen::Matrices together
      /**
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The subtraction of in0 and in1 (in0+in1)
       */
      boost::any SubtractEigenMatrix(boost::any const& in0, boost::any const& in1) const;

      /// Subtract two Eigen::Matrices
      /**
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The subtraction of in0 and in1 (in0-in1)
       */
      template<typename type0, typename type1>
	inline boost::any SubtractEigenMatrix(boost::any const& in0, boost::any const& in1) const {
	const type0& x0 = boost::any_cast<type0 const&>(in0);
	const type1& x1 = boost::any_cast<type1 const&>(in1);
	assert(x0.rows()==x1.rows());
	assert(x0.cols()==x1.cols());

	return (type0)(x0-x1);
      }

      /// Subtract two objects 
      /**
	 MUQ automatically checks for some common pairs.  However, the user may need to overload this function for special types.
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The subtraction of in0 and in1 (in0-in1)
       */
      virtual boost::any SubtractImpl(boost::any const& in0, boost::any const& in1) const;

      /// Multiply two Eigen::Matrices together
      /**
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The multiplication of in0 and in1 (in0+in1)
       */
      boost::any MultiplyEigenMatrix(boost::any const& in0, boost::any const& in1) const;

      /// Multiply two Eigen::Matrices
      /**
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The multiplication of in0 and in1 (in0-in1)
       */
      template<typename type0, typename type1>
	inline boost::any MultiplyEigenMatrix(boost::any const& in0, boost::any const& in1) const {
	const type0& x0 = boost::any_cast<type0 const&>(in0);
	const type1& x1 = boost::any_cast<type1 const&>(in1);
	assert(x0.cols()==x1.rows());

	return (type0)(x0*x1);
      }

      /// Multiply Eigen::Matrix times scalars 
      /**
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The multiplication of in0 and in1 (in0*in1)
       */
      boost::any MultiplyEigenMatrixScalar(boost::any const& in0, boost::any const& in1) const;

      /// Multiply Eigen::Matrix times scalars 
      /**
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The multiplication of in0 and in1 (in0*in1)
       */
      template<typename type0, typename type1> 
	inline boost::any MultiplyEigenScalar(boost::any const& in0, boost::any const& in1) const {
	const type0 x0 = boost::any_cast<type0 const>(in0);
	const type1 x1 = boost::any_cast<type1 const>(in1);
	
	return (type1)(x0 * x1);
      }

      /// Multiply two objects 
      /**
	 MUQ automatically checks for some common pairs.  However, the user may need to overload this function for special types.
	 @param[in] in0 The first input
	 @param[in] in1 The second input
	 \return The multiplication of in0 and in1 (in0*in1)
       */
      virtual boost::any MultiplyImpl(boost::any const& in0, boost::any const& in1) const;

      /// Apply a matrix (mat-vec)
      /**
	 If the input is a vector, treat is as the diagonal of a matrix
	 @param[in] A We are applying this matrix
	 @param[in] x We are applying the matrix to this vector
	 \return The result \f$y=A x\f$
       */
      virtual boost::any ApplyImpl(boost::any const& A, boost::any const& x) const;
      
      /// Apply the inverse of a matrix
      /**
	 If the input is a vector, treat is as the diagonal of a matrix
	 @param[in] A We are applying the inverse of this matrix
	 @param[in] x We are applying the inverse to this vector
	 \return The result \f$y=A^{-1} x\f$
       */
      virtual boost::any ApplyInverseImpl(boost::any const& A, boost::any const& x) const;

      /// Compute a zero object for boost::any
      /** 
	  @param[in] type We need a zero object of this type
	  @param[in] rows The size of the vector (defaults to 0 because some types have implied sizes (e.g., Eigen::Vector2d)) or number of rows of the matrix
	  @param[in] cols The number of columns in the matrix (defaults to 0 but, again, some types imply a size)
       */
      virtual boost::any ZeroImpl(std::type_info const& type, unsigned int const rows, unsigned int const cols) const;

      /// Compute a zero Eigen::Matrix
      /** 
	  @param[in] type We need a zero object of this EigenMatrixType
	  @param[in] rows The number of rows
	  @param[in] cols The number of columns
       */
      boost::any ZeroEigenMatrix(std::type_info const& type, unsigned int const rows, unsigned int const cols) const;

      /// Compute a zero scalar
      /** 
	  @param[in] type We need a zero object of this type
       */
      boost::any ZeroScalar(std::type_info const& type) const;

      /// Determine if an object is the zero object
      /**
	 @param[in] obj An input object
	 \return true: if obj is the zero object, false: if obj is not the zero object
       */
      virtual bool IsZeroImpl(boost::any const& obj) const;

      /// The inverse
      /**
	 @param[in] obj We need the inverse of this object
	 \return The inverse
       */
      virtual boost::any InverseImpl(boost::any const& obj) const;

    };
  } // namespace Utilities
} // namespace muq

#endif
