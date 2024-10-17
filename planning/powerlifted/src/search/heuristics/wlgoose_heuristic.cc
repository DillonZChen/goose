#include "wlgoose_heuristic.h"
#include "../task.h"

#include <cassert>

using namespace std;

WlGooseHeuristic::WlGooseHeuristic(const Options &opts, const Task &task)
{
    model = load_feature_generator(opts.get_goose_model_path());

    const planning::Domain domain = *(model->get_domain());
    std::unordered_map<std::string, planning::Predicate> name_to_predicate;
    for (const auto &pred : domain.predicates) {
        name_to_predicate[pred.name] = pred;
    }

    /* Construct a WLPlan Problem from Powerlifted */

    // Preprocess predicates from PWL
    for (size_t i = 0; i < task.predicates.size(); i++) {
        std::string pred_name = task.predicates[i].get_name();
        // predicates that may get skipped are '=' and static predicates
        if (name_to_predicate.find(pred_name) == name_to_predicate.end()) {
            continue;
        }
        pwl_index_to_predicate[i] = name_to_predicate.at(pred_name);
    }

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
    model->set_problem(problem);
}


int WlGooseHeuristic::compute_heuristic(const DBState &s, const Task &task)
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
        unordered_set<GroundAtom, TupleHash> tuples = predicate_indices[i].tuples;
        for (const auto &tuple : tuples) {
            std::vector<std::string> object_names;
            for (const auto &obj : tuple) {
                object_names.push_back(task.get_object_name(obj));
            }
            atoms.push_back({predicate, object_names});
        }
    }

    double h = model->predict(planning::State(atoms));
    int h_round = static_cast<int>(std::round(h));

    return h_round;
}

void WlGooseHeuristic::print_statistics()
{
    // TODO
}
