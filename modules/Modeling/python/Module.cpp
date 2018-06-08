#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>

#include "AllClassWrappers.h"

using namespace muq::Modeling::PythonBindings;
namespace py = pybind11;


PYBIND11_PLUGIN(pymuqModeling) {
    py::module m("pymuqModeling", "Python bindings for the muqModeling library.");

    WorkPieceWrapper(m);
    ModPieceWrapper(m);
    DistributionWrapper(m);
    CwiseUnaryOperatorsWrapper(m);

    return m.ptr();
}
