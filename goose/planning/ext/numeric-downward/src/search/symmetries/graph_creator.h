#ifndef SYMMETRIES_GRAPH_CREATOR_H
#define SYMMETRIES_GRAPH_CREATOR_H

//#include "../bliss-0.5/graph.hh"
#include <graph.hh>
#include "group.h"
#include "../plugin.h"
#include "../task_proxy.h"

enum SymmetryBasedSearchType {
    NO_SYMMETRIES,
    GOAL_ONLY_STABILIZED,
};


/**
 * This class will create a bliss graph which will be used to find the
 * automorphism groups
 */

class GraphCreator  {
    SymmetryBasedSearchType search_type;

    bool no_search;
    bool initialized;
    bool ignore_numeric;
    ap_float precision;

public:

    GraphCreator(const Options &opts);
    virtual ~GraphCreator();

    void initialize(const std::shared_ptr<AbstractTask> task);

    void get_canonical_state(std::vector<container_int> &values, std::vector<ap_float> &num_values) const {
        group.get_canonical_state(values, num_values);
    }

    bool to_canonical_state(PackedStateBin* buffer, std::vector<ap_float> &num_values) const {
        return group.to_canonical_state(buffer, num_values);
    }

    Permutation create_permutation_from_state_to_state(const GlobalState &from_state, const GlobalState &to_state) const;

    static void add_options_to_parser(OptionParser &parser);

    SymmetryBasedSearchType get_search_type() const { return search_type; }

    void free_memory() { group.free_memory(); }

private:
    Group group;

    int float_to_int(ap_float value) const;
    bliss::Digraph* create_bliss_directed_graph(const std::shared_ptr<AbstractTask> task) const;
};

#endif

