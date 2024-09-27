#ifndef GLOBAL_STATE_H
#define GLOBAL_STATE_H

#include "int_packer.h"
#include "state_id.h"

#include <cstddef>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class GlobalOperator;
class StateRegistry;

typedef IntPacker::Bin PackedStateBin;

// For documentation on classes relevant to storing and working with registered
// states see the file state_registry.h.
class GlobalState {
  friend class StateRegistry;
  template <typename Entry> friend class PerStateInformation;
  // Values for vars are maintained in a packed state and accessed on demand.
  const PackedStateBin *buffer;
  // registry isn't a reference because we want to support operator=
  const StateRegistry *registry;
  StateID id;
  // Only used by the state registry.
  GlobalState(const PackedStateBin *buffer_, const StateRegistry &registry_,
              StateID id_);

  const PackedStateBin *get_packed_buffer() const { return buffer; }

  const StateRegistry &get_registry() const { return *registry; }

  // No implementation to prevent default construction
  GlobalState();

 public:
  ~GlobalState();

  StateID get_id() const { return id; }

  container_int operator[](std::size_t index) const;

  bool same_values(const GlobalState &state) const;

  bool same_values(const std::vector<container_int> &values,
                   const std::vector<ap_float> &numeric_values) const;

  std::vector<ap_float> get_numeric_vars() const;
  std::string get_str_facts_and_fluents() const;
  void dump_facts_and_fluents() const;
  void dump_readableB() const;
  void dump_readable() const;
  void dump_pddl() const;
  void dump_fdr() const;
  std::string get_numeric_state_vals_string() const;
  std::string dump_plan_vis_log() const;
  std::string dump_plan_vis_log(const GlobalState &parent) const;

  std::vector<std::string>
  get_facts(const std::vector<std::vector<std::string>> &fdr_pair_to_name,
            const std::vector<std::vector<bool>> &fdr_pair_to_is_true) const;
  std::vector<ap_float> get_num_values(const std::vector<int> &indices) const;
};

#endif
