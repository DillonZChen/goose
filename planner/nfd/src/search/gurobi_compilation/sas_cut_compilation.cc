#include "sas_cut_compilation.h"

#include "../globals.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"
#include "../utils/markup.h"

using namespace std;
using namespace gurobi_ip_compilation;
using namespace numeric_helper;

GurobiSASStateChangeModelWithCuts::GurobiSASStateChangeModelWithCuts(
    const options::Options &opts)
    : GurobiSASStateChangeModel(opts) {}

void GurobiSASStateChangeModelWithCuts::initialize_mutex(
    const std::shared_ptr<AbstractTask> task,
    std::vector<std::vector<bool>> &action_mutex) {
  TaskProxy task_proxy(*task);
  OperatorsProxy ops = task_proxy.get_operators();
  VariablesProxy vars = task_proxy.get_variables();

  for (size_t op_id1 = 0; op_id1 < ops.size(); ++op_id1) {
    vector<int> postcondition(vars.size(), -1);
    for (EffectProxy effect_proxy : ops[op_id1].get_effects()) {
      FactProxy effect = effect_proxy.get_fact();
      int var = effect.get_variable().get_id();
      postcondition[var] = effect.get_value();
    }
    for (size_t op_id2 = 0; op_id2 < ops.size(); ++op_id2) {
      if (op_id1 == op_id2 || action_mutex[op_id1][op_id2]) continue;
      for (EffectProxy effect_proxy : ops[op_id2].get_effects()) {
        FactProxy effect = effect_proxy.get_fact();
        int var = effect.get_variable().get_id();
        int post = effect.get_value();
        if (postcondition[var] != post) {
          action_mutex[op_id1][op_id2] = true;
          action_mutex[op_id2][op_id1] = true;
          break;
        }
      }
    }
  }

  vector<int> precondition(vars.size(), -1);
  vector<int> postcondition(vars.size(), -1);
  vector<int> precondition2(vars.size(), -1);
  action_precedence_inner.resize(ops.size(), std::vector<bool>(ops.size(), false));

  for (size_t op_id1 = 0; op_id1 < ops.size(); ++op_id1) {
    for (FactProxy condition : ops[op_id1].get_preconditions()) {
      int pre_var_id = condition.get_variable().get_id();
      precondition[pre_var_id] = condition.get_value();
    }
    for (EffectProxy effect_proxy : ops[op_id1].get_effects()) {
      FactProxy effect = effect_proxy.get_fact();
      int var = effect.get_variable().get_id();
      postcondition[var] = effect.get_value();
    }
    for (size_t op_id2 = 0; op_id2 < ops.size(); ++op_id2) {
      if (op_id1 == op_id2 || action_mutex[op_id1][op_id2]) continue;
      for (FactProxy condition : ops[op_id2].get_preconditions()) {
        int pre_var_id = condition.get_variable().get_id();
        precondition2[pre_var_id] = condition.get_value();
      }
      for (EffectProxy effect_proxy : ops[op_id2].get_effects()) {
        FactProxy effect = effect_proxy.get_fact();
        int var = effect.get_variable().get_id();
        int post = effect.get_value();
        if (precondition[var] != -1 && postcondition[var] == -1 &&
            precondition[var] != post) {
          action_precedence_inner[op_id1][op_id2] = true;
        }
        if (precondition[var] != -1 && postcondition[var] == -1 &&
            precondition[var] == post) {
          action_precedence_inner[op_id2][op_id1] = true;
        }
        if (precondition[var] != -1 && postcondition[var] != -1 &&
            precondition[var] != postcondition[var] &&
            precondition2[var] == -1 && post != -1 &&
            post == postcondition[var]) {
          action_precedence_inner[op_id1][op_id2] = true;
        }
      }
    }
  }
}

void GurobiSASStateChangeModelWithCuts::precondition_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    std::vector<std::vector<GRBVar>> &x, int t_min, int t_max) {
  TaskProxy task_proxy(*task);
  VariablesProxy vars = task_proxy.get_variables();
  for (VariableProxy var : vars) {
    int n_vals = var.get_domain_size();
    int i_var = var.get_id();
    for (int val1 = 0; val1 < n_vals; val1++) {
      // prevail
      for (auto opt : pre_actions[i_var][val1]) {
        for (int t = t_min; t < t_max; ++t) {
          GRBLinExpr rhs(y[t][i_var][val1][val1]);
          double coeffcient = 1;
          for (int val2 = 0; val2 < n_vals; val2++) {
            if (val1 == val2) continue;
            rhs.addTerms(&coeffcient, &y[t][i_var][val1][val2], 1);
            rhs.addTerms(&coeffcient, &y[t][i_var][val2][val1], 1);
          }
          model->addConstr(x[t][opt] <= rhs);
        }
      }
    }
  }
}

void GurobiSASStateChangeModelWithCuts::add_action_precedence(
    const std::shared_ptr<AbstractTask> task,
    std::vector<std::vector<bool>> &action_precedence,
    std::vector<std::vector<bool>> &action_mutex) {
  TaskProxy task_proxy(*task);
  OperatorsProxy ops = task_proxy.get_operators();

  for (size_t op_id1 = 0; op_id1 < ops.size(); ++op_id1) {
    for (size_t op_id2 = 0; op_id2 < ops.size(); ++op_id2) {
      if (op_id1 == op_id2) continue;
      if (action_mutex[op_id1][op_id2]) {
        action_precedence_inner[op_id1][op_id2] = false;
        action_precedence_inner[op_id2][op_id1] = false;
        action_precedence[op_id1][op_id2] = false;
        action_precedence[op_id2][op_id1] = false;
      }
      if (action_precedence_inner[op_id1][op_id2]) {
        if (action_precedence[op_id2][op_id1] || action_precedence_inner[op_id2][op_id2]) {
          action_precedence_inner[op_id1][op_id2] = false;
          action_precedence_inner[op_id2][op_id1] = false;
          action_precedence[op_id1][op_id2] = false;
          action_precedence[op_id2][op_id1] = false;
          action_mutex[op_id1][op_id2] = true;
          action_mutex[op_id2][op_id1] = true;
        } else {
          action_precedence[op_id1][op_id2] = true;
        }
      }
    }
  }
}

static shared_ptr<GurobiIPConstraintGenerator> _parse(OptionParser &parser) {
  parser.document_synopsis(
      "SAS state change model with cuts",
      "For details, see" +
          utils::format_paper_reference(
              {"Menkes van den Briel", "Thomas Vossen", "Subbarao Kambhampati"},
              "Reviving integer programming approaches for AI planning: A "
              "branch-and-cut framework",
              "https://www.aaai.org/Papers/ICAPS/2005/ICAPS05-032.pdf",
              "Proceedings of the Fifteen International Conference on"
              " Automated Planning and Scheduling (ICAPS 2005)",
              "310-319", "2005"));
  parser.add_option<bool>("landmark", "use landmark constraints", "false");
  options::Options opts = parser.parse();

  if (parser.dry_run()) return nullptr;
  return make_shared<GurobiSASStateChangeModelWithCuts>(opts);
}

static PluginShared<GurobiIPConstraintGenerator> _plugin("sas_cut", _parse);
