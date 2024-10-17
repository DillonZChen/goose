#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

#include "../lp/lp_solver.h"

#include "../utils/markup.h"
#include "landmark_constraints.h"
#include "../numeric_operator_counting/numeric_helper.h"
#include "../numeric_landmarks/landmark_factory_scala.h"

using namespace std;
using namespace operator_counting;
using namespace numeric_helper;


void LandmarkConstraints::initialize_variables(const std::shared_ptr<AbstractTask> task,
                                           std::vector<lp::LPVariable> &variables,
                                           double infinity){
    factory = new landmarks::LandmarkFactoryScala(task);
    vector<set<int>> & landmarks_table = factory->get_landmarks_table();
    TaskProxy task_proxy(*task);
    State initial_state = task_proxy.get_initial_state();

    fact_landmarks = factory->compute_landmarks(initial_state);
    action_landmarks = factory->compute_action_landmarks(fact_landmarks);
    landmark_constraints_index.assign(action_landmarks.size(),-1);
}


void LandmarkConstraints::initialize_constraints(const std::shared_ptr<AbstractTask> task,
                                             std::vector<lp::LPConstraint> &constraints,
                                             double infinity){
    
    cout << "initializing constraints for landmarks" << endl;
    landmark_constraints(task, constraints, infinity,t_max);

}

bool LandmarkConstraints::update_constraints(const State &state, lp::LPSolver &lp_solver){
    return false;
}

bool LandmarkConstraints::update_constraints(const int horizon,
                                         lp::LPSolver &lp_solver,const std::shared_ptr<AbstractTask> task,
                                         std::vector<lp::LPVariable> &variables,
                                         double infinity, std::vector<lp::LPConstraint> & constraints){
    t_min = t_max;
    t_max = horizon;
    landmark_constraints(task, constraints, infinity,t_max);
    return false;
}

void LandmarkConstraints::landmark_constraints(const std::shared_ptr<AbstractTask> task,
                                                     std::vector<lp::LPConstraint> &constraints,
                                                     double infinity, int t_max){
    
    int id = 0;
    for (size_t op_id : action_landmarks){
        lp::LPConstraint constraint(1., infinity);
        for (int t = 0; t < t_max; ++t){
            constraint.insert((*index_opt)[op_id][t], 1.);
        }
        if (landmark_constraints_index[id] == -1){
            landmark_constraints_index[id]= constraints.size();
            constraints.push_back(constraint);
        }else{
            constraints[landmark_constraints_index[id]] = constraint;
        }
        ++id;
    }
}

static shared_ptr<IPConstraintGenerator> _parse(OptionParser &parser) {
    parser.document_synopsis(
                             "State based model",
                             "For each fact, a permanent constraint is added that considers the net "
                             "change of the fact, i.e., the total number of times the fact is added "
                             "minus the total number of times is removed. The bounds of each "
                             "constraint depend on the current state and the goal state and are "
                             "updated in each state. For details, see" + utils::format_paper_reference(
                                                                                                       {"Menkes van den Briel", "J. Benton", "Subbarao Kambhampati",
                                                                                                           "Thomas Vossen"},
                                                                                                       "An LP-based heuristic for optimal planning",
                                                                                                       "http://link.springer.com/chapter/10.1007/978-3-540-74970-7_46",
                                                                                                       "Proceedings of the Thirteenth International Conference on"
                                                                                                       " Principles and Practice of Constraint Programming (CP 2007)",
                                                                                                       "651-665",
                                                                                                       "2007") + utils::format_paper_reference(
                                                                                                                                               {"Blai Bonet"},
                                                                                                                                               "An admissible heuristic for SAS+ planning obtained from the"
                                                                                                                                               " state equation",
                                                                                                                                               "http://ijcai.org/papers13/Papers/IJCAI13-335.pdf",
                                                                                                                                               "Proceedings of the Twenty-Third International Joint"
                                                                                                                                               " Conference on Artificial Intelligence (IJCAI 2013)",
                                                                                                                                               "2268-2274",
                                                                                                                                               "2013") + utils::format_paper_reference(
                                                                                                                                                                                       {"Florian Pommerening", "Gabriele Roeger", "Malte Helmert",
                                                                                                                                                                                           "Blai Bonet"},
                                                                                                                                                                                       "LP-based Heuristics for Cost-optimal Planning",
                                                                                                                                                                                       "http://www.aaai.org/ocs/index.php/ICAPS/ICAPS14/paper/view/7892/8031",
                                                                                                                                                                                       "Proceedings of the Twenty-Fourth International Conference"
                                                                                                                                                                                       " on Automated Planning and Scheduling (ICAPS 2014)",
                                                                                                                                                                                       "226-234",
                                                                                                                                                                                       "AAAI Press 2014"));
    
    if (parser.dry_run())
        return nullptr;
    return make_shared<LandmarkConstraints>();
}

static PluginShared<IPConstraintGenerator> _plugin("landmark", _parse);

