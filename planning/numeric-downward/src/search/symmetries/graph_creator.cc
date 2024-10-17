#include "graph_creator.h"

#include <iostream>
#include <vector>

#include "../utils/timer.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "permutation.h"

using namespace std;
using namespace numeric_helper;

GraphCreator::GraphCreator(const Options &opts)
    : search_type(SymmetryBasedSearchType(opts.get_enum("symmetries"))),
      no_search(opts.get<bool>("no_search")),
      initialized(false),
      ignore_numeric(opts.get<bool>("ignore_numeric")),
      precision(opts.get<ap_float>("precision")) {
}


void GraphCreator::initialize(const std::shared_ptr<AbstractTask> task) {
    if (initialized)
        return;

    initialized = true;
    cout << "Initializing symmetry " << endl;
    group.initialize();

    bliss::Digraph* graph = create_bliss_directed_graph(task);

    graph->set_splitting_heuristic(bliss::Digraph::shs_flm);

    bliss::Stats stats1;
    cout << "Using Bliss to find group generators" << endl;
    graph->canonical_form(stats1,&(Group::add_permutation),&group);
    auto start = utils::g_timer();
    cout << "Got " << group.get_num_generators() << " group generators, time step: [t=" << start << "]" << endl;

    group.default_direct_product();
    cout<<"Number of generators: "<< group.get_num_generators()<<endl;

    // Deleting the graph
    delete graph;

    auto end = utils::g_timer();
    cout << "Done initializing symmetries [t=" << end << "]" << endl;
    cout << "Detecting symmetry takes " << end - start << endl;

    if (no_search)
        exit(0);
}



GraphCreator::~GraphCreator() {
    // Freeing the group
    free_memory();
    // Nothing to delete here
}

int GraphCreator::float_to_int(ap_float value) const {
    return static_cast<int>(std::round(value / precision));
}

bliss::Digraph* GraphCreator::create_bliss_directed_graph(const std::shared_ptr<AbstractTask> task) const {
   // initialization

    TaskProxy task_proxy(*task);
    NumericTaskProxy::redundant_constraints = false;
    NumericTaskProxy numeric_task(task_proxy, false, true);

    std::cout << "Computing number of vertices" << std::endl;
    VariablesProxy variables = task_proxy.get_variables();
    int num_of_vertex = 0;
    for (auto var : variables) {
        if (numeric_task.is_numeric_axiom(var.get_id())) {
            Permutation::var_to_regular_id.push_back(-1);
        } else {
            Permutation::var_to_regular_id.push_back(num_of_vertex);
            Permutation::regular_id_to_var.push_back(var.get_id());
            ++num_of_vertex;
        }
    }
    for (auto var : variables) {
        if (!numeric_task.is_numeric_axiom(var.get_id())) {
            Permutation::dom_sum_by_regular_id.push_back(num_of_vertex);
            num_of_vertex += var.get_domain_size();
            for (int num_of_value = 0; num_of_value < var.get_domain_size(); ++num_of_value) {
                Permutation::var_by_val.push_back(var.get_id());
            }
        }
    }
    Permutation::dom_sum_num_var = num_of_vertex;
    NumericVariablesProxy num_variables = task_proxy.get_numeric_variables();
    for (auto num_var : num_variables) {
        if (num_var.get_var_type() == regular) {
            Permutation::num_var_to_regular_id.push_back(num_of_vertex - Permutation::dom_sum_num_var);
            Permutation::regular_id_to_num_var.push_back(num_var.get_id());
            ++num_of_vertex;
        } else {
            Permutation::num_var_to_regular_id.push_back(-1);
        }
    }
    Permutation::length = num_of_vertex;

    std::cout << "Building a NPDG" << std::endl;
    bliss::Digraph* g = new bliss::Digraph();
    // add a vertex for each variable
    for (auto var : variables) {
        if (!numeric_task.is_numeric_axiom(var.get_id())) {
            g->add_vertex(0);
        }
    }
    // add a vertex for each propositnal value 
    for (auto var : variables){
        if (!numeric_task.is_numeric_axiom(var.get_id())) {
            for (int j = 0; j < var.get_domain_size(); j++){
               unsigned int idx = g->add_vertex(0);
               g->add_edge(Permutation::get_index_by_var(var.get_id()), idx);
            }
        }
    }

    unsigned int current_color = 1;

    // add a vertex for each numeric variable
    for (auto num_var : num_variables) {
        if (num_var.get_var_type() == regular) {
            unsigned int color = 0;

            if (ignore_numeric) {
                color = current_color;
                current_color += 1;
            }

            g->add_vertex(color);
        }
    }
    // add a vertex for a dummy numeric variable for constants
    unsigned int dummy_color = 0;
    if (ignore_numeric) {
        dummy_color = current_color;
        current_color += 1;
    }
    int dummy_var_idx = g->add_vertex(dummy_color);

    // add vertices for numeric constants
    std::unordered_map<int, unsigned int> constant_to_color; 
    for (OperatorProxy op : task_proxy.get_operators()) {
        for (int pre : numeric_task.get_action_num_list(op.get_id())){
            for (int i : numeric_task.get_numeric_conditions_id(pre)){
                const LinearNumericCondition &lnc = numeric_task.get_condition(i);
                ap_float denominator = 1.0;
                bool first = true;
                for (auto coeff : lnc.coefficients) {
                    if (std::fabs(coeff) >= precision) {
                        if (first) {
                            denominator = std::fabs(coeff);
                            first = false;
                        }
                        int normalized_coeff = float_to_int(coeff / denominator);
                        if (constant_to_color.find(normalized_coeff) == constant_to_color.end()) {
                            constant_to_color.insert(std::make_pair(normalized_coeff, current_color));
                            current_color += 1;
                        }
                    }
                }
                int normalized_constant = float_to_int(lnc.constant / denominator);
                if (constant_to_color.find(normalized_constant) == constant_to_color.end()) {
                    constant_to_color.insert(std::make_pair(normalized_constant, current_color));
                    current_color += 1;
                }
            }

            for (ap_float k : numeric_task.get_action_eff_list(op.get_id())) {
                if (std::fabs(k) >= precision) {
                    int normalized_k = float_to_int(k);
                    if (constant_to_color.find(normalized_k) == constant_to_color.end()) {
                        constant_to_color.insert(std::make_pair(normalized_k, current_color));
                        current_color += 1;
                    }
                }
            }

            for (size_t i = 0; i < numeric_task.get_action_n_linear_eff(op.get_id()); ++i) {
                ap_float denominator = 1.0;
                bool first = true;
                for (auto coeff : numeric_task.get_action_linear_coefficients(op.get_id())[i]) {
                    if (std::fabs(coeff) >= precision) {
                        if (first) {
                            denominator = std::fabs(coeff);
                            first = false;
                        }
                        int normalized_coeff = float_to_int(coeff / denominator);
                        if (constant_to_color.find(normalized_coeff) == constant_to_color.end()) {
                            constant_to_color.insert(std::make_pair(normalized_coeff, current_color));
                            current_color += 1;
                        }
                    }
                }
                ap_float constant = numeric_task.get_action_linear_constants(op.get_id())[i];
                int normalized_constant = float_to_int(constant / denominator);
                if (constant_to_color.find(normalized_constant) == constant_to_color.end()) {
                    constant_to_color.insert(std::make_pair(normalized_constant, current_color));
                    current_color += 1;
                }
            }
        }
    }
    for (size_t id_goal = 0; id_goal < numeric_task.get_n_numeric_goals(); ++id_goal) {
        for (int id_n_con : numeric_task.get_numeric_goals(id_goal)) {
            const LinearNumericCondition &lnc = numeric_task.get_condition(id_n_con);
            ap_float denominator = 1.0;
            bool first = true;
            for (auto coeff : lnc.coefficients) {
                if (std::fabs(coeff) >= precision) {
                    if (first) {
                        denominator = std::fabs(coeff);
                        first = false;
                    }
                    int normalized_coeff = float_to_int(coeff / denominator);
                    if (constant_to_color.find(normalized_coeff) == constant_to_color.end()) {
                        constant_to_color.insert(std::make_pair(normalized_coeff, current_color));
                        current_color += 1;
                    }
                }
            }
            int normalized_constant = float_to_int(lnc.constant / denominator);
            if (constant_to_color.find(normalized_constant) == constant_to_color.end()) {
                constant_to_color.insert(std::make_pair(normalized_constant, current_color));
                current_color += 1;
            }
        }
    }

    // determine colors for operators
    std::unordered_map<int, unsigned int> cost_to_color; 
    for (OperatorProxy op : task_proxy.get_operators()) {
        int cost = float_to_int(op.get_cost());
        if (cost_to_color.find(cost) == cost_to_color.end()) {
            cost_to_color.insert(std::make_pair(cost, current_color));
            current_color += 1;
        }
    }
    // determine colors for axioms 
    std::unordered_map<int, unsigned int> axiom_cost_to_color; 
    for (OperatorProxy op : task_proxy.get_axioms()) {
        int cost = float_to_int(op.get_cost());
        if (axiom_cost_to_color.find(cost) == axiom_cost_to_color.end()) {
            axiom_cost_to_color.insert(std::make_pair(cost, current_color));
            current_color += 1;
        }
    }

    unsigned int gt_color = current_color;
    unsigned int ge_color = current_color + 1;
    unsigned int effect_color = current_color + 2;
    unsigned int goal_color = current_color + 3;

    // add vertices for operators
    std::unordered_map<std::pair<int, int>, unsigned int> num_var_constant_to_idx;
    for (auto op : task_proxy.get_operators()) {
        unsigned int op_color = cost_to_color[float_to_int(op.get_cost())];
        unsigned int op_idx = g->add_vertex(op_color);

        for (auto pre : op.get_preconditions()) {
            if (!numeric_task.is_numeric_axiom(pre.get_variable().get_id())) {
                int var = pre.get_variable().get_id();
                int pre_val = pre.get_value();
                int pre_idx = Permutation::get_index_by_var_val(var, pre_val);
                g->add_edge(pre_idx, op_idx);
            }
        }
        for (auto eff : op.get_effects()) {
            FactProxy eff_fact = eff.get_fact();
            int eff_idx = Permutation::get_index_by_var_val(eff_fact.get_variable().get_id(), eff_fact.get_value());
            g->add_edge(op_idx, eff_idx);
        }

        for (int pre : numeric_task.get_action_num_list(op.get_id())) {
            for (int i : numeric_task.get_numeric_conditions_id(pre)) {
                const LinearNumericCondition &lnc = numeric_task.get_condition(i);
                unsigned int lnc_color = lnc.is_strictly_greater ? gt_color : ge_color;
                unsigned int lnc_idx = g->add_vertex(lnc_color);
                g->add_edge(lnc_idx, op_idx);

                ap_float denominator = 1.0;
                bool first = true;
                for (int j = 0, n = Permutation::regular_id_to_num_var.size(); j < n; ++j) {
                    if (std::fabs(lnc.coefficients[j]) >= precision) {
                        if (first) {
                            denominator = std::fabs(lnc.coefficients[j]);
                            first = false;
                        }
                        int var_idx = Permutation::get_index_by_num_regular_id(j);
                        int coeff = float_to_int(lnc.coefficients[j] / denominator);
                        auto key = std::make_pair(var_idx, coeff);
                        auto result = num_var_constant_to_idx.find(key);
                        if (result == num_var_constant_to_idx.end()) {
                            unsigned int idx = g->add_vertex(constant_to_color[coeff]);
                            num_var_constant_to_idx[key] = idx;
                            g->add_edge(var_idx, idx);
                            g->add_edge(idx, lnc_idx);
                        } else {
                            unsigned int idx =(*result).second;
                            g->add_edge(var_idx, idx);
                            g->add_edge(idx, lnc_idx);
                        }
                    }
                }
                int constant = float_to_int(lnc.constant / denominator);
                auto key = std::make_pair(dummy_var_idx, constant);
                auto result = num_var_constant_to_idx.find(key);
                if (result == num_var_constant_to_idx.end()) {
                    unsigned int idx = g->add_vertex(constant_to_color[constant]);
                    num_var_constant_to_idx[key] = idx;
                    g->add_edge(dummy_var_idx, idx);
                    g->add_edge(idx, lnc_idx);
                } else {
                    unsigned int idx =(*result).second;
                    g->add_edge(dummy_var_idx, idx);
                    g->add_edge(idx, lnc_idx);
                }
            }
        }

        for (int i = 0, n = Permutation::regular_id_to_num_var.size(); i < n; ++i) {
            ap_float k = numeric_task.get_action_eff_list(op.get_id())[i];
            if (std::fabs(k) >= precision) {
                unsigned int eff_idx = g->add_vertex(effect_color);
                g->add_edge(op_idx, eff_idx);
                int k_int = float_to_int(k);
                int var_idx = Permutation::get_index_by_num_regular_id(i);
                g->add_edge(eff_idx, var_idx);
                auto key = std::make_pair(dummy_var_idx, k_int);
                auto result = num_var_constant_to_idx.find(key);
                if (result == num_var_constant_to_idx.end()) {
                    unsigned int k_color = constant_to_color[k_int];
                    int idx = g->add_vertex(k_color);
                    num_var_constant_to_idx[key] = idx;
                    g->add_edge(dummy_var_idx, idx);
                    g->add_edge(idx, eff_idx);
                } else {
                    int idx = (*result).second;
                    g->add_edge(dummy_var_idx, idx);
                    g->add_edge(idx, eff_idx);
                }
            }
        }

        for (size_t i = 0; i < numeric_task.get_action_n_linear_eff(op.get_id()); ++i) {
            unsigned int eff_idx = g->add_vertex(effect_color);
            g->add_edge(op_idx, eff_idx);
            int lhs = numeric_task.get_action_linear_lhs(op.get_id())[i];
            unsigned int lhs_idx = Permutation::get_index_by_num_regular_id(lhs);
            g->add_edge(eff_idx, lhs_idx);

            ap_float denominator = 1.0;
            bool first = true;

            for (int j = 0, n = Permutation::regular_id_to_num_var.size(); j < n; ++j) {
                ap_float coeff = numeric_task.get_action_linear_coefficients(op.get_id())[i][j];
                if (std::fabs(coeff) >= precision) {
                    if (first) {
                        denominator = coeff;
                        first = false;
                    }
                    int var_idx = Permutation::get_index_by_num_regular_id(j);
                    int coeff_int = float_to_int(coeff / denominator);
                    auto key = std::make_pair(var_idx, coeff_int);
                    auto result = num_var_constant_to_idx.find(key);
                    if (result == num_var_constant_to_idx.end()) {
                        int coeff_color = constant_to_color[coeff_int];
                        unsigned int idx = g->add_vertex(coeff_color);
                        num_var_constant_to_idx[key] = idx;
                        g->add_edge(var_idx, idx);
                        g->add_edge(idx, eff_idx);
                    } else {
                        unsigned int idx = (*result).second;
                        g->add_edge(var_idx, idx);
                        g->add_edge(idx, eff_idx);
                    }
                }
            }

            ap_float constant = numeric_task.get_action_linear_constants(op.get_id())[i];
            if (std::fabs(constant) >= precision) {
                int constant_int = float_to_int(constant / denominator);
                auto key = std::make_pair(dummy_var_idx, constant_int);
                auto result = num_var_constant_to_idx.find(key);
                if (result == num_var_constant_to_idx.end()) {
                    int constant_color = constant_to_color[constant_int];
                    unsigned int idx = g->add_vertex(constant_color);
                    num_var_constant_to_idx[key] = idx;
                    g->add_edge(dummy_var_idx, idx);
                    g->add_edge(idx, eff_idx);
                } else {
                    unsigned int idx = (*result).second;
                    g->add_edge(dummy_var_idx, idx);
                    g->add_edge(idx, eff_idx);
                }
            }
        }
    }
    
    // add vertices for propositional axioms 
    for (auto op : task_proxy.get_axioms()) {
        int op_color = cost_to_color[float_to_int(op.get_cost())];
        int op_idx = g->add_vertex(op_color);

        for (auto pre : op.get_preconditions()) {
            if (!numeric_task.is_numeric_axiom(pre.get_variable().get_id())) {
                int var = pre.get_variable().get_id();
                int pre_val = pre.get_value();
                int pre_idx = Permutation::get_index_by_var_val(var, pre_val);
                g->add_edge(pre_idx, op_idx);
            }
        }
        for (auto eff : op.get_effects()) {
            FactProxy eff_fact = eff.get_fact();
            int eff_idx = Permutation::get_index_by_var_val(eff_fact.get_variable().get_id(), eff_fact.get_value());
            g->add_edge(op_idx, eff_idx);
        }

        for (int pre : numeric_task.get_action_num_list(task_proxy.get_operators().size() + op.get_id())) {
            for (int i : numeric_task.get_numeric_conditions_id(pre)) {
                const LinearNumericCondition &lnc = numeric_task.get_condition(i);
                unsigned int lnc_color = lnc.is_strictly_greater ? gt_color : ge_color;
                unsigned int lnc_idx = g->add_vertex(lnc_color);
                g->add_edge(lnc_idx, op_idx);

                ap_float denominator = 1.0;
                bool first = true;
                for (int j = 0, n = Permutation::regular_id_to_num_var.size(); j < n; ++j) {
                    if (std::fabs(lnc.coefficients[j]) >= precision) {
                        if (first) {
                            denominator = std::fabs(lnc.coefficients[j]);
                            first = false;
                        }
                        int var_idx = Permutation::get_index_by_num_regular_id(j);
                        int coeff = float_to_int(lnc.coefficients[j] / denominator);
                        auto key = std::make_pair(var_idx, coeff);
                        auto result = num_var_constant_to_idx.find(key);
                        if (result == num_var_constant_to_idx.end()) {
                            unsigned int idx = g->add_vertex(constant_to_color[coeff]);
                            num_var_constant_to_idx[key] = idx;
                            g->add_edge(var_idx, idx);
                            g->add_edge(idx, lnc_idx);
                        } else {
                            unsigned int idx =(*result).second;
                            g->add_edge(var_idx, idx);
                            g->add_edge(idx, lnc_idx);
                        }
                    }
                }
                int constant = float_to_int(lnc.constant / denominator);
                auto key = std::make_pair(dummy_var_idx, constant);
                auto result = num_var_constant_to_idx.find(key);
                if (result == num_var_constant_to_idx.end()) {
                    unsigned int idx = g->add_vertex(constant_to_color[constant]);
                    num_var_constant_to_idx[key] = idx;
                    g->add_edge(dummy_var_idx, idx);
                    g->add_edge(idx, lnc_idx);
                } else {
                    unsigned int idx =(*result).second;
                    g->add_edge(dummy_var_idx, idx);
                    g->add_edge(idx, lnc_idx);
                }
            }
        }
    }

    // adding edges to goal vertex
    int goal_idx = g->add_vertex(goal_color);
    for (FactProxy goal : task_proxy.get_goals()) {
        if(!numeric_task.is_numeric_axiom(goal.get_variable().get_id())){
            int val_idx = Permutation::get_index_by_var_val(goal.get_variable().get_id(), goal.get_value());
            g->add_edge(val_idx, goal_idx);
        }
    }
    for (size_t id_goal = 0; id_goal < numeric_task.get_n_numeric_goals(); ++id_goal) {
        for (pair<int, int> var_value: numeric_task.get_propositional_goals(id_goal)) {
            if (!numeric_task.is_numeric_axiom(var_value.first)) {
                int val_idx = Permutation::get_index_by_var_val(var_value.first, var_value.second);
                g->add_edge(val_idx, goal_idx);
            }
        }
        for (int id_n_con : numeric_task.get_numeric_goals(id_goal)) {
            const LinearNumericCondition &lnc = numeric_task.get_condition(id_n_con);
            unsigned int lnc_color = lnc.is_strictly_greater ? gt_color : ge_color;
            unsigned int lnc_idx = g->add_vertex(lnc_color);
            g->add_edge(lnc_idx, goal_idx);

            ap_float denominator = 1.0;
            bool first = true;

            for (int j = 0, n = Permutation::regular_id_to_num_var.size(); j < n; ++j) {
                if (std::fabs(lnc.coefficients[j]) >= precision) {
                    if (first) {
                        denominator = std::fabs(lnc.coefficients[j]);
                        first = false;
                    }
                    int var_idx = Permutation::get_index_by_num_regular_id(j);
                    int coeff = float_to_int(lnc.coefficients[j] / denominator);
                    auto key = std::make_pair(var_idx, coeff);
                    auto result = num_var_constant_to_idx.find(key);
                    if (result == num_var_constant_to_idx.end()) {
                        unsigned int idx = g->add_vertex(constant_to_color[coeff]);
                        num_var_constant_to_idx[key] = idx;
                        g->add_edge(var_idx, idx);
                        g->add_edge(idx, lnc_idx);
                    } else {
                        unsigned int idx =(*result).second;
                        g->add_edge(var_idx, idx);
                        g->add_edge(idx, lnc_idx);
                    }
                }
            }
            int constant = float_to_int(lnc.constant / denominator);
            auto key = std::make_pair(dummy_var_idx, constant);
            auto result = num_var_constant_to_idx.find(key);
            if (result == num_var_constant_to_idx.end()) {
                unsigned int idx = g->add_vertex(constant_to_color[constant]);
                num_var_constant_to_idx[key] = idx;
                g->add_edge(dummy_var_idx, idx);
                g->add_edge(idx, lnc_idx);
            } else {
                unsigned int idx =(*result).second;
                g->add_edge(dummy_var_idx, idx);
                g->add_edge(idx, lnc_idx);
            }
        }
    }

    NumericTaskProxy::redundant_constraints = true;

    return g;
}

Permutation GraphCreator::create_permutation_from_state_to_state(const GlobalState &from_state, const GlobalState &to_state) const {
    std::vector<container_int> tmp_state(g_variable_domain.size());
    for (size_t i = 0; i < g_variable_domain.size(); ++i)
        tmp_state[i] = from_state[i];
    std::vector<ap_float> tmp_numeric_values = from_state.get_numeric_vars();
    std::vector<int> from_trace = group.get_trace(tmp_state, tmp_numeric_values);

    for (size_t i = 0; i < g_variable_domain.size(); ++i)
        tmp_state[i] = to_state[i];
    tmp_numeric_values = to_state.get_numeric_vars();
    std::vector<int> to_trace = group.get_trace(tmp_state, tmp_numeric_values);

    Permutation p1(group.compose_permutation(to_trace), true);
    Permutation p2 = group.compose_permutation(from_trace);
    
    return Permutation(p2, p1);
}

void GraphCreator::add_options_to_parser(OptionParser &parser) {

    vector<string> sym_types;
    sym_types.push_back("none");
    sym_types.push_back("goal_only");
    parser.add_enum_option("symmetries", sym_types, "use symmetries", "goal_only");

    parser.add_option<bool>("no_search",
                           "No search is performed, exiting after creating the symmetries",
                            "false");
    parser.add_option<bool>("ignore_numeric",
                           "Ignore numeric values by assign different colors to numeric constants",
                            "false");


    parser.add_option<ap_float>("precision", "Threshold below which is considered as zero", "0.00001");
}



static GraphCreator *_parse(OptionParser &parser) {
//	cout << "=========================================================================================================" << endl;
//	cout << "======     Running symmetry parser" << endl;
//	cout << "=========================================================================================================" << endl;
    GraphCreator::add_options_to_parser(parser);
    Options opts = parser.parse();

    SymmetryBasedSearchType type = SymmetryBasedSearchType(opts.get_enum("symmetries"));
    if (type == NO_SYMMETRIES) {
        return 0;
    }
    /*
    if (type == INIT_AND_GOAL_STABILIZED_REGULAR_SEARCH) {
        // To enable this, just use stabilizing initial state in PDG.
        cout << "Init and goal stabilized symmetry pruning (Pochter et. al.) is disabled in this version" << endl;
        exit(1);
    }
    */
    if (!parser.dry_run()) {

    if (type == GOAL_ONLY_STABILIZED) {
        GraphCreator* gr = new GraphCreator(opts);
        if (gr) {
            cout << "Creating symmetry graph stabilizing goal only" << endl;;
        }

        return gr;
    }

    cout << "Invalid option for symmetry type" << endl;
    exit(1);
    } else {
        return 0;
    }

}


static Plugin<GraphCreator> _plugin("symmetry_state_pruning", _parse);
