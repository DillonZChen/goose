#include "sas_compilation.h"

#include "../globals.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"
#include "../utils/markup.h"

using namespace std;
using namespace gurobi_ip_compilation;
using namespace numeric_helper;

GurobiSASStateChangeModel::GurobiSASStateChangeModel(
    const options::Options &opts)
    : use_landmark(opts.get<bool>("landmark")),
      current_horizon(0),
      factory(nullptr) {}

void GurobiSASStateChangeModel::initialize(
    const int horizon, const std::shared_ptr<AbstractTask> task,
    std::vector<std::vector<bool>> &action_mutex, bool use_linear_effects) {
  cout << "initializing SAS SC" << endl;
  create_sets(task, use_linear_effects);
  if (use_landmark) initialize_landmark(task);
  initialize_mutex(task, action_mutex);
}

void GurobiSASStateChangeModel::update(const int horizon,
                                       const std::shared_ptr<AbstractTask> task,
                                       std::shared_ptr<GRBModel> model,
                                       std::vector<std::vector<GRBVar>> &x) {
  cout << "adding constraint from SAS SC" << endl;
  bool first = current_horizon == 0;
  int t_min = current_horizon;
  int t_max = horizon;
  add_variables(task, model, t_min, t_max);
  goal_state_constraint(task, model, t_max, first);
  update_state_change_constraint(task, model, t_min, t_max);
  precondition_constraint(task, model, x, t_min, t_max);
  effect_constraint(task, model, x, t_min, t_max);
  mutex_proposition_constraint(task, model, t_min, t_max);
  if (use_landmark) landmark_constraint(task, model, x, t_min, t_max, first);
  if (first) initial_state_constraint(task, model);
  current_horizon = horizon;
}

void GurobiSASStateChangeModel::create_sets(
    const std::shared_ptr<AbstractTask> task, bool use_linear_effects) {
  TaskProxy task_proxy(*task);
  numeric_task = NumericTaskProxy(task_proxy, false, true);
  OperatorsProxy ops = task_proxy.get_operators();
  VariablesProxy vars = task_proxy.get_variables();

  sc_actions.assign(vars.size(), vector<vector<set<int>>>());
  add_actions.assign(vars.size(), vector<set<int>>());
  pre_actions.assign(vars.size(), vector<set<int>>());

  int i_var = 0;
  for (VariableProxy var : vars) {
    int n_vals = var.get_domain_size();
    sc_actions[i_var].assign(n_vals, vector<set<int>>(n_vals, set<int>()));
    add_actions[i_var].assign(n_vals, set<int>());
    pre_actions[i_var].assign(n_vals, set<int>());
    i_var++;
  }

  for (size_t op_id = 0; op_id < ops.size(); ++op_id) {
    const OperatorProxy &op = ops[op_id];
    vector<int> precondition(vars.size(), -1);
    vector<int> effects(vars.size(), -1);
    for (FactProxy condition : op.get_preconditions()) {
      int pre_var_id = condition.get_variable().get_id();
      if (numeric_task.is_numeric_axiom(pre_var_id)) continue;
      precondition[pre_var_id] = condition.get_value();
    }
    for (EffectProxy effect_proxy : op.get_effects()) {
      FactProxy effect = effect_proxy.get_fact();
      int var = effect.get_variable().get_id();
      if (numeric_task.is_numeric_axiom(var)) continue;
      int pre = precondition[var];
      int post = effect.get_value();
      assert(post != -1);
      effects[var] = post;
      if (pre != -1) {
        sc_actions[var][pre][post].insert(op_id);
      } else {
        add_actions[var][post].insert(op_id);
      }
    }
    for (FactProxy condition : op.get_preconditions()) {
      int var = condition.get_variable().get_id();
      if (numeric_task.is_numeric_axiom(var)) continue;
      int pre = condition.get_value();
      int post = effects[var];
      if (post == -1 || post == pre) {
        pre_actions[var][pre].insert(op_id);
      }
    }
  }
}

void GurobiSASStateChangeModel::initialize_landmark(
    const std::shared_ptr<AbstractTask> task) {
  factory = std::unique_ptr<landmarks::LandmarkFactoryScala>(
      new landmarks::LandmarkFactoryScala(task));
  TaskProxy task_proxy(*task);
  State initial_state = task_proxy.get_initial_state();
  fact_landmarks = factory->compute_landmarks(initial_state);
  action_landmarks = factory->compute_action_landmarks(fact_landmarks);
}

void GurobiSASStateChangeModel::add_variables(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    int t_min, int t_max) {
  TaskProxy task_proxy(*task);
  VariablesProxy vars = task_proxy.get_variables();
  y.resize(t_max, std::vector<std::vector<std::vector<GRBVar>>>(vars.size()));

  for (int t = t_min; t < t_max; ++t) {
    for (VariableProxy var : vars) {
      int n_vals = var.get_domain_size();
      y[t][var.get_id()].resize(n_vals, std::vector<GRBVar>(n_vals));
      std::string base_name =
          "y^" + std::to_string(var.get_id()) + "_" + std::to_string(t);

      for (int val1 = 0; val1 < n_vals; ++val1) {
        for (int val2 = 0; val2 < n_vals; ++val2) {
          std::string name = base_name + "_" + std::to_string(val1) + "_" +
                             std::to_string(val2);
          y[t][var.get_id()][val1][val2] =
              model->addVar(0, 1, 0, GRB_BINARY, name);
        }
      }
    }
  }
}

void GurobiSASStateChangeModel::initial_state_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model) {
  TaskProxy task_proxy(*task);
  VariablesProxy vars = task_proxy.get_variables();

  State initial_state = task_proxy.get_initial_state();
  for (VariableProxy var : vars) {
    int i_var = var.get_id();
    int n_vals = var.get_domain_size();
    std::vector<double> coeffs(n_vals, 1);
    int initial_value = initial_state[i_var].get_value();
    GRBLinExpr lhs;
    lhs.addTerms(coeffs.data(), y[0][i_var][initial_value].data(), n_vals);
    model->addConstr(lhs == 1);

    {
      for (int in_val = 0; in_val < n_vals; in_val++) {
        if (in_val == initial_value) continue;
        GRBLinExpr lhs;
        lhs.addTerms(coeffs.data(), y[0][i_var][in_val].data(), n_vals);
        model->addConstr(lhs == 0);
      }
    }
  }
}

void GurobiSASStateChangeModel::goal_state_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    int t_max, bool first) {
  TaskProxy task_proxy(*task);
  VariablesProxy vars = task_proxy.get_variables();
  for (size_t id_goal = 0; id_goal < task_proxy.get_goals().size(); ++id_goal) {
    FactProxy goal = task_proxy.get_goals()[id_goal];
    if (numeric_task.is_numeric_axiom(goal.get_variable().get_id()) ||
        !numeric_task.numeric_goals_empty(id_goal))
      continue;  // this is a numeric goal
    std::string name = "SAS_goal_" + std::to_string(id_goal);
    if (!first) {
      GRBConstr constraint = model->getConstrByName(name);
      model->remove(constraint);
    }
    int var = goal.get_variable().get_id();
    int g_val = goal.get_value();
    int num_values = numeric_task.get_n_proposition_value(var);
    GRBLinExpr lhs;
    double coeff = 1;
    for (int val = 0; val < num_values; val++)
      lhs.addTerms(&coeff, &y[t_max - 1][var][val][g_val], 1);
    model->addConstr(lhs == 1, name);
  }
  for (size_t id_goal = 0; id_goal < numeric_task.get_n_numeric_goals(); ++id_goal) {
    for (pair<int, int> var_value: numeric_task.get_propositional_goals(id_goal)) {
      std::string name = "SAS_goal_" + std::to_string(id_goal) + "_" + std::to_string(var_value.first);
      if (!first) {
        GRBConstr constraint = model->getConstrByName(name);
        model->remove(constraint);
      }
      int num_values = numeric_task.get_n_proposition_value(var_value.first);
      GRBLinExpr lhs;
      double coeff = 1;
      for (int val = 0; val < num_values; val++)
        lhs.addTerms(&coeff, &y[t_max - 1][var_value.first][val][var_value.second], 1);
      model->addConstr(lhs == 1, name);
    }
  }
}

void GurobiSASStateChangeModel::update_state_change_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    int t_min, int t_max) {
  TaskProxy task_proxy(*task);
  for (int t = std::max(t_min - 1, 0); t < t_max - 1; ++t) {
    VariablesProxy vars = task_proxy.get_variables();
    for (VariableProxy var : vars) {
      int n_vals = var.get_domain_size();
      int i_var = var.get_id();
      std::vector<double> coeff(n_vals, 1);
      for (int val1 = 0; val1 < n_vals; val1++) {
        GRBLinExpr lhs;
        lhs.addTerms(coeff.data(), y[t + 1][i_var][val1].data(), n_vals);
        GRBLinExpr rhs;
        for (int val2 = 0; val2 < n_vals; val2++) {
          rhs.addTerms(coeff.data(), &y[t][i_var][val2][val1], 1);
        }
        model->addConstr(lhs == rhs);
      }
    }
  }
}

void GurobiSASStateChangeModel::effect_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    std::vector<std::vector<GRBVar>> &x, int t_min, int t_max) {
  TaskProxy task_proxy(*task);
  VariablesProxy vars = task_proxy.get_variables();
  for (VariableProxy var : vars) {
    int n_vals = var.get_domain_size();
    int i_var = var.get_id();
    for (int val1 = 0; val1 < n_vals; val1++) {
      // sc
      for (int val2 = 0; val2 < n_vals; val2++) {
        if (val1 == val2) continue;
        for (int t = t_min; t < t_max; ++t) {
          GRBLinExpr lhs;
          double coeff = 1;
          for (auto opt : sc_actions[i_var][val1][val2])
            lhs.addTerms(&coeff, &x[t][opt], 1);
          model->addConstr(lhs <= y[t][i_var][val1][val2]);
        }
      }

      // add
      for (auto opt : add_actions[i_var][val1]) {
        for (int t = t_min; t < t_max; ++t) {
          GRBLinExpr rhs;
          double coeff = 1;
          for (int val2 = 0; val2 < n_vals; val2++)
            rhs.addTerms(&coeff, &y[t][i_var][val2][val1], 1);
          model->addConstr(x[t][opt] <= rhs);
        }
      }

      // y
      for (int val2 = 0; val2 < n_vals; val2++) {
        if (val1 == val2) continue;
        for (int t = t_min; t < t_max; ++t) {
          GRBLinExpr rhs;
          double coeff = 1;
          for (auto opt : sc_actions[i_var][val1][val2])
            rhs.addTerms(&coeff, &x[t][opt], 1);
          for (auto opt : add_actions[i_var][val2])
            rhs.addTerms(&coeff, &x[t][opt], 1);
          model->addConstr(y[t][i_var][val1][val2] <= rhs);
        }
      }
    }
  }
}

void GurobiSASStateChangeModel::initialize_mutex(
    const std::shared_ptr<AbstractTask> task,
    std::vector<std::vector<bool>> &action_mutex) {
  for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id) {
    for (int op_mutex_id : numeric_task.get_mutex_actions(op_id)) {
      action_mutex[op_id][op_mutex_id] = true;
      action_mutex[op_mutex_id][op_id] = true;
    }
  }
}

void GurobiSASStateChangeModel::precondition_constraint(
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
          model->addConstr(x[t][opt] <= y[t][i_var][val1][val1]);
        }
      }
    }
  }
}

void GurobiSASStateChangeModel::mutex_proposition_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    int t_min, int t_max) {
  TaskProxy task_proxy(*task);
  VariablesProxy vars = task_proxy.get_variables();
  std::vector<int> var_id_to_domain_size(vars.size(), 0);
  for (VariableProxy var : vars) {
    var_id_to_domain_size[var.get_id()] = var.get_domain_size();
  }

  for (auto group : get_mutex_group()) {
    for (int t = t_min; t < t_max; ++t) {
      GRBLinExpr lhs;
      double coeff = 1;
      for (auto fact : group) {
        int var = fact.var;
        int val1 = fact.value;
        int n_val = var_id_to_domain_size[var];
        for (int val2 = 0; val2 < n_val; ++val2)
          lhs.addTerms(&coeff, &y[t][var][val2][val1], 1);
      }
      model->addConstr(lhs <= 1);
    }
  }
}

void GurobiSASStateChangeModel::landmark_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    std::vector<std::vector<GRBVar>> &x, int t_min, int t_max, bool first) {
  TaskProxy task_proxy(*task);
  VariablesProxy vars = task_proxy.get_variables();
  for (int op_id : action_landmarks) {
    std::string name = "action_landmark_" + std::to_string(op_id);
    if (!first) {
      GRBConstr constraint = model->getConstrByName(name);
      model->remove(constraint);
    }
    GRBLinExpr lhs;
    double coefficient = 1;
    for (int t = 0; t < t_max; ++t) lhs.addTerms(&coefficient, &x[t][op_id], 1);
    model->addConstr(lhs >= 1, name);
  }

  for (int fact : fact_landmarks) {
    if (fact < numeric_task.get_n_propositions()) {
      std::string name = "fact_landmark_" + std::to_string(fact);
      if (!first) {
        GRBConstr constraint = model->getConstrByName(name);
        model->remove(constraint);
      }
      std::pair<int, int> var_val = numeric_task.get_var_val(fact);
      int var = var_val.first;
      int val1 = var_val.second;
      GRBLinExpr lhs;
      double coefficient = 1;
      for (int t = 0; t < t_max; ++t) {
        for (int val2 = 0; val2 < vars[var].get_domain_size(); ++val2) {
          lhs.addTerms(&coefficient, &y[t][var][val2][val1], 1);
          if (val1 != val2)
            lhs.addTerms(&coefficient, &y[t][var][val1][val2], 1);
        }
      }
      model->addConstr(lhs >= 1, name);
    }
  }
}

static shared_ptr<GurobiIPConstraintGenerator> _parse(OptionParser &parser) {
  parser.document_synopsis(
      "SAS state change model",
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
  return make_shared<GurobiSASStateChangeModel>(opts);
}

static PluginShared<GurobiIPConstraintGenerator> _plugin("sas", _parse);
