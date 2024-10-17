#include "h_lm_numeric.h"
#include "../option_parser.h"
#include "../plugin.h"

#include "../utils/markup.h"
#include "../utils/memory.h"
#include "../lp/lp_solver.h"

#include "../numeric_landmarks/landmark_factory_scala.h"

using namespace std;
using namespace numeric_helper;
using namespace operator_counting;

void LMNumericConstriants::initialize_constraints(
                                              const shared_ptr<AbstractTask> task, vector<lp::LPConstraint> & /*constraints*/,
                                              double /*infinity*/) {
    TaskProxy task_proxy(*task);
    numeric_task = NumericTaskProxy(task_proxy, false, false);
    factory = utils::make_unique_ptr<landmarks::LandmarkFactoryScala>(task);

}


bool LMNumericConstriants::update_constraints(const State &state,
                                          lp::LPSolver &lp_solver) {
    constraints.clear();
    double infinity = lp_solver.get_infinity();
//    for (size_t n_id = 0; n_id < numeric_task.get_n_numeric_variables(); ++n_id){
//
//    }
    

    set<int> & fact_landmarks = factory->compute_landmarks(state);
    //cout << "landmakrs " << fact_landmarks.size() << endl;
    vector<set<int>> & landmarks_table = factory->get_landmarks_table();
    build_first_achiever(landmarks_table);
    
    for (size_t i : fact_landmarks){
        if (i < numeric_task.get_n_propositions()){
           // if
            //cout << numeric_task.get_proposition_name(i) << " is a fact landmark" << endl;
            // if not a int var
            if(!numeric_task.is_numeric_axiom(numeric_task.get_var(i))){
                pair<int,int> var_val = numeric_task.get_var_val(i);
                if (state[var_val.first].get_value() == var_val.second) continue;
                //cout << var_val.first << " " << var_val.second << endl;
                lp::LPConstraint constraint(1, infinity);
                for (int op_id : first_achievers[i]){
                    constraint.insert(op_id, 1);
                }
                if (!constraint.empty()) {
                   constraints.push_back(constraint);
                }
            }
        } else {
            //cout << numeric_task.get_condition(i - numeric_task.get_n_propositions()) << " is a condition landmark" << endl;
            int id_n_con = i - numeric_task.get_n_propositions();
            const LinearNumericCondition& lnc = numeric_task.get_condition(id_n_con);
            double lower_bound = -lnc.constant + numeric_task.get_epsilon(id_n_con);
            for (size_t i = 0; i < numeric_task.get_n_numeric_variables(); i++){
                int id_num = numeric_task.get_numeric_variable(i).id_abstract_task;
                lower_bound -= (state.nval(id_num) * lnc.coefficients[i]);
            }
            lp::LPConstraint constraint(lower_bound, infinity);
            for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
                double coeff = 0;
                for (size_t n_id = 0; n_id < numeric_task.get_n_numeric_variables(); ++n_id){
                    //cout << op_id << " " << lnc.coefficients[n_id]*numeric_task.get_action_eff_list(op_id)[n_id] << endl;
                    coeff += lnc.coefficients[n_id]*numeric_task.get_action_eff_list(op_id)[n_id];
                }
                if (coeff > 0)
                    constraint.insert(op_id, coeff);

            }
            if (!constraint.empty()) {
                constraints.push_back(constraint);
            }
        }
    }
    
    lp_solver.add_temporary_constraints(constraints);
    return false;
}

void LMNumericConstriants::build_first_achiever(vector<set<int>> &landmarks_table){
    fadd.assign(numeric_task.get_n_actions(),vector<bool>(numeric_task.get_n_propositions()+numeric_task.get_n_conditions(),false));
    action_landmarks.assign(numeric_task.get_n_actions(),vector<bool>(numeric_task.get_n_propositions()+numeric_task.get_n_conditions(),false));
    
    if (first_achievers.size() < numeric_task.get_n_propositions()+numeric_task.get_n_conditions())
        first_achievers.assign(numeric_task.get_n_propositions()+numeric_task.get_n_conditions(),set<int>());
    else{
        for(int i = 0; i<numeric_task.get_n_propositions()+numeric_task.get_n_conditions(); ++i)
            first_achievers[i].clear();
    }
//    first_achievers.assign(numeric_task.get_n_propositions()+numeric_task.get_n_conditions(),set<int>());
    
    // find action landmarks
    int n_propositions = numeric_task.get_n_propositions();
    for (size_t op_id = 0; op_id < numeric_task.get_n_actions(); ++op_id){
        
        const set<int> & pre_list = numeric_task.get_action_pre_list(op_id);
        for (int p : pre_list){
            set<int> & landmarks = landmarks_table[p];
            for (int l : landmarks) action_landmarks[op_id][l] = true;
        }
        
        const set<int> & num_list = numeric_task.get_action_num_list(op_id);
        for (int pre : num_list){
            for (int c : numeric_task.get_numeric_conditions_id(pre)){
                set<int> & landmarks = landmarks_table[c + n_propositions];
                for (int l : landmarks) action_landmarks[op_id][l] = true;
            }
        }
        
        // now find the first achievers
        const set<int> & add_list = numeric_task.get_action_add_list(op_id);
        for (int p : add_list){
            if (!action_landmarks[op_id][p]) {
                fadd[op_id][p] = true;
                first_achievers[p].insert(op_id);
            }
        }
        
        const set<int> & possible_add_list = numeric_task.get_action_possible_add_list(op_id);
        for (int c : possible_add_list) {
            if (!action_landmarks[op_id][c]) {
                fadd[op_id][c] = true;
                first_achievers[c].insert(op_id);
            }
        }
    }
}

static shared_ptr<ConstraintGenerator> _parse(OptionParser &parser) {
    parser.document_synopsis(
                             "numeric landmark heuristic",
                             "For details, see" + utils::format_paper_reference(
                                                                                {"Enrico Scala", "Patrik Haslum", "Daniele Magazzeni",
                                                                                    "Sylvie Thie ́baux"},
                                                                                "Landmarks for Numeric Planning Problems",
                                                                                "https://www.ijcai.org/proceedings/2017/0612.pdf",
                                                                                "Proceedings of the Twenty-Sixth International Joint Conference on Artificial Intelligence"
                                                                                " (IJCAI 2017))",
                                                                                "4384-4390",
                                                                                "AAAI Press 2017"));
    
    parser.document_language_support("action costs", "supported");
    parser.document_property("admissible", "yes");
    parser.document_property("safe", "yes");
    // TODO: prefer operators that are non-zero in the solution.
    parser.document_property("preferred operators", "no");
    
    
    Heuristic::add_options_to_parser(parser);
    Options opts = parser.parse();
    if (parser.help_mode())
        return nullptr;
    if (parser.dry_run())
        return nullptr;
    return make_shared<LMNumericConstriants>();
}

static PluginShared<ConstraintGenerator> _plugin("lm_numeric", _parse);


