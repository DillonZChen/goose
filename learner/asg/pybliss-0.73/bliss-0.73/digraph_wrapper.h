#ifndef DIGRAPH_WRAPPER_HH
#define DIGRAPH_WRAPPER_HH

#include <vector>

namespace bliss {
class Digraph;
}

class DigraphWrapper {
private:
    bliss::Digraph *graph;
    std::vector<std::vector<int> > automorphisms;
public:
    DigraphWrapper();
    ~DigraphWrapper();
    void add_vertex(int color);
    void add_edge(int v1, int v2);
    std::vector<std::vector<int> > find_automorphisms(
        double time_limit = 0);
    void add_automorphism(
        unsigned int automorphism_size,
        const unsigned int *automorphism);
};

#endif
