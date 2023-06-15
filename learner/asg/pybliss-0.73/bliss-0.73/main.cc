#include "digraph_wrapper.h"

#include <iostream>

using namespace std;

int main() {
    DigraphWrapper graph;
    graph.add_vertex(0);
    graph.add_vertex(1);
    graph.add_vertex(1);
    graph.add_edge(0, 1);
    graph.add_edge(0, 2);
    graph.find_automorphisms();
    const vector<const unsigned int *> &automorphisms = graph.get_automorphisms();
    for (const unsigned int *aut : automorphisms) {
        cout << "automorphism:" << endl;
        for (size_t i = 0; i < 3; ++i) {
            cout << i << "->" << aut[i] << endl;
        }
    }
}
