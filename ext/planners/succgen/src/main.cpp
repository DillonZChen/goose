#include "../include/planning/atom_packer.hpp"
#include "../include/planning/effects.hpp"
#include "../include/planning/goal.hpp"
#include "../include/planning/ground_expression.hpp"
#include "../include/planning/lifted_expression.hpp"
#include "../include/planning/node.hpp"
#include "../include/planning/numeric_effect.hpp"
#include "../include/planning/state.hpp"
#include "../include/planning/state_storer.hpp"
#include "../include/planning/visited_storage.hpp"

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/typing.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;
using namespace py::literals;

PYBIND11_MODULE(_succgen, m) {
  m.doc() = "Lifted Numeric Planning";

  auto planning_m = m.def_submodule("planning");

  // Node
  py::class_<planning::Node>(planning_m, "Node")
      .def(py::init<planning::SGState &, std::pair<int, std::vector<int>>, int, int>(),
           "state"_a,
           "achieving_action"_a,
           "s_id"_a,
           "parent_s_id"_a)
      .def_readonly("state", &planning::Node::state)
      .def_readonly("achieving_action", &planning::Node::achieving_action)
      .def_readonly("s_id", &planning::Node::s_id)
      .def_readonly("parent_s_id", &planning::Node::parent_s_id);

  // State
  py::class_<planning::SGState>(planning_m, "SGState")
      .def(py::init<planning::Atoms &, planning::Values &>(), "atoms"_a, "values"_a)
      .def("get_copy", &planning::SGState::get_copy)
      .def("add_atom", &planning::SGState::add_atom, "atom"_a)
      .def("del_atom", &planning::SGState::del_atom, "atom"_a)
      .def("set_value", &planning::SGState::set_value, "index"_a, "value"_a)
      .def(
          "apply_action", &planning::SGState::apply_action, "action"_a, "instantiation"_a, "atom_packer"_a, "nvars_map"_a)
      .def_readonly("atoms", &planning::SGState::atoms)
      .def_readonly("values", &planning::SGState::values);

  // Goal
  py::class_<planning::SGGoal>(planning_m, "SGGoal")
      .def(py::init<planning::Atoms &,
                    planning::Atoms &,
                    std::vector<std::shared_ptr<planning::GroundBooleanExpression>> &>(),
           "pos_goals"_a,
           "neg_goals"_a,
           "numeric_goals"_a)
      .def("satisfied_by", &planning::SGGoal::satisfied_by, "state"_a)
      .def_readonly("pos_goals", &planning::SGGoal::pos_goals)
      .def_readonly("neg_goals", &planning::SGGoal::neg_goals)
      .def_readonly("numeric_goals", &planning::SGGoal::numeric_goals);

  // Effects
  py::class_<planning::Effects>(planning_m, "Effects")
      .def(py::init<std::vector<planning::Atom> &,
                    std::vector<planning::Atom> &,
                    std::vector<std::shared_ptr<planning::NumericEffect>>>(),
           "adds"_a,
           "dels"_a,
           "numeric_effects"_a);

  // AtomPacker
  py::class_<planning::AtomPacker>(planning_m, "AtomPacker")
      .def(py::init<>())
      .def("pack", &planning::AtomPacker::pack, "predicate_index"_a, "instantiation"_a)
      .def("unpack", &planning::AtomPacker::unpack, "index"_a);

  // StateStorer
  py::class_<planning::StateStorer>(planning_m, "StateStorer")
      .def(py::init<>())
      .def("add", &planning::StateStorer::add, "state"_a)
      .def("contains", &planning::StateStorer::contains, "state"_a);

  // VisitedStorage
  py::class_<planning::VisitedStorage>(planning_m, "VisitedStorage")
      .def(py::init<>())
      .def("add", &planning::VisitedStorage::add, "node"_a)
      .def("get", &planning::VisitedStorage::get, "s_id"_a)
      .def("contains", &planning::VisitedStorage::contains, "state"_a);

  // FluentValueMap
  py::class_<planning::FluentValueMap>(planning_m, "FluentValueMap")
      .def(py::init<std::vector<std::pair<std::pair<int, std::vector<int>>, double>>>(), "fluent_value_map"_a);

  // FluentIndexMap
  py::class_<planning::FluentIndexMap>(planning_m, "FluentIndexMap")
      .def(py::init<std::vector<std::pair<std::pair<int, std::vector<int>>, int>>>(), "fluent_index_map"_a);

  // NumericEffect
  //   https://github.com/pybind/pybind11/issues/956#issuecomment-317022720
  py::class_<planning::NumericEffect, std::shared_ptr<planning::NumericEffect>>(planning_m, "NumericEffect")
      .def("apply", &planning::NumericEffect::apply, "input"_a, "evaluation"_a);
  py::class_<planning::IncreaseEffect, planning::NumericEffect, std::shared_ptr<planning::IncreaseEffect>>(
      planning_m, "IncreaseEffect")
      .def(py::init<int, std::vector<int>, std::shared_ptr<planning::LiftedExpression>>(),
           "table"_a,
           "row"_a,
           "expression"_a);
  py::class_<planning::DecreaseEffect, planning::NumericEffect, std::shared_ptr<planning::DecreaseEffect>>(
      planning_m, "DecreaseEffect")
      .def(py::init<int, std::vector<int>, std::shared_ptr<planning::LiftedExpression>>(),
           "table"_a,
           "row"_a,
           "expression"_a);
  py::class_<planning::AssignEffect, planning::NumericEffect, std::shared_ptr<planning::AssignEffect>>(planning_m,
                                                                                                       "AssignEffect")
      .def(py::init<int, std::vector<int>, std::shared_ptr<planning::LiftedExpression>>(),
           "table"_a,
           "row"_a,
           "expression"_a);

  // LiftedExpression
  py::class_<planning::LiftedExpression, std::shared_ptr<planning::LiftedExpression>>(planning_m, "LiftedExpression")
      .def("evaluate", &planning::LiftedExpression::evaluate, "instantiation"_a, "nvars_map"_a, "nvals"_a);
  py::class_<planning::LiftedValue, planning::LiftedExpression, std::shared_ptr<planning::LiftedValue>>(planning_m,
                                                                                                        "LiftedValue")
      .def(py::init<double>(), "value"_a);
  py::class_<planning::LiftedFunction, planning::LiftedExpression, std::shared_ptr<planning::LiftedFunction>>(
      planning_m, "LiftedFunction")
      .def(py::init<planning::Fluent>(), "fluent"_a);
  py::class_<planning::StaticLiftedFunction,
             planning::LiftedExpression,
             std::shared_ptr<planning::StaticLiftedFunction>>(planning_m, "StaticLiftedFunction")
      .def(py::init<planning::Fluent, planning::FluentValueMap>(), "fluent"_a, "values"_a);
  py::class_<planning::LiftedPlus, planning::LiftedExpression, std::shared_ptr<planning::LiftedPlus>>(planning_m,
                                                                                                      "LiftedPlus")
      .def(py::init<std::shared_ptr<planning::LiftedExpression>, std::shared_ptr<planning::LiftedExpression>>(),
           "lhs"_a,
           "rhs"_a);
  py::class_<planning::LiftedMinus, planning::LiftedExpression, std::shared_ptr<planning::LiftedMinus>>(planning_m,
                                                                                                        "LiftedMinus")
      .def(py::init<std::shared_ptr<planning::LiftedExpression>, std::shared_ptr<planning::LiftedExpression>>(),
           "lhs"_a,
           "rhs"_a);
  py::class_<planning::LiftedTimes, planning::LiftedExpression, std::shared_ptr<planning::LiftedTimes>>(planning_m,
                                                                                                        "LiftedTimes")
      .def(py::init<std::shared_ptr<planning::LiftedExpression>, std::shared_ptr<planning::LiftedExpression>>(),
           "lhs"_a,
           "rhs"_a);
  py::class_<planning::LiftedDivide, planning::LiftedExpression, std::shared_ptr<planning::LiftedDivide>>(
      planning_m, "LiftedDivide")
      .def(py::init<std::shared_ptr<planning::LiftedExpression>, std::shared_ptr<planning::LiftedExpression>>(),
           "lhs"_a,
           "rhs"_a);

  // GroundBooleanExpression
  py::class_<planning::GroundBooleanExpression, std::shared_ptr<planning::GroundBooleanExpression>>(
      planning_m, "GroundBooleanExpression")
      .def("eval_bool", &planning::GroundBooleanExpression::eval_bool, "values"_a)
      .def("eval_error", &planning::GroundBooleanExpression::eval_error, "values"_a);
  py::class_<planning::GroundNot, planning::GroundBooleanExpression, std::shared_ptr<planning::GroundNot>>(planning_m,
                                                                                                           "GroundNot")
      .def(py::init<std::shared_ptr<planning::GroundBooleanExpression>>(), "expr"_a);
  py::class_<planning::GroundOr, planning::GroundBooleanExpression, std::shared_ptr<planning::GroundOr>>(planning_m,
                                                                                                         "GroundOr")
      .def(py::init<std::vector<std::shared_ptr<planning::GroundBooleanExpression>>>(), "exprs"_a);
  py::class_<planning::GroundBinaryBooleanCondition,
             planning::GroundBooleanExpression,
             std::shared_ptr<planning::GroundBinaryBooleanCondition>>(planning_m, "GroundBinaryBooleanCondition");
  py::class_<planning::GroundLessThan,
             planning::GroundBinaryBooleanCondition,
             std::shared_ptr<planning::GroundLessThan>>(planning_m, "GroundLessThan")
      .def(py::init<std::shared_ptr<planning::GroundNumericExpression>,
                    std::shared_ptr<planning::GroundNumericExpression>>(),
           "lhs"_a,
           "rhs"_a);
  py::class_<planning::GroundLessThanEqualTo,
             planning::GroundBinaryBooleanCondition,
             std::shared_ptr<planning::GroundLessThanEqualTo>>(planning_m, "GroundLessThanEqualTo")
      .def(py::init<std::shared_ptr<planning::GroundNumericExpression>,
                    std::shared_ptr<planning::GroundNumericExpression>>(),
           "lhs"_a,
           "rhs"_a);
  py::class_<planning::GroundGreaterThan,
             planning::GroundBinaryBooleanCondition,
             std::shared_ptr<planning::GroundGreaterThan>>(planning_m, "GroundGreaterThan")
      .def(py::init<std::shared_ptr<planning::GroundNumericExpression>,
                    std::shared_ptr<planning::GroundNumericExpression>>(),
           "lhs"_a,
           "rhs"_a);
  py::class_<planning::GroundGreaterThanEqualTo,
             planning::GroundBinaryBooleanCondition,
             std::shared_ptr<planning::GroundGreaterThanEqualTo>>(planning_m, "GroundGreaterThanEqualTo")
      .def(py::init<std::shared_ptr<planning::GroundNumericExpression>,
                    std::shared_ptr<planning::GroundNumericExpression>>(),
           "lhs"_a,
           "rhs"_a);
  py::class_<planning::GroundEqualTo, planning::GroundBinaryBooleanCondition, std::shared_ptr<planning::GroundEqualTo>>(
      planning_m, "GroundEqualTo")
      .def(py::init<std::shared_ptr<planning::GroundNumericExpression>,
                    std::shared_ptr<planning::GroundNumericExpression>>(),
           "lhs"_a,
           "rhs"_a);

  // GroundNumericExpression
  py::class_<planning::GroundNumericExpression, std::shared_ptr<planning::GroundNumericExpression>>(
      planning_m, "GroundNumericExpression")
      .def("eval_real", &planning::GroundNumericExpression::eval_real, "values"_a);
  py::class_<planning::GroundValue, planning::GroundNumericExpression, std::shared_ptr<planning::GroundValue>>(
      planning_m, "GroundValue")
      .def(py::init<double>(), "value"_a);
  py::class_<planning::GroundFunction, planning::GroundNumericExpression, std::shared_ptr<planning::GroundFunction>>(
      planning_m, "GroundFunction")
      .def(py::init<int>(), "index"_a);
  py::class_<planning::GroundBinaryNumericExpression,
             planning::GroundNumericExpression,
             std::shared_ptr<planning::GroundBinaryNumericExpression>>(planning_m, "GroundBinaryNumericExpression");
  py::class_<planning::GroundPlus, planning::GroundBinaryNumericExpression, std::shared_ptr<planning::GroundPlus>>(
      planning_m, "GroundPlus")
      .def(py::init<std::shared_ptr<planning::GroundNumericExpression>,
                    std::shared_ptr<planning::GroundNumericExpression>>(),
           "lhs"_a,
           "rhs"_a);
  py::class_<planning::GroundMinus, planning::GroundBinaryNumericExpression, std::shared_ptr<planning::GroundMinus>>(
      planning_m, "GroundMinus")
      .def(py::init<std::shared_ptr<planning::GroundNumericExpression>,
                    std::shared_ptr<planning::GroundNumericExpression>>(),
           "lhs"_a,
           "rhs"_a);
  py::class_<planning::GroundTimes, planning::GroundBinaryNumericExpression, std::shared_ptr<planning::GroundTimes>>(
      planning_m, "GroundTimes")
      .def(py::init<std::shared_ptr<planning::GroundNumericExpression>,
                    std::shared_ptr<planning::GroundNumericExpression>>(),
           "lhs"_a,
           "rhs"_a);
  py::class_<planning::GroundDivide, planning::GroundBinaryNumericExpression, std::shared_ptr<planning::GroundDivide>>(
      planning_m, "GroundDivide")
      .def(py::init<std::shared_ptr<planning::GroundNumericExpression>,
                    std::shared_ptr<planning::GroundNumericExpression>>(),
           "lhs"_a,
           "rhs"_a);

#ifdef succgen_VERSION
  m.attr("__version__") = MACRO_STRINGIFY(succgen_VERSION);
#else
  m.attr("__version__") = "dev";
#endif
}
