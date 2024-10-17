#include "ip_compilation.h"

#include <cmath>

#include "../option_parser.h"
#include "../plugin.h"
#include "../task_proxy.h"

using namespace std;
using namespace gurobi_ip_compilation;

GurobiIPCompilation::GurobiIPCompilation(
    const options::Options &opts, const std::shared_ptr<AbstractTask> &task)
    : add_lazy_constraints(opts.get<bool>("lazy_constraints")),
      add_user_cuts(opts.get<bool>("user_cuts")),
      max_num_cuts(opts.get<int>("max_num_cuts")),
      add_one_time_step(opts.get<bool>("add_one_time_step")),
      use_linear_effects(opts.get<bool>("linear_effects")),
      lazy_mutex(opts.get<bool>("lazy_mutex")),
      node_count(0),
      min_action_cost(std::numeric_limits<ap_float>::max()),
      constraint_generators(
          opts.get_list<std::shared_ptr<GurobiIPConstraintGenerator>>(
              "gurobi_ipmodel")),
      task(task),
      env(new GRBEnv()),
      graph(nullptr),
      callback(nullptr) {
  env->set(GRB_IntParam_Threads, opts.get<int>("threads"));
  env->set(GRB_IntParam_OutputFlag, 0);

  if (add_user_cuts) add_lazy_constraints = true;

  TaskProxy task_proxy(*task);
  OperatorsProxy ops = task_proxy.get_operators();
  for (size_t op_id = 0; op_id < ops.size(); ++op_id) {
    const OperatorProxy &op = ops[op_id];
    ap_float cost = op.get_cost();
    if (cost < min_action_cost) min_action_cost = cost;
  }

  min_cost_diff = min_action_cost;

  for (size_t op_id = 0; op_id < ops.size(); ++op_id) {
    const OperatorProxy &op = ops[op_id];
    ap_float cost = op.get_cost();
    for (size_t op_id2 = op_id + 1; op_id2 < ops.size(); ++op_id2) {
      ap_float diff = fabs(cost - ops[op_id2].get_cost());
      if (diff > 0) min_cost_diff = std::min(diff, min_cost_diff);
    }
  }
}

GurobiIPCompilation::~GurobiIPCompilation() { delete env; }

void GurobiIPCompilation::initialize(const int horizon) {
  TaskProxy task_proxy(*task);
  OperatorsProxy ops = task_proxy.get_operators();
  action_mutex.resize(ops.size(), std::vector<bool>(ops.size(), false));
  use_callback = add_lazy_constraints || add_user_cuts;
  std::vector<std::vector<bool>> action_precedence;

  if (use_callback)
    action_precedence.resize(ops.size(), std::vector<bool>(ops.size(), false));

  for (auto generator : constraint_generators)
    generator->initialize(horizon, task, action_mutex, use_linear_effects);

  if (use_callback) {
    for (auto generator : constraint_generators)
      generator->add_action_precedence(task, action_precedence, action_mutex);
  }

  if (lazy_mutex) {
    for (size_t op_id1 = 0; op_id1 < ops.size() - 1; ++op_id1) {
      for (size_t op_id2 = op_id1 + 1; op_id2 < ops.size(); ++op_id2) {
        if (action_mutex[op_id1][op_id2]) {
          action_precedence[op_id1][op_id2] = true;
          action_precedence[op_id2][op_id1] = true;
        }
      }
    }
  }

  if (use_callback) {
    graph = std::make_shared<ActionPrecedenceGraph>(action_precedence);
    int n_edges = graph->get_n_edges();
    std::cout << "action precedence graph has " << ops.size() << " nodes" << std::endl;
    std::cout << "action precedence graph has " << n_edges << " edges" << std::endl;
    if (graph->has_no_cycle()) {
      use_callback = false;
      std::cout << "action precedence graph has no cycle" << std::endl;
      std::cout << "callback is not used" << std::endl;
    } else {
      if (add_lazy_constraints) env->set(GRB_IntParam_LazyConstraints, 1);
      if (add_user_cuts) env->set(GRB_IntParam_PreCrush, 1);
    }
  }

  model = std::make_shared<GRBModel>(*env);
  add_variables(0, horizon);

  for (auto generator : constraint_generators)
    generator->update(horizon, task, model, x);

  if (!lazy_mutex)
    add_mutex_constraints(0, horizon);

  if (use_callback) {
      callback = std::make_shared<ActionCycleEliminationCallback>(
          max_num_cuts, add_user_cuts, add_one_time_step, x, graph);
      model->setCallback(callback.get());
  }
}

void GurobiIPCompilation::update(const int horizon) {
  int t_min = x.size();
  add_variables(t_min, horizon);

  for (auto generator : constraint_generators)
    generator->update(horizon, task, model, x);

  if (!lazy_mutex)
    add_mutex_constraints(t_min, horizon);
}

void GurobiIPCompilation::add_variables(const int t_min, const int t_max) {
  TaskProxy task_proxy(*task);
  OperatorsProxy ops = task_proxy.get_operators();
  x.resize(t_max, std::vector<GRBVar>(ops.size()));
  for (int t = t_min; t < t_max; ++t) {
    for (size_t op_id = 0; op_id < ops.size(); ++op_id) {
      const OperatorProxy &op = ops[op_id];
      std::string name = "x_" + std::to_string(op_id) + "_" + std::to_string(t);
      x[t][op_id] = model->addVar(0, 1, op.get_cost(), GRB_BINARY, name);
    }
  }
}

void GurobiIPCompilation::add_mutex_constraints(const int t_min,
                                                const int t_max) {
  TaskProxy task_proxy(*task);
  OperatorsProxy ops = task_proxy.get_operators();
  int n_mutex = 0;
  for (size_t op_id1 = 0; op_id1 < ops.size() - 1; ++op_id1) {
    for (size_t op_id2 = op_id1 + 1; op_id2 < ops.size(); ++op_id2) {
      if (action_mutex[op_id1][op_id2]) {
        ++n_mutex;
        for (int t = t_min; t < t_max; ++t) {
          model->addConstr(x[t][op_id1] + x[t][op_id2] <= 1);
        }
      }
    }
  }

  if (t_min == 0) std::cout << "mutex action pairs: " << n_mutex << std::endl;
}

void GurobiIPCompilation::add_sequence_constraint() {
  std::cout << "adding sequence constraints" << std::endl;
  TaskProxy task_proxy(*task);
  OperatorsProxy ops = task_proxy.get_operators();
  std::vector<double> coeff(ops.size(), 1);
  int horizon = x.size();
  for (int t = 0; t < horizon; ++t) {
    GRBLinExpr sum_t;
    sum_t.addTerms(coeff.data(), x[t].data(), ops.size());
    model->addConstr(sum_t <= 1);

    if (t < horizon - 1) {
      GRBLinExpr sum_t_1;
      sum_t_1.addTerms(coeff.data(), x[t + 1].data(), ops.size());
      model->addConstr(sum_t_1 <= sum_t);
    }
  }

  if (use_callback) model->setCallback(nullptr);
}

ap_float GurobiIPCompilation::compute_plan() {
  try {
    model->optimize();
  } catch (GRBException e) {
    std::cout << "Error number: " << e.getErrorCode() << std::endl;
    std::cout << e.getMessage() << std::endl;
  } catch (...) {
    std::cout << "Error during optimize" << std::endl;
  }

  node_count += model->get(GRB_DoubleAttr_NodeCount);

  int status = model->get(GRB_IntAttr_Status);

  if (status == GRB_OPTIMAL) return model->get(GRB_DoubleAttr_ObjVal);

  return -1;
}

SearchEngine::Plan GurobiIPCompilation::extract_plan() {
  TaskProxy task_proxy(*task);
  OperatorsProxy ops = task_proxy.get_operators();
  SearchEngine::Plan plan;
  int t_max = x.size();

  for (int t = 0; t < t_max; ++t) {
    //std::cout << "t=" << t << std::endl;
    if (graph != nullptr && graph->get_n_edges() > 0) {
      std::vector<int> nodes;
      std::unordered_map<int, int> ns;
      for (size_t op_id = 0; op_id < ops.size(); ++op_id) {
        int n = std::round(x[t][op_id].get(GRB_DoubleAttr_X));
        if (n > 0) {
          nodes.push_back(op_id);
          ns[op_id] = n;
        }
      }
      if (nodes.size() > 0) {
        auto subplan = graph->topological_sort(nodes);

        if (subplan.size() != nodes.size()) {
          std::cout << "plan contains a cycle" << std::endl;
          return SearchEngine::Plan();
        }

        for (auto op_id : subplan) {
          for (int i = 0; i < ns[op_id]; ++i) {
            plan.push_back(ops[op_id].get_global_operator());
            //std::cout << ops[op_id].get_name() << std::endl;
          }
        }
      }
    } else {
      for (size_t op_id = 0; op_id < ops.size(); ++op_id) {
        int n = std::round(x[t][op_id].get(GRB_DoubleAttr_X));
        for (int i = 0; i < n; ++i) {
          plan.push_back(ops[op_id].get_global_operator());
          //std::cout << ops[op_id].get_name() << std::endl;
        }
      }
    }
  }

  //for (auto generator : constraint_generators) generator->dump();

  return plan;
}

ap_float GurobiIPCompilation::get_min_action_cost() { return min_action_cost; }

ap_float GurobiIPCompilation::get_min_plan_cost_diff() { return min_cost_diff; }

void GurobiIPCompilation::print_statistics() const {
  int num_vars = model->get(GRB_IntAttr_NumVars);
  int num_constraints = model->get(GRB_IntAttr_NumConstrs);
  int num_additional_constraints = 0;

  if (use_callback) {
    num_additional_constraints = callback->get_num_constraints();
    std::cout << "Added constraints: " << num_additional_constraints
              << std::endl;
    std::cout << "Cuts: " << callback->get_num_cuts() << std::endl;
  }

  std::cout << "Vars: " << num_vars << std::endl;
  std::cout << "Constraints: " << num_constraints - num_additional_constraints
            << std::endl;
  std::cout << "Nodes: " << node_count << std::endl;
}