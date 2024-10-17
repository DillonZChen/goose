#include "interval_add_heuristic.h"
#include "interval.h"
#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

using namespace std;
using namespace interval_relaxation_heuristic;

namespace interval_add_heuristic {


static Heuristic *_parse(OptionParser &parser) {
    parser.document_synopsis("Interval Numeric Additive heuristic", "");
    parser.document_language_support("action costs", "supported");
    parser.document_language_support("conditional effects", "supported");
    parser.document_language_support("numeric", "supported");
    parser.document_language_support(
        "axioms",
        "supported (in the sense that the planner won't complain -- "
        "handling of axioms might be very stupid "
        "and even render the heuristic unsafe)");
    parser.document_property("admissible", "yes for tasks without axioms");
    parser.document_property("consistent", "yes for tasks without axioms");
    parser.document_property("safe", "yes for tasks without axioms");
    parser.document_property("preferred operators", "no");

    Heuristic::add_options_to_parser(parser);
    Options opts = parser.parse();

    if (parser.dry_run())
        return 0;
    else
        return new IntervalAddHeuristic(opts);
}

void IntervalAddHeuristic::initialize() {
    if(DEBUG) cout << "Initializing (old-fashioned) Interval relaxed additive heuristic..." << endl;
    IntervalRelaxationHeuristic::initialize();
}

ap_float IntervalAddHeuristic::compute_heuristic(
		const GlobalState& global_state) {
	State state = convert_global_state(global_state);
	setup_exploration(state);
//	cout << "Starting exploration from initial state s_0" << endl;
	assert (planning_graph.size() == 1);
//	planning_graph.front().dump();
	relaxed_exploration();
	ap_float total_cost = 0;

	for (const auto goal: goal_propositions) {
//		cout << "Goal Prop ID " << goal->id << " costs " << goal->cost << endl;
		total_cost = update_cost(goal->cost, total_cost);
	}
	return total_cost;
}

IntervalAddHeuristic::IntervalAddHeuristic(const options::Options& options)
 : IntervalRelaxationHeuristic(options)
{}

IntervalAddHeuristic::~IntervalAddHeuristic() {
}

static Plugin<Heuristic> _plugin("iihadd", _parse);

}
