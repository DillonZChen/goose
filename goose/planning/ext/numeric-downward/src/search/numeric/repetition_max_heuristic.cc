#include "repetition_max_heuristic.h"
#include "interval.h"
#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

using namespace std;

namespace repetition_max_heuristic{

void RepetitionMaxHeuristic::initialize() {
	if(DEBUG) cout << "Initializing Interval repetition relaxed 'max' heuristic..." << endl;
	RepetitionRelaxationHeuristic::initialize();
}

ap_float RepetitionMaxHeuristic::compute_heuristic(const GlobalState& global_state) {
	State state = convert_global_state(global_state);
	//	if(DEBUG) cout << "Computing heuristic estimate Step 1: relaxed exploration :" << endl;
	setup_exploration_queue(state);
	if(DEBUG) cout << "First relaxed exploration -> reachability test " << endl;
	relaxed_exploration(true);

	for (auto &numvar : numeric_variables) {
		assert(!numvar.is_blocked());
		numvar.set_max_to_current_val();
	}
//	for (auto &numvar: numeric_cycle_breaker_vars) {
//		assert(!numvar.is_blocked());
//		numvar.set_max_to_current_val();
//	}
	setup_exploration_queue(state);

	if(DEBUG) cout << "Second relaxed exploration -> h_max computation" << endl;
	relaxed_exploration(false, true);

	if(DEBUG) {
		cout << "Done with relaxed exploration, dumping numeric variables and all achievers " << endl;
		for(auto nvar: numeric_variables) {
			nvar.dump();
			//		cout << " Achievers: " << endl;
			//		for (size_t i=0; i < nvar.get_achievers().size(); ++i) {
			//			NumericAchiever &achiever = nvar.get_achievers()[i];
			//			assert(achiever.reached_by);
			//			cout << " - " << i << ": " << achiever.reached_interval << " extended by "  << achiever.reached_by->str() << endl;
			//		}
			cout << "===================================================---" << endl;
		}
		cout << "Dumping cycle breaker variables " << endl;
		for(auto nvar: numeric_cycle_breaker_vars) {
			nvar.dump();
			cout << "===================================================---" << endl;
		}
	}
	//	if(DEBUG) cout << "Computing heuristic estimate Step 2: cost extraction :" << endl;
	ap_float total_cost = 0;
	for (const auto goal: goal_propositions) {
		if (DEBUG) cout << "Goal Prop ID " << goal->id << " costs " << goal->cost << endl;
		total_cost = update_cost(total_cost, goal->cost);
	}

	if(DEBUG) cout << "interval h_max estimate computed: " << total_cost;
	return total_cost;
}

static Heuristic *_parse(OptionParser &parser) {
	parser.document_synopsis("Repetition Relaxed Numeric Max heuristic", "");
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
		return new RepetitionMaxHeuristic(opts);
}

RepetitionMaxHeuristic::RepetitionMaxHeuristic(const options::Options& options)
: RepetitionRelaxationHeuristic(options)
{}

RepetitionMaxHeuristic::~RepetitionMaxHeuristic() {
}

static Plugin<Heuristic> _plugin("irhmax", _parse);

}
