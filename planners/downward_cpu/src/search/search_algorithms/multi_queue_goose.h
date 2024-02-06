#ifndef SEARCH_ALGORITHMS_MULTI_QUEUE_GOOSE_H
#define SEARCH_ALGORITHMS_MULTI_QUEUE_GOOSE_H

#include <deque>
#include <map>
#include <memory>
#include <vector>

#include "../learned_heuristics/goose_linear.h"
#include "../open_list.h"
#include "../search_algorithm.h"

class Evaluator;
class PruningMethod;

namespace plugins {
class Feature;
}

template<class T> 
class GooseOpenList {
  // copied from best_first_open_list.cc
  typedef std::deque<T> Bucket;

  std::map<int, Bucket> buckets;
  int size;

 public:
  GooseOpenList() {};

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
    --size;
    return result;
  }

  void insert(int key, const T &entry) {
    if (!buckets.count(key)) {
      buckets[key] = std::deque<T>();
    }
    buckets[key].push_back(entry);
    ++size;
  }

  bool empty() {
    return size == 0;
  }
};

namespace multi_queue_goose {
class MultiQueueGoose : public SearchAlgorithm {
  // fast hack for use with WL GOOSE

  std::shared_ptr<goose_linear::GooseLinear> goose_heuristic;
  std::vector<GooseOpenList<StateID>> open_lists;

  int n_linear_models_;
  bool symmetry_;

  std::shared_ptr<State> state;

 protected:
  virtual void initialize() override;
  virtual SearchStatus step() override;

 public:
  explicit MultiQueueGoose(const plugins::Options &opts);
  virtual ~MultiQueueGoose() = default;

  virtual void print_statistics() const override;

  void dump_search_space() const;

  // private:
  //     double hack_timer;
};

extern void add_options_to_feature(plugins::Feature &feature);
}  // namespace multi_queue_goose

#endif
