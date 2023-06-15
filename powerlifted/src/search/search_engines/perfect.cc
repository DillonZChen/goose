#include "perfect.h"
#include "search.h"
#include "utils.h"

#include "../open_lists/greedy_open_list.h"
#include "../plan_manager.h"
#include "../states/extensional_states.h"
#include "../states/sparse_states.h"
#include "../successor_generators/successor_generator.h"
#include "../utils/timer.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

void print_to_file(const DBState &s, const Task &task, std::ofstream& plan_file) {
    const auto& nullary_atoms = s.get_nullary_atoms();
    for (size_t j = 0; j < nullary_atoms.size(); ++j) {
        if (nullary_atoms[j])
            plan_file << task.predicates[j].get_name() << " ";
    }
    const auto& relations = s.get_relations();
    for (size_t i = 0; i < relations.size(); ++i) {
        string relation_name = task.predicates[i].get_name();
        unordered_set<GroundAtom, TupleHash> tuples = relations[i].tuples;
        for (auto &tuple : tuples) {
            plan_file << relation_name << "(";
            for (auto obj : tuple) {
                plan_file << task.objects[obj].get_name() << ",";
            }
            plan_file << ") ";
        }
    }
    plan_file << endl;
}

template <class PackedStateT>
utils::ExitCode PerfectSearch<PackedStateT>::search(const Task &task,
                                                    SuccessorGenerator &generator,
                                                    Heuristic &heuristic)
{
    cout << "Starting perfect search" << endl;
    StatePackerT packer(task);

    auto s = task.get_initial_state();
    std::ofstream plan_file(PlanManager::get_plan_filename());

    print_to_file(s, task, plan_file);

    string line;
    ifstream file(std::getenv("PLAN_PATH"));
    if (file.is_open()) {
        while ( getline (file,line) ) {
            if (line[0] == ';') {  // finished parsing
                break;
            }


            line = line.substr(1, line.size()-2);

            // Vector of string to save tokens
            std::vector<string> toks;
            stringstream check1(line);
            string intermediate;
            // Tokenizing w.r.t. space ' '
            while(getline(check1, intermediate, ' ')) {
                toks.push_back(intermediate);
            }

            ActionSchema as = task.get_action_schemas()[0];
            bool good = false;
            for (const auto& action:task.get_action_schemas()) {
                if (toks[0] == action.get_name()) {
                    as = action;
                    good = true;
                    break;
                }
            }
            if (!good) {
                std::cout << "invalid plan because cannot find action schema " << toks[0] << std::endl;
                return utils::ExitCode::SEARCH_UNSUPPORTED;
            }

            std::vector<int> instantiation(toks.size()-1);

            for (size_t i = 0; i < toks.size() - 1; i++) {
                // can probably make a dictionary
                good = false;
                for (size_t j = 0; j < task.objects.size(); j++) {
                    if (toks[i+1] == task.get_object_name(j)) {
                        instantiation[i] = j;
                        good = true;
                        break;
                    }
                }
                if (!good) {
                    std::cout << "invalid plan because cannot find object" << std::endl;
                    return utils::ExitCode::SEARCH_UNSUPPORTED;
                }
            }

            LiftedOperatorId* op_id = new LiftedOperatorId(as.get_index(), std::move(instantiation));
            s = generator.generate_successor(*op_id, as, s);
            print_to_file(s, task, plan_file);
        }
        file.close();
        if (!task.is_goal(s)) {
            std::cout << "invalid plan because final state is not a goal state" << std::endl;
            return utils::ExitCode::SEARCH_UNSUPPORTED;
        }
    } else {
        cout << "Unable to open file";
        return utils::ExitCode::SEARCH_UNSUPPORTED;
    }
    plan_file << "; GOOD";
    return utils::ExitCode::SUCCESS;
}



template <class PackedStateT>
void PerfectSearch<PackedStateT>::print_statistics() const {
}

// explicit template instantiations
template class PerfectSearch<SparsePackedState>;
template class PerfectSearch<ExtensionalPackedState>;