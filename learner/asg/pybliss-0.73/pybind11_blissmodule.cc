#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "bliss-0.73/digraph_wrapper.h"

using namespace std;
using namespace pybind11::literals;
namespace py = pybind11;

PYBIND11_PLUGIN(pybind11_blissmodule) {
    py::module m("pybind11_blissmodule", "pybind11 bliss module");

    py::class_<DigraphWrapper>(m, "DigraphWrapper")
        .def(py::init<>())
        .def("add_vertex", &DigraphWrapper::add_vertex, "doc",
            "color"_a)
        .def("add_edge", &DigraphWrapper::add_edge, "doc",
            "v1"_a, "v2"_a)
        .def("find_automorphisms", &DigraphWrapper::find_automorphisms,
            "doc", "time_limit"_a);

    return m.ptr();
}
