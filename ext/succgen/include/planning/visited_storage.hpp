#ifndef PLANNING_VISITED_STORAGE_HPP
#define PLANNING_VISITED_STORAGE_HPP

#include "node.hpp"
#include "state.hpp"
#include "state_storer.hpp"

#include <set>
#include <vector>

namespace planning {
  class VisitedStorage {
    StateStorer states_;
    std::unordered_map<int, std::shared_ptr<Node>> i_to_node_;

   public:
    VisitedStorage() {
      states_ = StateStorer();
      i_to_node_ = std::unordered_map<int, std::shared_ptr<Node>>();
    };

    void add(const Node &node) {
      states_.add(node.state);
      i_to_node_[node.s_id] = std::make_shared<Node>(node);
    }

    Node get(int s_id) const {
      auto it = i_to_node_.find(s_id);
      if (it != i_to_node_.end()) {
        return *(it->second);
      }
      throw std::out_of_range("Node not found");
    }

    bool contains(const SGState &state) { return states_.contains(state); }
  };

}  // namespace planning

#endif  // PLANNING_VISITED_STORAGE_HPP
