#include "lm_cut_numeric_constraints.h"

#include "../option_parser.h"
#include "../plugin.h"

#include "../lp/lp_solver.h"

#include "../utils/markup.h"
#include "../utils/memory.h"

#include <cassert>

using namespace std;

namespace operator_counting {
LMCutNumericConstraints::LMCutNumericConstraints(const Options &opts)
    : ceiling_less_than_one(opts.get<bool>("ceiling_less_than_one")),
      ignore_numeric(opts.get<bool>("ignore_numeric")),
      use_random_pcf(opts.get<bool>("random_pcf")),
      use_irmax(opts.get<bool>("irmax")),
      disable_ma(opts.get<bool>("disable_ma")),
      use_second_order_simple(opts.get<bool>("use_second_order_simple")),
      use_constant_assignment(opts.get<bool>("use_constant_assignment")),
      use_bounds(opts.get<bool>("use_bounds")),
      precision(opts.get<ap_float>("precision")),
      epsilon(opts.get<ap_float>("epsilon")) {}

void LMCutNumericConstraints::initialize_constraints(
    const shared_ptr<AbstractTask> task, vector<lp::LPConstraint> & /*constraints*/,
    double /*infinity*/) {
    TaskProxy task_proxy(*task);
    landmark_generator = utils::make_unique_ptr<numeric_lm_cut_heuristic::LandmarkCutLandmarks>(
        task_proxy, ceiling_less_than_one, ignore_numeric, use_random_pcf, use_irmax, disable_ma,
        use_second_order_simple, precision, epsilon, use_constant_assignment, use_bounds);

}


bool LMCutNumericConstraints::update_constraints(const State &state,
                                          lp::LPSolver &lp_solver) {
    assert(landmark_generator);
    vector<lp::LPConstraint> constraints;
    double infinity = lp_solver.get_infinity();

    bool dead_end = landmark_generator->compute_landmarks(
        state, nullptr,
        [&](const vector<pair<double,int>> &op_ids, int /*cost*/) {
            constraints.emplace_back(1.0, infinity);
            lp::LPConstraint &landmark_constraint = constraints.back();
            for (pair<double,int> op_id : op_ids) {
                //cout << op_id.second << " " << op_id.first << endl;
                landmark_constraint.insert(op_id.second, 1./op_id.first);
            }
            //cout << "-" << endl;
        });
    //cout << "--" << endl;

    if (dead_end) {
        return true;
    } else {
        lp_solver.add_temporary_constraints(constraints);
        return false;
    }
}

static shared_ptr<ConstraintGenerator> _parse(OptionParser &parser) {
    parser.document_synopsis(
        "LM-cut landmark constraints",
        "Computes a set of landmarks in each state using the LM-cut method. "
        "For each landmark L the constraint sum_{o in L} Count_o >= 1 is added "
        "to the operator counting LP temporarily. After the heuristic value "
        "for the state is computed, all temporary constraints are removed "
        "again. For details, see" + utils::format_paper_reference(
            {"Florian Pommerening", "Gabriele Roeger", "Malte Helmert",
             "Blai Bonet"},
            "LP-based Heuristics for Cost-optimal Planning",
            "http://www.aaai.org/ocs/index.php/ICAPS/ICAPS14/paper/view/7892/8031",
            "Proceedings of the Twenty-Fourth International Conference"
            " on Automated Planning and Scheduling (ICAPS 2014)",
            "226-234",
            "AAAI Press 2014") + utils::format_paper_reference(
            {"Blai Bonet"},
            "An admissible heuristic for SAS+ planning obtained from the"
            " state equation",
            "http://ijcai.org/papers13/Papers/IJCAI13-335.pdf",
            "Proceedings of the Twenty-Third International Joint"
            " Conference on Artificial Intelligence (IJCAI 2013)",
            "2268-2274",
            "2013"));

    parser.add_option<bool>("ceiling_less_than_one", "use 1 instead of m_a when m_a < 1", "false");
    parser.add_option<bool>("ignore_numeric", "ignore numeric conditions", "false");
    parser.add_option<bool>("random_pcf", "use randomized precondition choice function", "false");
    parser.add_option<bool>("irmax", "use repetition relaxation for pcf", "false");
    parser.add_option<bool>("disable_ma", "use m_a = 1", "false");
    parser.add_option<bool>("use_second_order_simple", "exploit second order simple effects", "false");
    parser.add_option<bool>("use_constant_assignment", "relax constant assignment effects to simple effects", "false");
    parser.add_option<bool>("use_bounds", "use bounds of numeric variables and effects", "false");
    parser.add_option<ap_float>("precision", "values less than this value are considered as zero", "0.000001");
    parser.add_option<ap_float>("epsilon", "small value added to strict inequalities", "0");

    if (parser.dry_run())
        return nullptr;

    Options opts = parser.parse();
    return make_shared<LMCutNumericConstraints>(opts);
}

static PluginShared<ConstraintGenerator> _plugin("lmcutnumeric_constraints", _parse);
}
