#include "wl_utils.h"

#include <unordered_set>
#include <vector>

namespace wl_utils {

planning::Domain get_wlplan_domain(const Task &task)
{
    std::vector<planning::Predicate> predicates;
    for (size_t i = 0; i < task.predicates.size(); i++) {
        std::string pred_name = task.predicates[i].get_name();
        // predicates that may get skipped are '=' and static predicates
        if (pred_name == "=") {
            continue;
        }
        predicates.push_back(planning::Predicate(pred_name, task.predicates[i].getArity()));
    }
    return planning::Domain("domain", predicates);
}

planning::Problem get_wlplan_problem(const planning::Domain &domain, const Task &task)
{   
    std::unordered_map<int, planning::Predicate> pwl_index_to_predicate =
        get_pwl_index_to_predicate(domain, task);

    // Collect objects
    std::vector<std::string> objects;
    for (const auto &obj : task.objects) {
        objects.push_back(obj.get_name());
    }

    // Deal with goals
    std::vector<planning::Atom> positive_goals;
    std::vector<planning::Atom> negative_goals;

    for (const auto &goal : task.get_goal().positive_nullary_goals) {
        planning::Predicate predicate = pwl_index_to_predicate.at(goal);
        planning::Atom atom = planning::Atom(predicate, {});
        positive_goals.push_back(atom);
    }

    for (const auto &goal : task.get_goal().negative_nullary_goals) {
        planning::Predicate predicate = pwl_index_to_predicate.at(goal);
        planning::Atom atom = planning::Atom(predicate, {});
        negative_goals.push_back(atom);
    }

    for (const auto &goal : task.get_goal().goal) {
        planning::Predicate predicate = pwl_index_to_predicate.at(goal.get_predicate_index());
        std::vector<planning::Object> objects;
        for (const auto arg : goal.get_arguments()) {
            objects.push_back(planning::Object(task.get_object_name(arg)));
        }
        planning::Atom atom = planning::Atom(predicate, objects);
        if (goal.is_negated()) {
            negative_goals.push_back(atom);
        }
        else {
            positive_goals.push_back(atom);
        }
    }

    // Construct WLPlan problem and set for model
    planning::Problem problem = planning::Problem(domain, objects, positive_goals, negative_goals);

    return problem;
}

std::unordered_map<int, planning::Predicate>
get_pwl_index_to_predicate(const planning::Domain &domain, const Task &task)
{
    std::unordered_map<int, planning::Predicate> pwl_index_to_predicate;

    std::unordered_map<std::string, planning::Predicate> name_to_predicate;
    for (const auto &pred : domain.predicates) {
        name_to_predicate[pred.name] = pred;
    }

    for (size_t i = 0; i < task.predicates.size(); i++) {
        std::string pred_name = task.predicates[i].get_name();
        if (name_to_predicate.find(pred_name) == name_to_predicate.end()) {
            continue;
        }
        pwl_index_to_predicate[i] = name_to_predicate.at(pred_name);
    }

    return pwl_index_to_predicate;
}

planning::State
to_wlplan_state(const DBState &s,
                const Task &task,
                const std::unordered_map<int, planning::Predicate> &pwl_index_to_predicate)
{

    std::vector<planning::Atom> atoms;  // list of wlplan atoms

    const auto &nullary_atoms = s.get_nullary_atoms();
    for (size_t j = 0; j < nullary_atoms.size(); ++j) {
        if (nullary_atoms[j]) {
            atoms.push_back({pwl_index_to_predicate.at(j), {}});
        }
    }
    const auto &predicate_indices = s.get_relations();
    for (const auto &kv : pwl_index_to_predicate) {
        int i = kv.first;
        planning::Predicate predicate = kv.second;
        std::unordered_set<GroundAtom, TupleHash> tuples = predicate_indices[i].tuples;
        for (const auto &tuple : tuples) {
            std::vector<std::string> object_names;
            for (const auto &obj : tuple) {
                object_names.push_back(task.get_object_name(obj));
            }
            atoms.push_back({predicate, object_names});
        }
    }

    return planning::State(atoms);
}

}  // namespace wl_utils
