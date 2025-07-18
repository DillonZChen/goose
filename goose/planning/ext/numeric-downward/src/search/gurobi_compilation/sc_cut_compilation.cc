#include "sc_cut_compilation.h"

#include "../globals.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"
#include "../utils/markup.h"

using namespace std;
using namespace gurobi_ip_compilation;
using namespace numeric_helper;

GurobiStateChangeModelWithCuts::GurobiStateChangeModelWithCuts(
    const options::Options &opts)
    : GurobiStateChangeModel(opts) {}

void GurobiStateChangeModelWithCuts::precondition_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    std::vector<std::vector<GRBVar>> &x, int t_min, int t_max) {
  TaskProxy task_proxy(*task);
  VariablesProxy vars = task_proxy.get_variables();
  OperatorsProxy ops = task_proxy.get_operators();
  for (VariableProxy var : vars) {
    int n_vals = var.get_domain_size();
    int i_var = var.get_id();
    for (int val = 0; val < n_vals; val++) {
      int p = numeric_task.get_proposition(i_var, val);
      for (int t = t_min; t < t_max; ++t) {
        double coeff = 1;
        GRBLinExpr lhs;
        for (int op_id : pnd[p]) {
          model->addConstr(x[t][op_id] <=
                           y_pa[t + 1][p] + y_a[t + 1][p] + y_pd[t + 1][p]);
          lhs.addTerms(&coeff, &x[t][op_id], 1);
        }
      }
    }
  }
}

void GurobiStateChangeModelWithCuts::initialize_mutex(
    const std::shared_ptr<AbstractTask> task,
    std::vector<std::vector<bool>> &action_mutex) {
  TaskProxy task_proxy(*task);
  OperatorsProxy ops = task_proxy.get_operators();
  for (size_t op_id1 = 0; op_id1 < ops.size(); ++op_id1) {
    for (int p : numeric_task.get_action_del_list(op_id1)) {
      for (int op_id2 : anp[p]) action_mutex[op_id1][op_id2] = true;
    }
  }

  action_precedence_inner.resize(ops.size(), std::vector<bool>(ops.size(), false));

  VariablesProxy vars = task_proxy.get_variables();
  for (VariableProxy var : vars) {
    int n_vals = var.get_domain_size();
    for (int val = 0; val < n_vals; ++val) {
      int p = numeric_task.get_proposition(var.get_id(), val);
      for (int op_id1 : pnd[p]) {
        for (int op_id2 : anp[p])
          if (!action_mutex[op_id1][op_id2]) action_precedence_inner[op_id2][op_id1] = true;
        for (int op_id2 : pd[p])
          if (!action_mutex[op_id1][op_id2]) action_precedence_inner[op_id1][op_id2] = true;
      }
    }
  }
}

void GurobiStateChangeModelWithCuts::add_action_precedence(
    const std::shared_ptr<AbstractTask> task,
    std::vector<std::vector<bool>> &action_precedence,
    std::vector<std::vector<bool>> &action_mutex) {
  TaskProxy task_proxy(*task);
  VariablesProxy vars = task_proxy.get_variables();

  for (size_t op_id1 = 0; op_id1 < numeric_task.get_n_actions(); ++op_id1) {
    for (size_t op_id2 = 0; op_id2 < numeric_task.get_n_actions(); ++op_id2) {
      if (op_id1 == op_id2) continue;
      if (action_mutex[op_id1][op_id2]) {
        action_precedence_inner[op_id1][op_id2] = false;
        action_precedence_inner[op_id2][op_id1] = false;
        action_precedence[op_id1][op_id2] = false;
        action_precedence[op_id2][op_id1] = false;
      }
      if (action_precedence_inner[op_id1][op_id2]) {
        if (action_precedence[op_id2][op_id1] || action_precedence_inner[op_id2][op_id1]) {
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
      "State change model with cuts",
      "For details, see" +
          utils::format_paper_reference(
              {"Thomas Vossen", "Michael Ball", "Amnon Lotem", "Dana Nau"},
              "On the Use of Integer Programming Models in AI Planning",
              "https://www.ijcai.org/Proceedings/99-1/Papers/045.pdf",
              "Proceedings of the Sixteenth International Joint Conference on"
              " Artificial Intelligence (IJCAI 1999)",
              "304-309", "1999"));
  parser.add_option<bool>("landmark", "use landmark constraints", "false");
  options::Options opts = parser.parse();

  if (parser.dry_run()) return nullptr;
  return make_shared<GurobiStateChangeModelWithCuts>(opts);
}

static PluginShared<GurobiIPConstraintGenerator> _plugin("sc_cut", _parse);
