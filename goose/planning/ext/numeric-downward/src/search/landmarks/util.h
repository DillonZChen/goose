#ifndef LANDMARKS_UTIL_H
#define LANDMARKS_UTIL_H

#include "../globals.h" // container_int

#include <unordered_map>
#include <vector>

struct GlobalCondition;
class GlobalOperator;

namespace landmarks {
class LandmarkNode;

bool _possibly_fires(const std::vector<GlobalCondition> &conditions,
                     const std::vector<std::vector<int>> &lvl_var);

std::unordered_map<int, container_int> _intersect(
    const std::unordered_map<int, container_int> &a,
    const std::unordered_map<int, container_int> &b);

bool _possibly_reaches_lm(const GlobalOperator &o,
                          const std::vector<std::vector<int>> &lvl_var,
                          const LandmarkNode *lmp);
}

#endif
