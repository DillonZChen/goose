#include "coloured_graph.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <map>
#include <regex>
#include <string>
#include <vector>

CGraph::CGraph() {}

CGraph::CGraph(const Edges &edges, const std::vector<int> &colour)
    : edges_(edges), colour_(colour) {}
