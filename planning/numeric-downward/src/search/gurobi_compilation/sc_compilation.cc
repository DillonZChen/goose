#include "sc_compilation.h"

#include "../globals.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"
#include "../utils/markup.h"

using namespace std;
using namespace gurobi_ip_compilation;
using namespace numeric_helper;

GurobiStateChangeModel::GurobiStateChangeModel(const options::Options &opts)
    : use_landmark(opts.get<bool>("landmark")),
      current_horizon(0),
      factory(nullptr) {}

void GurobiStateChangeModel::initialize(
    const int horizon, const std::shared_ptr<AbstractTask> task,
    std::vector<std::vector<bool>> &action_mutex, bool use_linear_effects) {
  cout << "initializing SC" << endl;
  create_sets(task, use_linear_effects);
  if (use_landmark) initialize_landmark(task);
  initialize_mutex(task, action_mutex);
}

void GurobiStateChangeModel::update(const int horizon,
                                    const std::shared_ptr<AbstractTask> task,
                                    std::shared_ptr<GRBModel> model,
                                    std::vector<std::vector<GRBVar>> &x) {
  cout << "adding constraint from SC" << endl;
  bool first = current_horizon == 0;
  int t_min = current_horizon;
  int t_max = horizon;
  add_variables(task, model, t_min, t_max, first);
  goal_state_constraint(task, model, t_max, first);
  update_state_change_constraint(task, model, t_min, t_max);
  precondition_constraint(task, model, x, t_min, t_max);
  effect_constraint(task, model, x, t_min, t_max);
  if (use_landmark) landmark_constraint(task, model, x, t_min, t_max, first);
  if (first) initial_state_constraint(task, model);
  current_horizon = horizon;
}

void GurobiStateChangeModel::create_sets(
    const std::shared_ptr<AbstractTask> task, bool use_linear_effects) {
  TaskProxy task_proxy(*task);
  numeric_task = NumericTaskProxy(task_proxy, false, true);
  OperatorsProxy ops = task_proxy.get_operators();
  int n_prop = numeric_task.get_n_propositions();
  VariablesProxy vars = task_proxy.get_variables();

  pnd.assign(n_prop, set<int>());
  anp.assign(n_prop, set<int>());
  pd.assign(n_prop, set<int>());

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
      // assert(pre != post);
      effects[var] = post;
      // if a is e
      if (pre != post) {
        // add but not pre
        anp[numeric_task.get_proposition(var, post)].insert(op_id);
      }
    }
    for (FactProxy condition : op.get_preconditions()) {
      int var = condition.get_variable().get_id();
      int pre = condition.get_value();
      int post = effects[var];
      if (numeric_task.is_numeric_axiom(var)) continue;
      if (post == -1 || post == pre) {
        pnd[numeric_task.get_proposition(var, pre)].insert(op_id);
      } else if (post != pre) {
        pd[numeric_task.get_proposition(var, pre)].insert(op_id);
      } else {
        assert(false);
      }
    }
  }
}

void GurobiStateChangeModel::initialize_landmark(
    const std::shared_ptr<AbstractTask> task) {
  factory = std::unique_ptr<landmarks::LandmarkFactoryScala>(
      new landmarks::LandmarkFactoryScala(task));
  TaskProxy task_proxy(*task);
  State initial_state = task_proxy.get_initial_state();
  fact_landmarks = factory->compute_landmarks(initial_state);
  action_landmarks = factory->compute_action_landmarks(fact_landmarks);
}

void GurobiStateChangeModel::add_variables(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    int t_min, int t_max, bool first) {
  TaskProxy task_proxy(*task);
  VariablesProxy vars = task_proxy.get_variables();
  int n_propositions = numeric_task.get_n_propositions();
  y_a.resize(t_max + 1, std::vector<GRBVar>(n_propositions));
  y_pa.resize(t_max + 1, std::vector<GRBVar>(n_propositions));
  y_pd.resize(t_max + 1, std::vector<GRBVar>(n_propositions));
  y_m.resize(t_max + 1, std::vector<GRBVar>(n_propositions));

  if (first) {
    for (VariableProxy var : vars) {
      int n_vals = var.get_domain_size();
      for (int val = 0; val < n_vals; ++val) {
        int p = numeric_task.get_proposition(var.get_id(), val);
        y_a[0][p] = model->addVar(0, 1, 0, GRB_BINARY);
        y_pa[0][p] = model->addVar(0, 1, 0, GRB_BINARY);
        y_pd[0][p] = model->addVar(0, 1, 0, GRB_BINARY);
        y_m[0][p] = model->addVar(0, 1, 0, GRB_BINARY);
        model->addConstr(y_a[0][p] + y_m[0][p] + y_pd[0][p] <= 1);
        model->addConstr(y_pa[0][p] + y_m[0][p] + y_pd[0][p] <= 1);
      }
    }
  }

  for (int t = t_min + 1; t < t_max + 1; ++t) {
    for (VariableProxy var : vars) {
      int n_vals = var.get_domain_size();
      for (int val = 0; val < n_vals; ++val) {
        int p = numeric_task.get_proposition(var.get_id(), val);
        y_a[t][p] = model->addVar(0, 1, 0, GRB_BINARY);
        y_pa[t][p] = model->addVar(0, 1, 0, GRB_BINARY);
        y_pd[t][p] = model->addVar(0, 1, 0, GRB_BINARY);
        y_m[t][p] = model->addVar(0, 1, 0, GRB_BINARY);
        model->addConstr(y_a[t][p] + y_m[t][p] + y_pd[t][p] <= 1);
        model->addConstr(y_pa[t][p] + y_m[t][p] + y_pd[t][p] <= 1);
      }
    }
  }
}

void GurobiStateChangeModel::initial_state_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model) {
  TaskProxy task_proxy(*task);
  VariablesProxy vars = task_proxy.get_variables();

  State initial_state = task_proxy.get_initial_state();
  for (VariableProxy var : vars) {
    int i_var = var.get_id();
    int initial_value = initial_state[i_var].get_value();
    int p = numeric_task.get_proposition(i_var, initial_value);
    model->addConstr(y_a[0][p] == 1);

    {
      int n_vals = var.get_domain_size();
      for (int in_val = 0; in_val < n_vals; in_val++) {
        int p = numeric_task.get_proposition(i_var, in_val);
        model->addConstr(y_pa[0][p] == 0);
        model->addConstr(y_pd[0][p] == 0);
        model->addConstr(y_m[0][p] == 0);
        if (in_val != initial_value) {
          model->addConstr(y_a[0][p] == 0);
        }
      }
    }
  }
}

void GurobiStateChangeModel::goal_state_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    int t_max, bool first) {
  TaskProxy task_proxy(*task);
  VariablesProxy vars = task_proxy.get_variables();
  for (size_t id_goal = 0; id_goal < task_proxy.get_goals().size(); ++id_goal) {
    FactProxy goal = task_proxy.get_goals()[id_goal];
    if (numeric_task.is_numeric_axiom(goal.get_variable().get_id()) ||
        !numeric_task.numeric_goals_empty(id_goal))
      continue;  // this is a numeric goal
    std::string name = "SC_goal_" + std::to_string(id_goal);
    if (!first) {
      GRBConstr constraint = model->getConstrByName(name);
      model->remove(constraint);
    }
    int var = goal.get_variable().get_id();
    int g_val = goal.get_value();
    int p = numeric_task.get_proposition(var, g_val);
    model->addConstr(y_a[t_max][p] + y_pa[t_max][p] + y_m[t_max][p] >= 1, name);
  }
  for (size_t id_goal = 0; id_goal < numeric_task.get_n_numeric_goals(); ++id_goal) {
    for (pair<int, int> var_value: numeric_task.get_propositional_goals(id_goal)) {
      int p = numeric_task.get_proposition(var_value.first, var_value.second);
      std::string name = "SC_goal_" + std::to_string(id_goal) + "_" + std::to_string(p);
      if (!first) {
        GRBConstr constraint = model->getConstrByName(name);
        model->remove(constraint);
      }
      model->addConstr(y_a[t_max][p] + y_pa[t_max][p] + y_m[t_max][p] >= 1, name);
    }
  }
}

void GurobiStateChangeModel::update_state_change_constraint(
    const std::shared_ptr<AbstractTask> task, std::shared_ptr<GRBModel> model,
    int t_min, int t_max) {
  TaskProxy task_proxy(*task);
  for (int t = t_min; t < t_max; ++t) {
    VariablesProxy vars = task_proxy.get_variables();
    for (VariableProxy var : vars) {
      int n_vals = var.get_domain_size();
      int i_var = var.get_id();
      std::vector<double> coeff(n_vals, 1);
      for (int val = 0; val < n_vals; val++) {
        int p = numeric_task.get_proposition(i_var, val);
        model->addConstr(y_pa[t + 1][p] + y_m[t + 1][p] + y_pd[t + 1][p] <=
                         y_a[t][p] + y_pa[t][p] + y_m[t][p]);
      }
    }
  }
}

void GurobiStateChangeModel::precondition_constraint(
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
          model->addConstr(x[t][op_id] <= y_pa[t + 1][p]);
          lhs.addTerms(&coeff, &x[t][op_id], 1);
        }
      }
    }
  }
}

void GurobiStateChangeModel::effect_constraint(
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
        // pnd
        {
          GRBLinExpr lhs;
          for (int op_id : pnd[p]) {
            lhs.addTerms(&coeff, &x[t][op_id], 1);
          }
          model->addConstr(lhs >= y_pa[t + 1][p]);
        }
        // anp
        {
          GRBLinExpr lhs;
          for (int op_id : anp[p]) {
            model->addConstr(x[t][op_id] <= y_a[t + 1][p]);
            lhs.addTerms(&coeff, &x[t][op_id], 1);
          }
          model->addConstr(lhs >= y_a[t + 1][p]);
        }
        // pd
        {
          GRBLinExpr lhs;
          for (int op_id : pd[p]) lhs.addTerms(&coeff, &x[t][op_id], 1);
          model->addConstr(lhs == y_pd[t + 1][p]);
        }
      }
    }
  }
}

void GurobiStateChangeModel::initialize_mutex(
    const std::shared_ptr<AbstractTask> task,
    std::vector<std::vector<bool>> &action_mutex) {
  for (size_t op_id1 = 0; op_id1 < numeric_task.get_n_actions(); ++op_id1) {
    for (int p : numeric_task.get_action_del_list(op_id1)) {
      for (int op_id2 : pnd[p]) action_mutex[op_id1][op_id2] = true;
      for (int op_id2 : pd[p]) action_mutex[op_id1][op_id2] = true;
      for (int op_id2 : anp[p]) action_mutex[op_id1][op_id2] = true;
    }
  }
}

void GurobiStateChangeModel::landmark_constraint(
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
      GRBLinExpr lhs;
      double coefficient = 1;
      for (int t = 0; t < t_max + 1; ++t) {
        lhs.addTerms(&coefficient, &y_a[t][fact], 1);
        lhs.addTerms(&coefficient, &y_pa[t][fact], 1);
        lhs.addTerms(&coefficient, &y_m[t][fact], 1);
      }
      model->addConstr(lhs >= 1, name);
    }
  }
}

static shared_ptr<GurobiIPConstraintGenerator> _parse(OptionParser &parser) {
  parser.document_synopsis(
      "State change model",
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
  return make_shared<GurobiStateChangeModel>(opts);
}

static PluginShared<GurobiIPConstraintGenerator> _plugin("sc", _parse);
