#ifndef SEARCH_ENGINES_PREF_SCHEMA_SEARCH_H
#define SEARCH_ENGINES_PREF_SCHEMA_SEARCH_H

#include "../ngoose/wlf_heuristic.h"
#include "../search_engine.h"

#include "../open_lists/open_list.h"

#include <memory>
#include <vector>

/// there's a lot of shared code here with wlf_mq_eager_search

class GlobalOperator;
class Heuristic;
class PruningMethod;
class ScalarEvaluator;

namespace options {
class Options;
}

template <class T> class GooseOpenList {
  // copied from standard_scalar_open_list.cc
  typedef std::deque<T> Bucket;

  std::map<int, Bucket> buckets;

  int size_;

 public:
  GooseOpenList() { size_ = 0; };

  T remove_min() {
    assert(size > 0);
    auto it = buckets.begin();
    assert(it != buckets.end());
    Bucket &bucket = it->second;
    assert(!bucket.empty());
    T result = bucket.front();
    bucket.pop_front();
    if (bucket.empty())
      buckets.erase(it);
    --size_;
    return result;
  }

  void insert(ap_float key, const T &entry) {
    if (!buckets.count(key)) {
      buckets[key] = std::deque<T>();
    }
    buckets[key].push_back(entry);
    ++size_;
  }

  bool empty() { return size_ == 0; }

  bool size() { return size_; }
};

namespace pref_schema_search {
class PrefSchemaSearch : public SearchEngine {
  int n_schemata_;
  // checking ints should be faster than checking closed nodes
  std::unordered_set<StateID> popped;
  std::vector<GooseOpenList<StateID> *> open_lists;
  int cur_queue;

  std::unordered_map<std::string, int> name_to_schema_index;

  wlf_heuristic::WlfHeuristic *heuristic;

  ap_float best_h;

  std::pair<SearchNode, bool> fetch_next_node();
  void print_checkpoint_line(int g) const;

 protected:
  virtual void initialize() override;
  virtual SearchStatus step() override;
  virtual void save_plan_if_necessary() const override;

 public:
  explicit PrefSchemaSearch(const options::Options &opts);
  virtual ~PrefSchemaSearch() = default;

  virtual void print_statistics() const override;

  void dump_search_space() const;
};
}  // namespace pref_schema_search

#endif
