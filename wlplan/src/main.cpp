#include "../include/data/dataset.hpp"
#include "../include/feature_generation/wl_features.hpp"
#include "../include/graph/ilg_generator.hpp"
#include "../include/planning/domain.hpp"
#include "../include/planning/object.hpp"
#include "../include/planning/predicate.hpp"
#include "../include/planning/problem.hpp"

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/typing.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;
using namespace py::literals;

// clang-format off
PYBIND11_MODULE(_wlplan, m) {
m.doc() = "WLPlan: WL Features for PDDL Planning";

/* Planning */
auto planning_m = m.def_submodule("planning");
auto atom = py::class_<planning::Atom>(planning_m, "Atom", 
R"(Parameters
----------
    predicate : Predicate
        Predicate object.

    objects : List[Object]
        List of object names.
)");
auto domain = py::class_<planning::Domain>(planning_m, "Domain", 
R"(Parameters
----------
    name : str
        Domain name.

    predicates : List[Predicate]
        List of predicates.

    constant_objects : List[Object], optional
        List of constant objects.
)");
auto object = py::class_<planning::Object>(planning_m, "Object", 
R"(Object is a type alias for a str.
)");
auto predicate = py::class_<planning::Predicate>(planning_m, "Predicate", 
R"(Parameters
----------
    name : str
        Predicate name.

    arity : int
        Predicate arity.
)");
auto problem = py::class_<planning::Problem>(planning_m, "Problem", 
R"(Parameters
----------
    domain : Domain
        Domain object.

    objects : List[Object]
        List of object names.

    positive_goals : List[Atom]
        List of positive goal atoms.

    negative_goals : List[Atom]
        List of negative goal atoms.
)");
auto state = py::class_<planning::State>(planning_m, "State", 
R"(State is a type alias for a list of Atoms.
)");

predicate
  .def(py::init<std::string &, int>(), 
        "name"_a, "arity"_a)
  .def("__repr__", &::planning::Predicate::to_string)
  .def("__eq__", &::planning::Predicate::operator==);

domain
  .def(py::init<std::string &, std::vector<planning::Predicate>, std::vector<planning::Object>>(), 
        "name"_a, "predicates"_a, "constant_objects"_a)
  .def(py::init<std::string &, std::vector<planning::Predicate>>(), 
        "name"_a, "predicates"_a)
  .def("__repr__", &::planning::Domain::to_string)
  .def("__eq__", &::planning::Domain::operator==);

atom
  .def(py::init<planning::Predicate &, std::vector<std::string> &>(), 
        "predicate"_a, "objects"_a)
  .def("__repr__", &::planning::Atom::to_string)
  .def("__eq__", &::planning::Atom::operator==);

problem
  .def(py::init<planning::Domain &, std::vector<std::string> &, std::vector<planning::Atom> &, std::vector<planning::Atom> &>(), 
        "domain"_a, "objects"_a, "positive_goals"_a, "negative_goals"_a);

/* Data */
auto data_m = m.def_submodule("data");
auto dataset = py::class_<data::Dataset>(data_m, "Dataset", 
R"(WLPlan dataset object.

Datasets contain a domain and a list of problem states.

Parameters
----------
    domain : Domain
        Domain object.

    data : List[ProblemStates]
        List of problem states.
)");
auto problem_states = py::class_<data::ProblemStates>(data_m, "ProblemStates", 
R"(Stores a problem and training states for the problem.

Upon initialisation, the problem and states are checked for consistency.

Parameters
----------
    problem : Problem
        Problem object.

    states : List[State]
        List of training states.
)");


problem_states
  .def(py::init<planning::Problem &, std::vector<planning::State> &>(), 
        "problem"_a, "states"_a);

dataset
  .def(py::init<planning::Domain &, std::vector<data::ProblemStates> &>(), 
        "domain"_a, "data"_a);

/* Graph */
auto graph_m = m.def_submodule("graph");
auto graph = py::class_<graph::Graph>(graph_m, "Graph", 
R"(WLPlan graph object.

Graphs have integer node colours and edge labels.

Parameters
----------
    node_colours : List[int]
        List of node colours, where `node[i]` is the colour of node `i` indexed from 0.

    node_names : List[str], optional
        List of node names, where `node_names[i]` is the name of node `i` indexed from 0.

    edges : List[Tuple[int, int]]
        List of labelled edges, where `edges[u] = [(r_1, v_1), ..., (r_k, v_k)]` represents edges from node `u` to nodes `v_1, ..., v_k` with labels `r_1, ..., r_k`, respectively. WLPlan graphs are directed so users must ensure that edges are undirected.

Attributes
----------
    node_colours : List[int]
        List of node colours.

    edges : List[Tuple[int, int]]
        List of labelled edges.

Methods
-------
    get_node_name(u: int) -> str
        Get the name of node `u`.

    dump() -> None
        Print the graph representation.
)");
// auto ilg_generator = py::class_<graph::ILGGenerator>(graph_m, "ILGGenerator");

graph
  .def(py::init<std::vector<int>, std::vector<std::vector<std::pair<int, int>>>>(), 
        "node_colours"_a, "edges"_a)
  .def(py::init<std::vector<int>, std::vector<std::string>, std::vector<std::vector<std::pair<int, int>>>>(), 
        "node_colours"_a, "node_names"_a, "edges"_a)
  .def_readonly("node_colours", &graph::Graph::nodes, ":meta private:")
  .def_readonly("edges", &graph::Graph::edges, ":meta private:")
  .def("get_node_name", &graph::Graph::get_node_name, "u"_a, ":meta private:")
  .def("dump", &graph::Graph::dump, ":meta private:")
  .def("__repr__", &::graph::Graph::to_string, ":meta private:");

/* Feature generation */
auto feature_generation_m = m.def_submodule("feature_generation");
auto wl_features = py::class_<feature_generation::WLFeatures>(feature_generation_m, "_WLFeatures");

wl_features
  .def(py::init<const std::string &>(), 
        "filename"_a)
  .def(py::init<planning::Domain &, std::string, int, std::string, bool>(), 
        "domain"_a, "graph_representation"_a, "iterations"_a, "prune_features"_a, "multiset_hash"_a)
  .def("collect", py::overload_cast<const data::Dataset>(&feature_generation::WLFeatures::collect),
        "dataset"_a)
  .def("collect", py::overload_cast<const std::vector<graph::Graph> &>(&feature_generation::WLFeatures::collect),
        "graphs"_a)
  .def("set_problem", &feature_generation::WLFeatures::set_problem,
        "problem"_a)
  .def("get_string_representation", py::overload_cast<const feature_generation::Embedding &>(&feature_generation::WLFeatures::get_string_representation),
        "embedding"_a)
  .def("get_string_representation", py::overload_cast<const planning::State &>(&feature_generation::WLFeatures::get_string_representation),
        "state"_a)
  .def("embed", py::overload_cast<const data::Dataset &>(&feature_generation::WLFeatures::embed), 
        "dataset"_a)
  .def("embed", py::overload_cast<const std::vector<graph::Graph> &>(&feature_generation::WLFeatures::embed),
        "graphs"_a)
  .def("embed", py::overload_cast<const planning::State &>(&feature_generation::WLFeatures::embed),
        "state"_a)
  .def("get_n_features", &feature_generation::WLFeatures::get_n_features)
  .def("get_seen_counts", &feature_generation::WLFeatures::get_seen_counts)
  .def("get_unseen_counts", &feature_generation::WLFeatures::get_unseen_counts)
  .def("get_n_seen_graphs", &feature_generation::WLFeatures::get_n_seen_graphs)
  .def("get_n_seen_nodes", &feature_generation::WLFeatures::get_n_seen_nodes)
  .def("get_n_seen_edges", &feature_generation::WLFeatures::get_n_seen_edges)
  .def("get_n_seen_initial_colours", &feature_generation::WLFeatures::get_n_seen_initial_colours)
  .def("get_n_seen_refined_colours", &feature_generation::WLFeatures::get_n_seen_refined_colours)
  .def("set_weights", &feature_generation::WLFeatures::set_weights,
        "weights"_a)
  .def("get_weights", &feature_generation::WLFeatures::get_weights)
  .def("predict", py::overload_cast<const graph::Graph &>(&feature_generation::WLFeatures::predict),
        "graph"_a)
  .def("predict", py::overload_cast<const planning::State &>(&feature_generation::WLFeatures::predict),
        "state"_a)
  .def("save", &feature_generation::WLFeatures::save);

/* Version */
#ifdef WLPLAN_VERSION
  m.attr("__version__") = MACRO_STRINGIFY(WLPLAN_VERSION);
#else
  m.attr("__version__") = "dev";
#endif
}
// clang-format on
