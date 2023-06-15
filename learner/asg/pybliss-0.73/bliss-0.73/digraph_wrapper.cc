#include "digraph_wrapper.h"

#include "defs.h"
#include "graph.h"

#include <cassert>
#include <iostream>

using namespace std;

void _add_automorphism(void* param, unsigned int size, const unsigned int *automorphism) {
    DigraphWrapper *wrapper = static_cast<DigraphWrapper *>(param);
    wrapper->add_automorphism(size, automorphism);
}

DigraphWrapper::DigraphWrapper() {
    graph = new bliss::Digraph();
}

DigraphWrapper::~DigraphWrapper() {
    delete graph;
}

void DigraphWrapper::add_vertex(int color) {
    graph->add_vertex(color);
}

void DigraphWrapper::add_edge(int v1, int v2) {
    graph->add_edge(v1, v2);
}

vector<vector<int> > DigraphWrapper::find_automorphisms(double time_limit) {
    automorphisms.clear();
    graph->set_splitting_heuristic(bliss::Digraph::shs_fs);
    graph->set_time_limit(time_limit);
    bliss::Stats stats;
    //cout << "DigraphWrapper: searching for automorphisms... " << endl;
    try {
        graph->find_automorphisms(stats, &(_add_automorphism), this);
    } catch (bliss::BlissException &e) {
        e.dump();
    }
    vector<vector<int> > result;
    result.swap(automorphisms);
    return result;
}

void DigraphWrapper::add_automorphism(
    unsigned int automorphism_size, const unsigned int *automorphism) {
    assert(automorphisms_size == graph->get_nof_vertices());
    //cout << "DigraphWrapper: found generator" << endl;
    // Copy the array to the vector (do not just store a pointer to the array!)
    vector<int> new_aut;
    new_aut.reserve(automorphism_size);
    for (size_t i = 0; i < automorphism_size; ++i) {
        //cout << i << "->" << automorphism[i] << endl;
        new_aut.push_back(automorphism[i]);
    }
    automorphisms.push_back(new_aut);
}
