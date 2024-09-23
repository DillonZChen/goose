#include "../../include/graph/ilg_generator.hpp"

#define X(description, name) name,
char const *fact_description_name[] = {ILG_FACT_DESCRIPTIONS};
#undef X

#define to_obj_node(x) x
#define to_atom_node(x) x.to_string()

namespace graph {
  ILGGenerator::ILGGenerator(const planning::Domain &domain, bool differentiate_constant_objects)
      : domain(domain),
        predicate_to_colour(domain.predicate_to_colour),
        differentiate_constant_objects(differentiate_constant_objects) {
    // initialise initial node colours
    if (differentiate_constant_objects) {
      // add constant object colours
      for (size_t i = 0; i < domain.constant_objects.size(); i++) {
        int colour = -(i + 1);
        colour_to_description[colour] = "_constant_object_ " + domain.constant_objects[i];
      }
    }

    colour_to_description[0] = "_object_";

    // add predicate colours
    for (size_t i = 0; i < domain.predicates.size(); i++) {
      for (int j = 0; j < (int)ILGFactDescription::_LAST; j++) {
        int colour = fact_colour(i, (ILGFactDescription)j);
        std::string desc = domain.predicates[i].name + " " + fact_description_name[j];
        colour_to_description[colour] = desc;
      }
    }
  }

  void ILGGenerator::set_problem(const planning::Problem &problem) {
    // reset graph and variables
    Graph graph = Graph(/*store_node_names=*/true);
    positive_goal_names = std::unordered_set<std::string>();
    negative_goal_names = std::unordered_set<std::string>();
    this->problem = std::make_shared<planning::Problem>(problem);

    /// add nodes
    int colour;

    // add constant object nodes
    for (size_t i = 0; i < problem.get_constant_objects().size(); i++) {
      std::string node = to_obj_node(domain.constant_objects[i]);
      if (differentiate_constant_objects) {
        colour = -(i + 1);
      } else {
        colour = 0;
      }
      graph.add_node(node, colour);
    }

    // objects
    for (const auto &object : problem.get_problem_objects()) {
      std::string node = to_obj_node(object);
      colour = 0;
      graph.add_node(node, colour);
    }

    // atoms
    for (const auto &atom : problem.get_positive_goals()) {
      std::string node = to_atom_node(atom);
      colour = fact_colour(atom, ILGFactDescription::F_POS_GOAL);
      graph.add_node(node, colour);
      positive_goal_names.insert(node);
    }

    for (const auto &atom : problem.get_negative_goals()) {
      std::string node = to_atom_node(atom);
      colour = fact_colour(atom, ILGFactDescription::F_NEG_GOAL);
      graph.add_node(node, colour);
      negative_goal_names.insert(node);
    }

    /// add labelled edges
    // add edges atoms <-> objects
    for (const auto &atom : problem.get_positive_goals()) {
      for (size_t r = 0; r < atom.objects.size(); r++) {
        std::string atom_node = to_atom_node(atom);
        std::string object_node = to_obj_node(atom.objects[r]);
        graph.add_edge(atom_node, r, object_node);
        graph.add_edge(object_node, r, atom_node);
      }
    }

    /// set pointer
    base_graph = std::make_shared<Graph>(graph);
    n_edges_added = std::vector<int>(base_graph->nodes.size(), 0);
  }

  std::shared_ptr<Graph> ILGGenerator::modify_graph_from_state(const planning::State &state,
                                                               const std::shared_ptr<Graph> graph,
                                                               bool store_changes) {
    if (store_changes) {
      n_nodes_added = 0;
      std::fill(n_edges_added.begin(), n_edges_added.end(), 0);
      pos_goal_changed = std::vector<int>();
      neg_goal_changed = std::vector<int>();
      pos_goal_changed_pred = std::vector<int>();
      neg_goal_changed_pred = std::vector<int>();
      graph->set_store_node_names(false);
    }

    int atom_node, object_node, pred_idx;
    std::string atom_node_str;

    for (const auto &atom : state) {
      atom_node_str = to_atom_node(atom);
      pred_idx = predicate_to_colour.at(atom.predicate->name);
      if (positive_goal_names.count(atom_node_str)) {
        atom_node = graph->get_node_index(atom_node_str);
        graph->change_node_colour(atom_node, fact_colour(pred_idx, ILGFactDescription::T_POS_GOAL));
        if (store_changes) {
          pos_goal_changed.push_back(atom_node);
          pos_goal_changed_pred.push_back(pred_idx);
        }
      } else if (negative_goal_names.count(atom_node_str)) {
        atom_node = graph->get_node_index(atom_node_str);
        graph->change_node_colour(atom_node, fact_colour(pred_idx, ILGFactDescription::T_NEG_GOAL));
        if (store_changes) {
          neg_goal_changed.push_back(atom_node);
          neg_goal_changed_pred.push_back(pred_idx);
        }
      } else {
        atom_node =
            graph->add_node(atom_node_str, fact_colour(pred_idx, ILGFactDescription::NON_GOAL));
        if (store_changes) {
          n_nodes_added++;
        }

        for (size_t r = 0; r < atom.objects.size(); r++) {
          // object nodes should never be needed to be added
          object_node = graph->get_node_index(to_obj_node(atom.objects[r]));
          graph->add_edge(atom_node, r, object_node);
          graph->add_edge(object_node, r, atom_node);
          if (store_changes) {
            n_edges_added[object_node]++;
          }
        }
      }
    }

    return graph;
  }

  void ILGGenerator::reset_graph() const {
    for (size_t i = 0; i < pos_goal_changed.size(); i++) {
      base_graph->change_node_colour(
          pos_goal_changed[i],
          fact_colour(pos_goal_changed_pred[i], ILGFactDescription::F_POS_GOAL));
    }

    for (const auto &node : neg_goal_changed) {
      base_graph->change_node_colour(
          node, fact_colour(neg_goal_changed_pred[node], ILGFactDescription::F_NEG_GOAL));
    }

    for (int i = 0; i < n_nodes_added; i++) {
      base_graph->nodes.pop_back();
    }

    for (int i = 0; i < n_nodes_added; i++) {
      base_graph->edges.pop_back();
    }

    for (size_t i = 0; i < base_graph->nodes.size(); i++) {
      for (int j = 0; j < n_edges_added[i]; j++) {
        base_graph->edges[i].pop_back();
      }
    }
  }

  std::shared_ptr<Graph> ILGGenerator::to_graph(const planning::State &state) {
    std::shared_ptr<Graph> graph = std::make_shared<Graph>(*base_graph);
    graph = modify_graph_from_state(state, graph, false);
    return graph;
  }

  std::shared_ptr<Graph> ILGGenerator::to_graph_opt(const planning::State &state) {
    base_graph = modify_graph_from_state(state, base_graph, true);
    return base_graph;
  }

  int ILGGenerator::get_n_edge_labels() const { return domain.max_arity(); }

  std::vector<int> ILGGenerator::get_init_colours() const {
    std::vector<int> init_colours;
    for (const auto &kv : colour_to_description) {
      init_colours.push_back(kv.first);
    }
    return init_colours;
  }

  void ILGGenerator::print_init_colours() const {
    std::cout << "Initial ILG node colours:" << std::endl;
    for (const auto &kv : colour_to_description) {
      std::cout << kv.first << " -> " << kv.second << std::endl;
    }
  }

  void ILGGenerator::dump_graph() const { base_graph->dump(); }
}  // namespace graph
