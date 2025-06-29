#include "execution_detection.h"
#include "../plugin.h"
#include "../option_parser.h"
#include "../global_state.h"
#include "../globals.h"
#include "../task_proxy.h"
#include <algorithm>
#include <fstream>

using namespace std;
using namespace rd;

ExecuteDetection::ExecuteDetection(const Options &opts)
: SearchEngine(opts) {
    cout << "we are here" << endl;
    // get actions
    const std::shared_ptr<AbstractTask> task(g_root_task());
    TaskProxy task_proxy(*task);
    resource_detection = new ResourceDetection(task_proxy);
    //get_resource_actions(task_proxy);
    write_domain(task_proxy,true);
    write_domain(task_proxy,false);
    //write_sas(task_proxy,true);
    //write_sas(task_proxy,false);
}

ExecuteDetection::~ExecuteDetection(){
    
}

void ExecuteDetection::write_domain(TaskProxy & task_proxy, bool numeric_mode){
    // check if they are touching any resource variable:
    resource_detection->create_sets_actions(task_proxy, numeric_mode);
    set<int> variable_n = resource_detection->variables_unchanged();
    set<int> variable_r = resource_detection->variables_resource();
    set<int> normal = resource_detection->operators_unchanged();
    set<int> resourced = resource_detection->operator_resource();
    
    if(numeric_mode == true){
        for (int n : variable_n){
            cout << n << " " << task_proxy.get_variables()[n].get_name() << " propositional " << endl;
        }
        for (int n : variable_r){
            cout << n << " " << task_proxy.get_variables()[n].get_name() << " resource " << endl;
        }
    }
    
    ofstream outfile_domain;
    string name_domain = numeric_mode ? "domain_translated.pddl" : "domain_grounded.pddl";
    
    outfile_domain.open(name_domain, ios::out);
    
    outfile_domain << "(define (domain translated)"<<endl;
    outfile_domain << "  (:requirements" << endl;
    outfile_domain << "    :typing :numeric-fluents" <<endl;
    outfile_domain << "  )" << endl;
    outfile_domain << "  (:predicates " <<endl;
    for (int id_var : variable_n){
        VariableProxy var= task_proxy.get_variables()[id_var];
        for (int d = 0; d < var.get_domain_size(); ++d){
            outfile_domain << "    (" << var.get_name() << "_" << d << ")" << endl;
        }
    }
    outfile_domain << "  )" << endl;
    outfile_domain << "  (:functions " <<endl;
    for (int id_var : variable_r){
        VariableProxy var= task_proxy.get_variables()[id_var];
        outfile_domain << "    (" << var.get_name() << ")" << endl;
    }
    outfile_domain << "  )" << endl;
    
    for (int id_op : normal){
        map<int,int> net_effect;
        OperatorProxy op = task_proxy.get_operators()[id_op];
        string name = op.get_name();
        replace(name.begin(), name.end(), ' ', '_');
        outfile_domain << "  (:action " << name << endl;
        outfile_domain << "     :parameters()" << endl;
        outfile_domain << "     :precondition (and" << endl;
        for (FactProxy condition : op.get_preconditions()) {
            int id_var = condition.get_variable().get_id();
            VariableProxy var= task_proxy.get_variables()[id_var];
            if (variable_n.find(id_var)!=variable_n.end()){
                outfile_domain << "       (" << var.get_name() << "_" << condition.get_value() << ")" << endl;
            }else{
                outfile_domain << "       (= (" << var.get_name() << ") " <<  resource_detection->get_mu(id_var,condition.get_value()) << ")" << endl;
                net_effect[id_var] = resource_detection->get_mu(id_var,condition.get_value());
            }
        }
        outfile_domain << "     )" << endl;
        outfile_domain << "     :effect (and" << endl;
        for (FactProxy condition : op.get_preconditions()) {
            int id_var = condition.get_variable().get_id();
            VariableProxy var= task_proxy.get_variables()[id_var];
            if (variable_n.find(id_var)!=variable_n.end()){
                if (resource_detection->get_action_effects(id_op,id_var) != -1){
                    outfile_domain << "       (not (" << var.get_name() << "_" << condition.get_value() << "))" << endl;
                }
            }
        }
        for (EffectProxy eff : op.get_effects()) {
            int id_var = eff.get_fact().get_variable().get_id();
            VariableProxy var= task_proxy.get_variables()[id_var];
            if (variable_n.find(id_var)!=variable_n.end()){
                outfile_domain << "       (" << var.get_name() << "_" << eff.get_fact().get_value() << ")" << endl;
            }else{
                // assign variables?
                double increase = resource_detection->get_mu(id_var,eff.get_fact().get_value()) - net_effect[id_var];
                if (abs(increase) > 0.5 ){
                    if (increase > 0)
                        outfile_domain << "       (increase (" << var.get_name() << ") " <<  increase << ")" << endl;
                    else
                        outfile_domain << "       (decrease (" << var.get_name() << ") " <<  -increase << ")" << endl;
                    
                }
            }
        }
        outfile_domain << "       (increase (total-cost) " << op.get_cost() << ")" << endl;
        outfile_domain << "     )" << endl;
        outfile_domain << "   )" << endl;
    }
    
    // numeric actions
    for (int id_var : variable_r){
        list<set<int>> ea = resource_detection->get_equivalent_actions(task_proxy,id_var);
        // this is a numeric action, take first action of the group
        for (set<int> group : ea){
            if (group.empty()) continue;
            int id_op = *group.begin();
            OperatorProxy op = task_proxy.get_operators()[id_op];
            string name = op.get_name();
            replace(name.begin(), name.end(), ' ', '_');
            outfile_domain << "  (:action " << name << endl;
            outfile_domain << "     :parameters()" << endl;
            outfile_domain << "     :precondition (and" << endl;
            for (FactProxy condition : op.get_preconditions()) {
                int id_var = condition.get_variable().get_id();
                // this is a normal variable
                VariableProxy var= task_proxy.get_variables()[id_var];
                if (variable_n.find(id_var)!=variable_n.end()){
                    outfile_domain << "       (" << var.get_name() << "_" << condition.get_value() << ")" << endl;
                } else {
                    if (resource_detection->get_action_behaviour(id_op,id_var) == ActionTypeRD::CONSUMER) {
                        outfile_domain << "       (<= (" << var.get_name() << ") " << resource_detection->get_max(id_var) << ")" << endl;
                        outfile_domain << "       (>= (- (" << var.get_name() << ") " << -resource_detection->get_delta(id_op,id_var) << ") 0)" << endl;
                    } else if (resource_detection->get_action_behaviour(id_op,id_var) == ActionTypeRD::PRODUCER){
                        outfile_domain << "       (<= (+ (" << var.get_name() << ") " << resource_detection->get_delta(id_op,id_var) << ")" << resource_detection->get_max(id_var) << ")" << endl;
                        outfile_domain << "       (>= (" << var.get_name() << ") 0)" << endl;
                    } else {
                        assert(false);
                    }
                }
            }
            outfile_domain << "       )" << endl;
            outfile_domain << "     :effect (and" << endl;
            for (FactProxy condition : op.get_preconditions()) {
                int id_var = condition.get_variable().get_id();
                VariableProxy var= task_proxy.get_variables()[id_var];
                if (variable_n.find(id_var)!=variable_n.end()){
                    if (resource_detection->get_action_effects(id_op,id_var)!=-1){
                        outfile_domain << "       (not (" << var.get_name() << "_" << condition.get_value() << "))" << endl;
                    }
                }
            }
            for (EffectProxy eff : op.get_effects()) {
                int id_var = eff.get_fact().get_variable().get_id();
                VariableProxy var= task_proxy.get_variables()[id_var];
                
                if (variable_n.find(id_var)!=variable_n.end()){
                    outfile_domain << "       (" << var.get_name() << "_" << eff.get_fact().get_value() << ")" << endl;
                } else{
                    if (resource_detection->get_action_behaviour(id_op,id_var) == ActionTypeRD::CONSUMER) {
                    outfile_domain << "       (decrease ("<< var.get_name() << ") " << -resource_detection->get_delta(id_op,id_var) << ")" << endl;
                    }else if (resource_detection->get_action_behaviour(id_op,id_var) == ActionTypeRD::PRODUCER) {
                        outfile_domain << "       (increase ("<< var.get_name() << ") " << resource_detection->get_delta(id_op,id_var) << ")" << endl;
                    }
                }
            }
            outfile_domain << "       (increase (total-cost) " << op.get_cost() << ")" << endl;
            outfile_domain << "       )" << endl;
            outfile_domain << "     )" << endl;
        }
    }
    outfile_domain << "   )" << endl;
    outfile_domain.close();
    cout << name_domain << " written" << endl;

    ofstream outfile_problem;
    string name_problem = numeric_mode ? "problem_translated.pddl" : "problem_grounded.pddl";
    
    outfile_problem.open(name_problem, ios::out);

    // problem file
    outfile_problem << "(define (problem p)"<<endl;
    outfile_problem << "  (:domain translated)" << endl;
    outfile_problem << "  (:init " <<endl;
    State initial_state = task_proxy.get_initial_state();
    for (int id_var : variable_n){
        VariableProxy var= task_proxy.get_variables()[id_var];
        outfile_problem << "    (" << var.get_name() << "_" << initial_state[id_var].get_value() << ")" << endl;
    }
    for (int id_var : variable_r){
        VariableProxy var= task_proxy.get_variables()[id_var];
        outfile_problem << "    (= (" << var.get_name() << ") " << resource_detection->get_mu(id_var,initial_state[id_var].get_value()) << ")" << endl;
    }
    outfile_problem << "    (= (total-cost) 0)" << endl;

    outfile_problem << "  ) " <<endl;
    outfile_problem << "  (:goal (and " <<endl;
    for (FactProxy goal : task_proxy.get_goals()) {
        int id_var = goal.get_variable().get_id();
        VariableProxy var= task_proxy.get_variables()[id_var];
        outfile_problem << "    (" << var.get_name() << "_" << goal.get_value() << ")" << endl;
    }
    outfile_problem << "  ))" << endl;
    outfile_problem << "(:metric minimize (total-cost))" << endl;
    outfile_problem << ") " <<endl;
    outfile_problem.close();
    cout << name_problem << " written" << endl;
}

static SearchEngine *_parse(OptionParser &parser) {
    parser.document_synopsis("resource detection", "");
    SearchEngine::add_options_to_parser(parser);
    //lp::add_lp_solver_option_to_parser(parser);
    Options opts = parser.parse();

    if (parser.dry_run())
    return nullptr;
    else
    return new ExecuteDetection(opts);
}


static Plugin<SearchEngine> _plugin("resource_detection", _parse);
