#include "heuristic.h"

#include "evaluation_context.h"
#include "evaluation_result.h"
#include "global_operator.h"
#include "globals.h"
#include "option_parser.h"
#include "operator_cost.h"
#include "plugin.h"

#include "tasks/cost_adapted_task.h"
#include "numeric_operator_counting/numeric_helper.h"

#include <cassert>
#include <cstdlib>
#include <limits>

using namespace std;

Heuristic::Heuristic(const Options &opts)
    : description(opts.get_unparsed_config()),
      initialized(false),
      multiplicator(0),
      heuristic_cache(HEntry(NO_VALUE_INT, true)), //TODO: is true really a good idea here?
      cache_h_values(opts.get<bool>("cache_estimates")),
      task(get_task_from_options(opts)),
      task_proxy(*task),
      cost_type(OperatorCost(opts.get_enum("cost_type"))) {
          
          numeric_helper::NumericTaskProxy::redundant_constraints = opts.get<bool>("redundant_constraints");
          
          if (opts.get<bool>("rounding_up")) compute_multiplicator(1e-5);
}

Heuristic::~Heuristic() {
}

void Heuristic::set_preferred(const GlobalOperator *op) {
    if (!op->is_marked()) {
        op->mark();
        preferred_operators.push_back(op);
    }
}

void Heuristic::set_preferred(OperatorProxy op) {
    set_preferred(op.get_global_operator());
}

bool Heuristic::reach_state(
    const GlobalState & /*parent_state*/,
    const GlobalOperator & /*op*/,
    const GlobalState & /*state*/) {
    return false;
}

ap_float Heuristic::get_adjusted_cost(const GlobalOperator &op) const {
    return get_adjusted_action_cost(op, cost_type);
}

State Heuristic::convert_global_state(const GlobalState &global_state) const {
    return task_proxy.convert_global_state(global_state);
}

void Heuristic::compute_multiplicator(ap_float epsilon) {
    for (OperatorProxy op : task_proxy.get_operators()) {
        int m = 1;
        ap_float cost = op.get_cost();
        ap_float integral_part = 0;
        ap_float fractional_part = std::modf(cost, &integral_part);

        while (std::fabs(fractional_part) > epsilon) {
            m *= 10;
            cost = fractional_part * 10;
            fractional_part = std::modf(cost, &integral_part);
        }

        if (m > multiplicator) multiplicator = m;
    }
}

void Heuristic::add_options_to_parser(OptionParser &parser) {
    ::add_cost_type_option_to_parser(parser);
    // TODO: When the cost_type option is gone, use "no_transform" as default.
    parser.add_option<shared_ptr<AbstractTask>>(
        "transform",
        "Optional task transformation for the heuristic. "
        "Currently only adapt_costs is available.",
        OptionParser::NONE);
    parser.add_option<bool>("cache_estimates", "cache heuristic estimates", "true");
    parser.add_option<bool>("redundant_constraints","add redundant_constraints", "true");
    parser.add_option<bool>("rounding_up","rounding up the heuristic value", "false");
}

// This solution to get default values seems nonoptimal.
// This is currently only used by the LAMA/FF synergy.
Options Heuristic::default_options() {
    Options opts = Options();
    opts.set<shared_ptr<AbstractTask>>("transform", g_root_task());
    opts.set<int>("cost_type", NORMAL);
    opts.set<bool>("cache_estimates", false);
    opts.set<bool>("redundant_constraints", false);
    opts.set<bool>("rounding_up", false);
    return opts;
}

EvaluationResult Heuristic::compute_result(EvaluationContext &eval_context) {
    EvaluationResult result;

    if (!initialized) {
        initialize();
        initialized = true;
    }

    assert(preferred_operators.empty());

    const GlobalState &state = eval_context.get_state();
    bool calculate_preferred = eval_context.get_calculate_preferred();

    ap_float heuristic = NO_VALUE;

    if (!calculate_preferred && cache_h_values &&
        heuristic_cache[state].h != NO_VALUE && !heuristic_cache[state].dirty) {
        heuristic = heuristic_cache[state].h;
        result.set_count_evaluation(false);
    } else {
        heuristic = compute_heuristic(state);
        if (cache_h_values) {
            heuristic_cache[state] = HEntry(heuristic, false);
        }
        for (const GlobalOperator *preferred_operator : preferred_operators)
            preferred_operator->unmark();
        result.set_count_evaluation(true);
    }

    assert(heuristic == DEAD_END || heuristic >= 0);

    if (heuristic == DEAD_END) {
        /*
          It is permissible to mark preferred operators for dead-end
          states (thus allowing a heuristic to mark them on-the-fly
          before knowing the final result), but if it turns out we
          have a dead end, we don't want to actually report any
          preferred operators.
        */
        preferred_operators.clear();
        heuristic = EvaluationResult::INFTY;
    }

#ifndef NDEBUG
    if (heuristic != EvaluationResult::INFTY) {
        for (size_t i = 0; i < preferred_operators.size(); ++i)
            assert(preferred_operators[i]->is_applicable(state));
    }
#endif

    if (multiplicator > 0)
        heuristic = std::ceil(heuristic * multiplicator) / multiplicator;

    result.set_h_value(heuristic);
    result.set_preferred_operators(move(preferred_operators));
    assert(preferred_operators.empty());
    return result;
}

std::vector<ap_float> Heuristic::compute_heuristic_batch(
    const std::vector<GlobalState> &states) {
    std::vector<ap_float> ret;
    for (const auto &state : states) {
        ret.push_back(compute_heuristic(state));
    }
    return ret;
}

std::vector<ap_float> Heuristic::compute_result_batch(
        const std::vector<GlobalState> &states) {
    return compute_heuristic_batch(states);
}

string Heuristic::get_description() const {
    return description;
}


static PluginTypePlugin<Heuristic> _type_plugin(
    "Heuristic",
    "A heuristic specification is either a newly created heuristic "
    "instance or a heuristic that has been defined previously. "
    "This page describes how one can specify a new heuristic instance. "
    "For re-using heuristics, see OptionSyntax#Heuristic_Predefinitions.\n\n"
    "Definitions of //properties// in the descriptions below:\n\n"
    " * **admissible:** h(s) <= h*(s) for all states s\n"
    " * **consistent:** h(s) <= c(s, s') + h(s') for all states s "
    "connected to states s' by an action with cost c(s, s')\n"
    " * **safe:** h(s) = infinity is only true for states "
    "with h*(s) = infinity\n"
    " * **preferred operators:** this heuristic identifies "
    "preferred operators ");
