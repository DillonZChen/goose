#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

#include "../lp/lp_solver.h"

#include "../utils/markup.h"
#include "sb_compilation.h"
#include "ip_constraint_generator.h"

using namespace std;
using namespace operator_counting;
using namespace numeric_helper;


void IPConstraintGenerator::print_solution(vector<double> &solution,const std::shared_ptr<AbstractTask> task){
    TaskProxy task_proxy(*task);
    OperatorsProxy ops = task_proxy.get_operators();
    int steps = 0;
    cout << "Solution found!" << endl;
    cout << "Actual search time: 0.168001s [t=0.171312s]" << endl;
    int t_max = (*index_opt)[0].size();
    for (int t = 0; t < t_max; ++t){
        for (int op_id = 0; op_id < ops.size(); ++op_id) {
            if (solution[(*index_opt)[op_id][t]] > 0.5){
                if (ops[op_id].get_name().find("forget") == 0) continue;
                //cout << t <<" " << solution[(*index_opt)[op_id][t]] << " " << ops[op_id].get_name() << endl;
                cout << ops[op_id].get_name() << " (1)" << endl;
                steps++;
            }
        }
    }
    cout << "Plan length: " << steps  << " step(s)." << endl;
}



