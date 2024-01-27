#ifndef BLIND_HEURISTIC_H
#define BLIND_HEURISTIC_H

#include "../heuristic.h"



namespace blind_heuristic {
class Blind : public Heuristic {

protected:
  virtual int compute_heuristic(const State &ancestor_state) override;
  virtual std::vector<int> compute_heuristic_batch(
    const std::vector<State> &ancestor_states) override;
  
public:
  explicit Blind(const plugins::Options &opts);
};

} // namespace blind_heuristic

#endif

