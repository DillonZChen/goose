#include "rmax_heuristic.h"
#include "interval.h"
#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

using namespace std;
using namespace interval_relaxation_heuristic;
using namespace numeric_helper;


namespace rmax_heuristic {
    
    
    static Heuristic *_parse(OptionParser &parser) {
        parser.document_synopsis("Interval Numeric Max heuristic", "");
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

        parser.add_option<bool>("restrict_achievers",
                                "restric achievers to actions whose preconditions are cheaper than"
                                "the numeric effect to achieve (Scala et al. JAIR 2020)",
                                "false");
        parser.add_option<bool>("ceiling_less_than_one", "use 1 instead of m_a when m_a < 1", "false");
        
        Heuristic::add_options_to_parser(parser);
        Options opts = parser.parse();
        
        if (parser.dry_run())
            return 0;
        else
            return new RMaxHeuristic(opts);
    }
    
    void RMaxHeuristic::initialize() {
        if(DEBUG) cout << "Initializing (old-fashioned) Interval relaxed 'max' heuristic..." << endl;
    }
    
    ap_float RMaxHeuristic::compute_heuristic(
                                              const GlobalState& global_state) {
        State state = convert_global_state(global_state);
        OperatorsProxy ops = task_proxy.get_operators();

        if (restrict_achievers) all_achievers.assign(numeric_task.get_n_conditions(),set<int>());
        
        // check if goals are satisfied
        cond_dist.assign(numeric_task.get_n_propositions(),max_float);
        cond_num_dist.assign(numeric_task.get_n_conditions(),max_float);
        action_dist.assign(ops.size(),max_float);
        //is_init_state.assign(numeric_task.get_n_conditions(),false);
        closed.assign(ops.size(),false);
        action_comp_number_execution.assign(ops.size(),vector<double>(numeric_task.get_n_conditions(),-1));

        HeapQueue<int> a_plus; // cannot use adaptive queue when costs are non integer
        set<int> reachable;
        
        //update initial state
        
        for (size_t var = 0; var < numeric_task.get_n_vars(); ++var) {
            int val = state[var].get_value();
            int condition = numeric_task.get_proposition(var,val);
            cond_dist[condition] = 0;
        }
        
        for (size_t var = 0; var < numeric_task.get_n_conditions(); ++var) {
            const LinearNumericCondition &num_values = numeric_task.get_condition(var);
            double lower_bound = - num_values.constant + numeric_task.get_epsilon(var);
            for (size_t i = 0; i < numeric_task.get_n_numeric_variables(); i++){
                int id_num = numeric_task.get_numeric_variable(i).id_abstract_task;
                lower_bound -= (state.nval(id_num) * num_values.coefficients[i]);
            }
            if (lower_bound <= 0){
                cond_num_dist[var] = 0;
                //is_init_state[var] = true;
            }
            //cout << num_values << " (" << var << ") has distance " << cond_num_dist[var] << " in the initial state" << endl;
        }

        for (size_t op_id = 0; op_id < ops.size(); ++op_id){
            // check if condition is satisfied
            const OperatorProxy &op = ops[op_id];
            bool applicable = true;
            for (FactProxy condition : op.get_preconditions()) {
                if (numeric_task.is_numeric_axiom(condition.get_variable().get_id())) continue;
                if (state[condition.get_variable().get_id()].get_value() != condition.get_value()){
                    applicable = false;
                }
            }
            // add there numeric actions
            for (int pre : numeric_task.get_action_num_list(op_id)){
                for (int c : numeric_task.get_numeric_conditions_id(pre)){
                    const LinearNumericCondition &lnc = numeric_task.get_condition(c);
                    double value = lnc.constant - numeric_task.get_epsilon(c);
                    for (size_t i = 0; i < numeric_task.get_n_numeric_variables(); i++){
                        int id_num = numeric_task.get_numeric_variable(i).id_abstract_task;
                        value += (state.nval(id_num) * lnc.coefficients[i]);
                    }
                    if (value < 0) applicable = false;
                }
            }
            
            if (applicable){
                //action is applicable
                a_plus.push(0,op_id);
                reachable.insert(op_id);
                action_dist[op_id] = 0;
                //cout << "adding applicable action " << op.get_name() << " to queue " << endl;
            } else {
                //cout << "action " << op.get_name() << " not applicable " << endl;
            }
            
        }
        
        // explore
        set<int> reachable_here;
        while(!a_plus.empty()){
            pair<ap_float, int> top_pair = a_plus.pop();
            ap_float distance = top_pair.first;
            int current_op_id = top_pair.second;
            reachable_here.insert(current_op_id);
            closed[current_op_id] = true;
            //cout << "\tconsidering " << task_proxy.get_operators()[current_op_id].get_name() << " " << distance << endl;
//            for (int i = 0; i < numeric_task.get_n_propositions(); i++){
//                //cout << "\t"<< i << " "<<cond_dist[i] << endl;
//            }
//            for (int i = 0; i < numeric_task.get_n_conditions(); i++){
//                cout << "\t"<< numeric_task.get_condition(i) << " "<<cond_num_dist[i] << endl;
//            }
//            double estimate = check_goal();
//            if (estimate < max_float) {
////                for (size_t i = 0; i < numeric_task.get_n_numeric_variables(); i++){
////                    int id_num = numeric_task.get_numeric_variable(i).id_abstract_task;
////                    cout << state.nval(id_num) << " ";
////                }
//                cout << " => " << estimate << endl;
//                //exit(1);
//                return estimate;
//            }
            update_reachable_conditions_actions(state,current_op_id, a_plus);
        }
//        for (size_t i = 0; i < numeric_task.get_n_numeric_variables(); i++){
//            int id_num = numeric_task.get_numeric_variable(i).id_abstract_task;
//            cout << state.nval(id_num) << " ";
//        }
//        cout << " => " << check_goal() << endl;
//        exit(1);
        return check_goal();
    }
    
    // TODO improve this
    double RMaxHeuristic::check_goal(){
        // ADD GOAL CONDITION
        bool satisfied_goals = true;
        double estimate = 0;
        for (size_t id_goal = 0; id_goal < task_proxy.get_goals().size(); ++id_goal) {
            
            FactProxy goal = task_proxy.get_goals()[id_goal];
            int var = goal.get_variable().get_id();
            int val = goal.get_value();
            int c = numeric_task.get_proposition(var,val);
            if (!numeric_task.numeric_goals_empty(id_goal)) continue; // this is a numeric goal
            if (cond_dist[c] == max_float){
                satisfied_goals = false;
                //cout << "\t\t" << var << " " << val << " not satisfied " << endl;
                return max_float;
            }else{
                //cout << var << " " << val << "satisfied!!!" << endl;
                if (estimate < cond_dist[c]){
                    //cout << "\t\t\testimation is now better " <<  cond_dist[c] << " " << estimate << endl;
                    estimate = cond_dist[c];
                }
            }
        }
        //satisfied_goals = true;
        //size_t n_propositions = numeric_task.get_n_propositions();
        for (size_t id_goal = 0; id_goal < numeric_task.get_n_numeric_goals(); ++id_goal) {
            if (!satisfied_goals) continue;
            list<int> goals = numeric_task.get_numeric_goals(id_goal);
            for (int id_n_con : goals){
                const LinearNumericCondition& lnc = numeric_task.get_condition(id_n_con);
                //cout << "\t\tchecking goal " << lnc << " (" << id_n_con << ") " << cond_num_dist[id_n_con] << endl;
//                /int c = id_n_con + n_propositions;
                if (cond_num_dist[id_n_con] == max_float){
                    satisfied_goals = false;
                    return max_float;
                }else{
                    //cout << "\t\t\testimation is now better " <<  cond_num_dist[id_n_con] << " " << estimate << endl;
                    if (estimate < cond_num_dist[id_n_con])
                        estimate = cond_num_dist[id_n_con];
                }
            }
        }
        if(satisfied_goals) return estimate;
        return max_float ;
    }
    
    void RMaxHeuristic::update_reachable_conditions_actions(const State &s_0, int gr_id, HeapQueue<int>& a_plus){
        // preconditions
        OperatorProxy gr = task_proxy.get_operators()[gr_id];
        double c_a = gr.get_cost();
        //cout << task_proxy.get_operators()[gr_id].get_name() << endl;
        for (EffectProxy effect_proxy : gr.get_effects()) {
            FactProxy effect = effect_proxy.get_fact();
            int var = effect.get_variable().get_id();
            int post = effect.get_value();
            int condition = numeric_task.get_proposition(var,post);
            //cout << "\tpossible achiever " << var << " " << post << " " << condition << " : " << cond_dist[condition] << " " << c_a << " " << action_dist[gr_id] << endl;
            if (cond_dist[condition] > 0) {
                double cond_dist_comp = c_a + action_dist[gr_id];
                if (cond_dist_comp < cond_dist[condition]) {
                    cond_dist[condition] = cond_dist_comp;
                }
                update_reachable_actions(gr_id, condition, a_plus);
            }
        }
        
        for (int nc_id : possible_achievers[gr_id]){
            const LinearNumericCondition &lnc = numeric_task.get_condition(nc_id);
            double current_distance = cond_num_dist[nc_id];
            //cout << "\tpossible achiever " << lnc << ", " << current_distance << endl;
            if (current_distance > 0){
                double rep_needed = action_comp_number_execution[gr_id][nc_id];

                if (rep_needed == -1){ // assume null
                    rep_needed = get_number_of_execution(gr_id, s_0, nc_id);
                    action_comp_number_execution[gr_id][nc_id] = rep_needed;
                    //cout << "\t\trepetition " << rep_needed << endl;
                }
                
                if (rep_needed >= 0) {
                    if (restrict_achievers) all_achievers[nc_id].insert(gr_id);
                    // add new distance
                    double new_distance = rep_needed * c_a + min_over_possible_achievers(nc_id);//action_dist[gr_id];
                    if (new_distance < current_distance) {
                        cond_num_dist[nc_id] = new_distance;
                        //cout << "\t\t\tnew distance " << new_distance << endl;
                    }
                    update_reachable_actions(gr_id, nc_id+numeric_task.get_n_propositions(), a_plus);
                }
            }
        }
    }
    
    double RMaxHeuristic::min_over_possible_achievers(int nc_id){
        set<int> & achievers = restrict_achievers ? all_achievers[nc_id] : possible_achievers_inverted[nc_id];
        double min_cost = max_float;
        for (int op : achievers){
            min_cost = min(min_cost,action_dist[op]);
            if (min_cost == 0) return 0;
        }
        return min_cost;
    }

    
    void RMaxHeuristic::update_reachable_actions(int gr_id, int cond, HeapQueue<int>& a_plus){
        set<int> &set = condition_to_action[cond];
        //cout << "set size " << set.size() << " " << cond << endl;
        for (int gr2 : set) {
            //cout << "checking preconditions of " << task_proxy.get_operators()[gr2].get_name() << endl;
            double c = check_conditions(gr2);
            //cout << "\t\tprecondition costs " << c << endl;
            if (c < action_dist[gr2] && c != -max_float) {
                action_dist[gr2]  = c;
                if (!closed[gr2]){
                    a_plus.push(c,gr2);
                    closed[gr2] = false;
                    //cout <<"\t\t\tadding " << task_proxy.get_operators()[gr2].get_name() << " " << c << endl;
                }else{
                    // TODO check this
                    //a_plus.push(c,gr2);
                    //cout <<"\t\t\tnot adding " << task_proxy.get_operators()[gr2].get_name() << endl;
                }
            }
        }
    }
    
    // todo implement this
    double RMaxHeuristic::check_conditions(int gr_id){
        // get preconditions and return conditions and return max
        double estimate = -max_float;
        for (int c : numeric_task.get_action_pre_list(gr_id)) {
            if (cond_dist[c] > estimate) estimate = cond_dist[c];
        }
        for (int i : numeric_task.get_action_num_list(gr_id)) {
            for (int c : numeric_task.get_numeric_conditions_id(i)){
                //cout << "\t\t\t" << numeric_task.get_condition(c) <<" " << cond_num_dist[c] << endl;
                if (cond_num_dist[c] > estimate) estimate = cond_num_dist[c];
            }
        }
        return estimate;
    }
    
    double RMaxHeuristic::get_number_of_execution(int gr, const State &state, int n_condition){
        const LinearNumericCondition &num_values = numeric_task.get_condition(n_condition);
        double lower_bound =   -num_values.constant + numeric_task.get_epsilon(n_condition);
        for (size_t i = 0; i < numeric_task.get_n_numeric_variables(); i++){
            int id_num = numeric_task.get_numeric_variable(i).id_abstract_task;
            lower_bound -= state.nval(id_num) * num_values.coefficients[i];
        }
        if (net_effects[gr][n_condition] == 0) return -2;
        double rep = lower_bound/net_effects[gr][n_condition];
        if (rep < 0) return 0; // already satisfied
        if (ceiling_less_than_one && rep < 1) return 1;
        return rep;
    }
    

    void RMaxHeuristic::generate_preconditions(){
        condition_to_action.assign(numeric_task.get_n_propositions() + numeric_task.get_n_conditions(), set<int>());
        size_t n_propositions = numeric_task.get_n_propositions();
        for (size_t op_id = 0; op_id < task_proxy.get_operators().size(); ++op_id){
            for (int c : numeric_task.get_action_pre_list(op_id)) {
                condition_to_action[c].insert(op_id);
                //cout << "********* adding " << task_proxy.get_operators()[op_id].get_name() << " to " << c << condition_to_action[c].size() << endl;
            }
            for (int i : numeric_task.get_action_num_list(op_id)) {
                for (int c : numeric_task.get_numeric_conditions_id(i)){
                    condition_to_action[c+n_propositions].insert(op_id);
                    //cout << "********* adding " << task_proxy.get_operators()[op_id].get_name() << " to " << c+n_propositions << " " <<  condition_to_action[c+n_propositions].size() << endl;
                    //cout << "condition: " << numeric_task.get_condition(c) << endl;
                }
            }
        }
    }
    
    void RMaxHeuristic::generate_possible_achievers(){
        possible_achievers.assign(task_proxy.get_operators().size(),set<int>());
        possible_achievers_inverted.assign(numeric_task.get_n_conditions(),set<int>());
        net_effects.assign(task_proxy.get_operators().size(),vector<double>(numeric_task.get_n_conditions(),0.));
        size_t n_numeric_variables = numeric_task.get_n_numeric_variables();
        for (size_t nc_id = 0; nc_id < numeric_task.get_n_conditions(); ++nc_id){
            for (size_t op_id = 0; op_id < task_proxy.get_operators().size(); ++op_id){
                const LinearNumericCondition &nc = numeric_task.get_condition(nc_id);
                double cumulative_effect = 0;//nc.constant;
                for (size_t v = 0; v < n_numeric_variables; ++v){
                    cumulative_effect+=(nc.coefficients[v]*numeric_task.get_action_eff_list(op_id)[v]);
                }
                net_effects[op_id][nc_id] = cumulative_effect;
                
                if (cumulative_effect>0){
                    //cout << task.get_operators()[op_id].get_name() << " effect on " << numeric_task.get_condition(nc_id) << " is " << cumulative_effect << endl;
                    //double cumulative_effect = 0;
                    //                for (size_t v = 0; v < n_numeric_variables; ++v){                                        cout << "\t" << v << " " << nc.coefficients[v]<< " " << numeric_task.get_action_eff_list(op_id)[v] << endl;
                    //                }
                    possible_achievers[op_id].insert(nc_id);
                    possible_achievers_inverted[nc_id].insert(op_id);
                }
            }
        }
        //    ComparisonAxiomsProxy axioms = task.get_comparison_axioms();
        //    for (size_t num_id = 0; num_id < axioms.size(); ++num_id){
        //        ComparisonAxiomProxy ax = axioms[num_id];
        //        cout << ax.get_true_fact().get_name() << " " << endl;
        //        for (int ax_id : numeric_task.get_numeric_conditions_id(num_id))
        //            cout << "\t"<< ax_id << " "<< numeric_task.get_condition(ax_id) << endl;
        //    }
    }
    
    RMaxHeuristic::RMaxHeuristic(const options::Options& options)
    : IntervalRelaxationHeuristic(options)
    {
        restrict_achievers = options.get<bool>("restrict_achievers");
        ceiling_less_than_one = options.get<bool>("ceiling_less_than_one");
        numeric_task = NumericTaskProxy(task_proxy);
        max_float = 999999;
        generate_possible_achievers();
        generate_preconditions();

    }
    
    RMaxHeuristic::~RMaxHeuristic() {
    }
    
    static Plugin<Heuristic> _plugin("hrmax", _parse);
    
}

