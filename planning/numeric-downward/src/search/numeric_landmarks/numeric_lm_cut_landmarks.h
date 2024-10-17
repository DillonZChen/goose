#ifndef HEURISTICS_NUMERIC_LM_CUT_LANDMARKS_H
#define HEURISTICS_NUMERIC_LM_CUT_LANDMARKS_H

#include "../globals.h"
#include "../priority_queue.h"
#include "../task_tools.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "../utils/rng.h"
#include "numeric_bound.h"

#include <cassert>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace numeric_lm_cut_heuristic {
    // TODO: Fix duplication with the other relaxation heuristics.
    struct RelaxedProposition;
    
    enum PropositionStatus {
        UNREACHED = 0,
        REACHED = 1,
        GOAL_ZONE = 2,
        BEFORE_GOAL_ZONE = 3
    };
    
    struct RelaxedOperator {
        int id;
        int original_op_id_1;
        int original_op_id_2;
        std::vector<RelaxedProposition *> preconditions;
        std::vector<RelaxedProposition *> effects;
        std::vector<ap_float> sose_constants;
        bool conditional;
        bool infinite;
        
        ap_float base_cost_1; // 0 for axioms, 1 for regular operators
        ap_float base_cost_2; // 0 for axioms, 1 for regular operators
        
        ap_float cost_1;
        ap_float cost_2;
        int unsatisfied_preconditions;
        ap_float h_max_supporter_cost; // h_max_cost of h_max_supporter
        RelaxedProposition *h_max_supporter;
        
        string name;
        
        RelaxedOperator(std::vector<RelaxedProposition *> &&pre,
                        std::vector<RelaxedProposition *> &&eff,
                        int op_id, ap_float base, string &n, bool conditional)
        : original_op_id_1(-1),
          original_op_id_2(op_id),
          preconditions(pre),
          effects(eff),
          conditional(conditional),
          infinite(false),
          base_cost_1(0),
          base_cost_2(base),
          name(n) {
        }

        RelaxedOperator(std::vector<RelaxedProposition *> &&pre,
                        int op_id, ap_float base, string &n, bool conditional, bool infinite)
        : original_op_id_1(-1),
          original_op_id_2(op_id),
          preconditions(pre),
          conditional(conditional),
          infinite(infinite),
          base_cost_1(0),
          base_cost_2(base),
          name(n) {
        }

        RelaxedOperator(std::vector<RelaxedProposition *> &&pre_1, const std::vector<RelaxedProposition *> &pre_2,
                        std::vector<RelaxedProposition *> &&eff, std::vector<ap_float> &&sose_constants,
                        int op_id_1, int op_id_2, ap_float base_1, ap_float base_2, string &n_1, string& n_2)
        : original_op_id_1(op_id_1),
          original_op_id_2(op_id_2),
          preconditions(pre_1),
          effects(eff),
          sose_constants(sose_constants),
          conditional(false),
          infinite(false),
          base_cost_1(base_1),
          base_cost_2(base_2),
          name(n_1 + " " + n_2) {
          preconditions.insert(preconditions.end(), pre_2.begin(), pre_2.end());
        }

        void update_h_max_supporter();
        void select_random_supporter();
    };
    
    struct RelaxedProposition {
        std::vector<RelaxedOperator *> precondition_of;
        std::vector<RelaxedOperator *> effect_of;
        
        PropositionStatus status;
        bool explored;
        bool is_numeric_condition;
        int id_numeric_condition;
        ap_float h_max_cost;
        string name;
    };
    
    class LandmarkCutLandmarks {
        numeric_helper::NumericTaskProxy numeric_task;
        std::vector<RelaxedOperator> relaxed_operators;
        std::vector<std::vector<RelaxedProposition>> propositions;
        std::vector<numeric_helper::LinearNumericCondition> conditions;
        std::vector<ap_float> epsilons;
        RelaxedProposition artificial_precondition;
        RelaxedProposition artificial_goal;
        int num_propositions;
        int n_var;
        int n_infinite_operators;
        int n_second_order_simple_operators;
        bool ceiling_less_than_one;
        bool ignore_numeric_conditions;
        bool use_random_pcf;
        bool use_irmax;
        bool disable_ma;
        bool use_second_order_simple;
        bool use_constant_assignment;
        ap_float precision;
        ap_float epsilon;
        std::vector<ap_float> numeric_initial_state;
        std::vector<vector<RelaxedOperator *>> original_to_relaxed_operators;
        std::vector<std::vector<std::vector<int>>> linear_effect_to_conditions_plus;
        std::vector<std::vector<std::vector<int>>> linear_effect_to_conditions_minus;
        std::vector<int> condition_to_op_id;
        std::vector<std::vector<ap_float>> operator_to_simple_effects;
        std::vector<std::vector<std::vector<ap_float>>> operator_condition_to_composite_coefficients;
        std::vector<std::vector<bool>> operator_condition_to_has_upper_bound;
        std::vector<std::vector<ap_float>> operator_condition_to_upper_bound;
        bool use_bounds;
        numeric_bound::NumericBound numeric_bound;
        std::vector<std::vector<bool>> has_sose;
        std::vector<double> op_base_cost;
        
        HeapQueue<RelaxedProposition *> priority_queue;
        
        void initialize();
        ap_float calculate_base_operator_cost(size_t op_id) const;
        void build_relaxed_operator(const OperatorProxy &op, size_t op_id);
        std::vector<RelaxedProposition*> build_precondition(const OperatorProxy &op, size_t op_id);
        void add_linear_conditions(const OperatorProxy &op);
        size_t add_numeric_condition(numeric_helper::LinearNumericCondition lnc);
        std::vector<numeric_helper::LinearNumericCondition> make_redundant_conditions(const numeric_helper::LinearNumericCondition &lnc, const std::set<int> &condition_ids) const;
        bool has_effect(int op_id, const std::vector<ap_float> &coefficients) const;
        bool has_constant_assignment_effect(int op_id, const std::vector<ap_float> &coefficients, bool use_bounded_linear) const;
        bool has_linear_effect(int op_id, const std::vector<ap_float> &coefficients, bool use_bounded_linear, bool only_conditional) const;
        std::vector<ap_float> calculate_composite_coefficients(int op_id, const numeric_helper::LinearNumericCondition &lnc) const;
        std::pair<bool, ap_float> calculate_simple_effect_constant(int op_id, const std::vector<ap_float> &coefficients, bool use_bounded_linear) const;
        std::pair<bool, std::vector<std::pair<int, ap_float>>> get_sose_supporters(const TaskProxy &task_proxy, int op_id, const numeric_helper::LinearNumericCondition &lnc) const;
        void build_linear_operators(const TaskProxy &task_proxy, const OperatorProxy &op);
        void build_simple_effects();
        void delete_noops();
        RelaxedProposition *get_proposition(const FactProxy &fact);
        RelaxedProposition *get_proposition(const int &n_condition);
        void setup_exploration_queue();
        void setup_exploration_queue_state(const State &state);
        void first_exploration(const State &state);
        void first_exploration_incremental(const State &state, std::vector<RelaxedOperator *> &cut);
        void second_exploration(const State &state,
                                std::vector<RelaxedProposition *> &queue,
                                std::vector<RelaxedOperator *> &cut,
                                std::vector<std::pair<ap_float, ap_float>> &m_list);
        
        bool enqueue_if_necessary(RelaxedProposition *prop, ap_float cost) {
            assert(cost >= 0);
            if (prop->status == UNREACHED || prop->h_max_cost > cost) {
                prop->status = REACHED;
                prop->h_max_cost = cost;
                priority_queue.push(cost, prop);
                return true;
            }
            return false;
        }
        
        void update_queue(const State &state, RelaxedProposition *prec, RelaxedProposition *eff, RelaxedOperator *op);
        std::pair<ap_float, ap_float> calculate_numeric_times(const State &state, RelaxedProposition *effect, RelaxedOperator *relaxed_op, bool use_ma);
        ap_float calculate_constant_assignment_effect(const State &state, int op_id, const std::vector<ap_float> &coefficients, bool use_bounded_linear) const;
        ap_float calculate_linear_expression(const State &state, const std::vector<ap_float> &coefficients) const;
        
        void mark_goal_plateau(const State &state, RelaxedProposition *subgoal);
        void validate_h_max() const;
    public:
        using Landmark = std::vector<pair<ap_float,int>>;
        using CostCallback = std::function<void (ap_float)>;
        using LandmarkCallback = std::function<void (const Landmark &, int)>;
        
        LandmarkCutLandmarks(const TaskProxy &task_proxy, bool ceiling_less_than_one = false, bool ignore_numeric = false,
                             bool use_random_pcf = false, bool use_irmax = false, bool disable_ma = false,
                             bool use_second_order_simple = false, ap_float precision = 0.000001, ap_float epsilon = 0,
                             bool use_constant_assignment = false, int bound_iterations = 0);
        virtual ~LandmarkCutLandmarks();
        
        /*
         Compute LM-cut landmarks for the given state.
         
         If cost_callback is not nullptr, it is called once with the cost of each
         discovered landmark.
         
         If landmark_callback is not nullptr, it is called with each discovered
         landmark (as a vector of operator indices) and its cost. This requires
         making a copy of the landmark, so cost_callback should be used if only the
         cost of the landmark is needed.
         
         Returns true iff state is detected as a dead end.
         */
        bool compute_landmarks(State state, CostCallback cost_callback,
                               LandmarkCallback landmark_callback);
    };
    
    inline void RelaxedOperator::update_h_max_supporter() {
        assert(!unsatisfied_preconditions);
        for (size_t i = 0; i < preconditions.size(); ++i)
            if (preconditions[i]->h_max_cost > h_max_supporter->h_max_cost)
                h_max_supporter = preconditions[i];
        h_max_supporter_cost = h_max_supporter->h_max_cost;
    }

    inline void RelaxedOperator::select_random_supporter() {
        assert(!unsatisfied_preconditions);
        size_t n_zero = 0;
        for (size_t i = 0; i < preconditions.size(); ++i)
            if (preconditions[i]->h_max_cost <= 0) ++n_zero;
        if (n_zero == preconditions.size()) {
            int index = (*g_rng())(preconditions.size());
            h_max_supporter = preconditions[index];
        } else {
            int index = (*g_rng())(preconditions.size() - n_zero);
            int counter = 0;
            for (size_t i = 0; i < preconditions.size(); ++i) {
                if (preconditions[i]->h_max_cost > 0) {
                    if (counter == index) {
                        h_max_supporter = preconditions[i];
                        break;
                    }
                    ++counter;
                }
            }

        }
        h_max_supporter_cost = h_max_supporter->h_max_cost;
    }
}

#endif

