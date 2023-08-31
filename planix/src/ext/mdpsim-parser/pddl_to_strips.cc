#include <iostream>
#include <stdexcept>
#include <map>
#include <vector>
#include <string>

#include "pddl_to_strips.h"

#include "problems.h"
#include "representations/strips_problems.h"


using STRIPS::Mask;
using STRIPS::StripsMultiCostAction;


namespace PPDDL {

StripsMultiObjectiveProblem parseToStrips(std::string const& domain_and_problem_fname) {
  MdpsimProblem const* problem = parsePPDDL(domain_and_problem_fname);
  assert(problem);
  return translateToMoStrips(*problem);
}


StripsMultiObjectiveProblem parseToStrips(std::string const& domain_fname,
                                         std::string const& problem_fname)
{
  MdpsimProblem const* problem = parsePPDDL(domain_fname, problem_fname);
  assert(problem);
  return translateToMoStrips(*problem);
}


Mask translateConjOfPosAtoms(StateFormula const& fml,
                             std::map<Atom const*, size_t> const& atom_to_idx)
{
  if (Atom const* atm = dynamic_cast<Atom const*>(&fml)) {
    auto atom_ite = atom_to_idx.find(atm);
    assert(atom_ite != atom_to_idx.end());
    return Mask(atom_to_idx.size()).set(atom_ite->second);
  }
  else if (Conjunction const* conj = dynamic_cast<Conjunction const*>(&fml)) {
    Mask conj_mask(atom_to_idx.size());
    for (StateFormula const* const& conjunct : conj->conjuncts()) {
      assert(conjunct != nullptr);
      conj_mask |= translateConjOfPosAtoms(*conjunct, atom_to_idx);
    }
    return conj_mask;
  }
  else {
    std::cerr << "Formula " << fml << " is not supported in the goal\n";
    throw std::logic_error("Unsupported formula in the goal");
  }
  return Mask(0);
}


void translateStripsEffect(Effect const& ppddl_effect, STRIPS::Effect& strips_effect,
                           std::map<Atom const*, size_t> const& atom_to_idx)
{
  using ConjuncEff = ConjunctiveEffect;
  using CondEff = ConditionalEffect;
  using ProbEff = ProbabilisticEffect;
  using QuantEff = QuantifiedEffect;

  if (AddEffect const* add_eff = dynamic_cast<AddEffect const*>(&ppddl_effect)) {
    Atom const& atom = add_eff->atom();
    auto const ite = atom_to_idx.find(&atom);
    assert(ite != atom_to_idx.end());
    strips_effect.add.set(ite->second);
  }
  else if (DeleteEffect const* del_eff = dynamic_cast<DeleteEffect const*>(&ppddl_effect)) {
    Atom const& atom = del_eff->atom();
    auto const ite = atom_to_idx.find(&atom);
    assert(ite != atom_to_idx.end());
    strips_effect.del.set(ite->second);
  }
  else if (ConjuncEff const* conj_eff = dynamic_cast<ConjuncEff const*>(&ppddl_effect)) {
    for (Effect const* c : conj_eff->conjuncts()) {
      assert(c != nullptr);
      translateStripsEffect(*c, strips_effect, atom_to_idx);
    }
  }
  else if (CondEff const* cond_eff = dynamic_cast<CondEff const*>(&ppddl_effect)) {
    std::cerr << "ConditionalEffect '" << *cond_eff << "' not supported\n";
    throw std::logic_error("Unsupported effect");
  }
  else if (dynamic_cast<UpdateEffect const*>(&ppddl_effect)) {
    // The supported update effects are handled by Action::cost(). Perhaps it would be better to
    // handle it here to support conditional cost, i.e., state-dependent action cost
  }
  else if (ProbEff const* prob_eff = dynamic_cast<ProbEff const*>(&ppddl_effect)) {
    std::cerr << "ProbabilisticEffect '" << *prob_eff << "' not supported\n";
    throw std::logic_error("Unsupported effect");
  }
  else if (QuantEff const* quant_eff = dynamic_cast<QuantEff const*>(&ppddl_effect)) {
    std::cerr << "QuantifiedEffect '" << *quant_eff << "' not supported\n";
    throw std::logic_error("Unsupported effect");
  }
  else {
    std::cerr << "Effect '" << ppddl_effect << "' not supported\n";
    throw std::logic_error("Unsupported effect");
  }
}

StripsMultiCostAction translateAction(Action const& ppddl_action,
                                      std::map<Atom const*, size_t> const& atom_to_idx,
                                      std::map<std::string, size_t> const& metric_to_idx)
{
  Mask strips_prec = translateConjOfPosAtoms(ppddl_action.precondition(), atom_to_idx);

  STRIPS::Effect strips_effect(atom_to_idx.size());
  translateStripsEffect(ppddl_action.effect(), strips_effect, atom_to_idx);

  std::vector<double> cost(metric_to_idx.size(), 0);
  for (auto const& name_val : ppddl_action.cost()) {
    auto const idx_it = metric_to_idx.find(name_val.first);
    assert(idx_it != metric_to_idx.end());
    assert(idx_it->second < cost.size());
    cost[idx_it->second] = name_val.second;
  }

  return {ppddl_action.name(), cost, strips_prec, strips_effect};
}


StripsMultiObjectiveProblem translateToMoStrips(MdpsimProblem const& problem) {
  using STRIPS::StripsState;

  // same as size() == 1 but with the methods provided. This is needed to make sure the AtomTable
  // only has atoms from a single problem.
  assert(++MdpsimProblem::begin() == MdpsimProblem::end());

  auto const& atom_table = Atom::getAtomTable();
  size_t n_atoms = atom_table.size();

  // The ordering is irrelevant but need to be fixed for the STRIPS representation this is the
  // ordering that will be used
  std::vector<Atom const*> ordered_atoms(n_atoms, nullptr);
  std::map<Atom const*, size_t> atom_to_idx;

  for (size_t idx = 0; Atom const* const& atm : atom_table) {
    assert(atm != nullptr);
    ordered_atoms[idx] = atm;
    atom_to_idx[atm] = idx;
    ++idx;
  }

  StripsState s0(n_atoms);
  for (auto init = problem.initialState(); Atom const* const& atm : init.atoms) {
    assert(atom_to_idx.find(atm) != atom_to_idx.end());
    s0.set(atom_to_idx[atm]);
  }

  StateFormula const& ppddl_goal = problem.goal();
  Mask goal_set = translateConjOfPosAtoms(ppddl_goal, atom_to_idx);

  // Similarly to atoms, the ordering of the metrics do not matter but it needs to be fixed
  std::map<std::string, size_t> metric_to_idx;
  VecExpression const& metrics = problem.metrics();
  for (size_t idx = 0; Expression const* const& m : metrics) {
    Fluent const* fluent = dynamic_cast<Fluent const*>(m);
    if (!fluent) {
      std::cout << "Metric '" << m << "' is not a fluent and currently not supported\n";
      throw std::logic_error("Unsupported metric");
    }
    metric_to_idx[fluent->name()] = idx;
    ++idx;
  }

  std::vector<StripsMultiCostAction> actions;

  for (Action const* ppddl_action : problem.actions()) {
    // FWT: Somehow there are nullptr in problem.actions(). This is an issue from MDPSIM
    if (ppddl_action == nullptr) {
      continue;
    }
    actions.push_back(translateAction(*ppddl_action, atom_to_idx, metric_to_idx));
  }

  return {s0, goal_set, actions};
}


StripsProblem translateToStrips(MdpsimProblem const& problem) {
  using STRIPS::StripsState;
  using STRIPS::StripsAction;

  StripsMultiObjectiveProblem moProblem = translateToMoStrips(problem);

  // Should be either 0 or 1. If is 0, then all actions have unitary cost.
  int n_metrics = moProblem.numCostFunctions();

  if (n_metrics > 1) {
    std::cout << "Parsed problem is not a single-objective problem and has "
              << n_metrics << " objectives. Metrics are\n";
    VecExpression const& metrics = problem.metrics();
    for (Expression const* const& m : metrics) {
      std::cout << m << std::endl;
    }
    throw std::logic_error("Expected single-objective problem but parsed multi-objective problem");
  }

  StripsState s0 = moProblem.initialState();
  Mask goal_set = moProblem.goalSet();
  std::vector<StripsMultiCostAction> moActions = moProblem.allActions();
  std::vector<StripsAction> soActions;
  for (const auto moAction : moActions) {
    double cost;
    if (n_metrics == 0) {
      cost = 1;
    } else {
      cost = moAction.cost()[0];
    }
    soActions.push_back({
      moAction.name(),
      cost,
      moAction.precondition(),
      moAction.effect()
    });
  }

  return {s0, goal_set, soActions};
}

}  // namespace PPDDL
