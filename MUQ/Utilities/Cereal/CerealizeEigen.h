#ifndef CEREALIZEEIGEN_H_
#define CEREALIZEEIGEN_H_

#include "MUQ/config.h"

#if MUQ_HAS_PARCER

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <Eigen/Dense>
#include <fstream>

// adapted from: https://stackoverflow.com/questions/22884216/serializing-eigenmatrix-using-cereal-library

namespace cereal {
  template <class Archive, class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols> inline
    typename std::enable_if<traits::is_output_serializable<BinaryData<_Scalar>, Archive>::value, void>::type
    save(Archive & ar, Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> const & m) {
    int32_t rows = m.rows();
    int32_t cols = m.cols();
    ar(rows);
    ar(cols);
    ar(binary_data(m.data(), rows * cols * sizeof(_Scalar)));
  }
  
  template <class Archive, class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols> inline
    typename std::enable_if<traits::is_input_serializable<BinaryData<_Scalar>, Archive>::value, void>::type
    load(Archive & ar, Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> & m) {
    int32_t rows;
    int32_t cols;
    ar(rows);
    ar(cols);
    
    m.resize(rows, cols);
    
    ar(binary_data(m.data(), static_cast<std::size_t>(rows * cols * sizeof(_Scalar))));
  }
} // namespace cereal

#endif
#endif
