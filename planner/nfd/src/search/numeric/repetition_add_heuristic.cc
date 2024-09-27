#include "repetition_add_heuristic.h"
#include "interval.h"

#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

using namespace std;

namespace repetition_add_heuristic {

void RepetitionAddHeuristic::initialize() {
    if(DEBUG) cout << "Initializing Interval repetition relaxed 'add' heuristic..." << endl;
    RepetitionRelaxationHeuristic::initialize();
}

ap_float RepetitionAddHeuristic::compute_heuristic(const GlobalState& global_state) {
	State state = convert_global_state(global_state);
	if(DEBUG) cout << "Computing heuristic estimate Step 1: relaxed exploration :" << endl;
	setup_exploration_queue(state);
	relaxed_exploration();

	if(DEBUG) cout << "Computing heuristic estimate Step 2: cost extraction :" << endl;

	ap_float total_cost = 0;
	for (const auto goal: goal_propositions) {
//		cout << "Goal Prop ID " << goal->id << " costs " << goal->cost << endl;
		total_cost = update_cost(total_cost, goal->cost);
	}

	if(DEBUG) cout << "Computed heuristic estimate: " << total_cost << endl;
	return total_cost;
}

static Heuristic *_parse(OptionParser &parser) {
    parser.document_synopsis("Interval Numeric Add heuristic", "");
    parser.document_language_support("action costs", "supported");
    parser.document_language_support("conditional effects", "supported");
    parser.document_language_support("numeric", "supported");
    parser.document_language_support("axioms",
        "supported (in the sense that the planner won't complain -- "
        "handling of axioms might be very stupid "
        "and even render the heuristic unsafe)");
    parser.document_property("admissible", "no");
    parser.document_property("consistent", "no");
    parser.document_property("safe", "yes for tasks without axioms");
    parser.document_property("preferred operators", "no");

    Heuristic::add_options_to_parser(parser);
    Options opts = parser.parse();

    if (parser.dry_run())
        return 0;
    else
        return new RepetitionAddHeuristic(opts);
}


RepetitionAddHeuristic::RepetitionAddHeuristic(const options::Options& options)
	: RepetitionRelaxationHeuristic(options), did_write_overflow_warning(false)
{}

RepetitionAddHeuristic::~RepetitionAddHeuristic() {
}

static Plugin<Heuristic> _plugin("irhadd", _parse);

}
