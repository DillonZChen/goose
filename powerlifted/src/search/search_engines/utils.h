
#pragma once

#include "../utils/segmented_vector.h"

#include <ctime>
#include <utility>
#include <unordered_map>
#include <vector>
#include <time.h>
#include <sys/time.h>

class LiftedOperatorId;
class PackedStateHash;
class SuccessorGenerator;
class SparsePackedState;
class SparseStatePacker;
class Task;

double get_wall_time();

void print_no_solution_found(const double& timer_start);

void print_goal_found(
    const SuccessorGenerator& generator,
    const double& timer_start);


void extract_plan(
    segmented_vector::SegmentedVector<std::pair<int, LiftedOperatorId>> &cheapest_parent,
    SparsePackedState state,
    const std::unordered_map<SparsePackedState, int, PackedStateHash> &visited,
    segmented_vector::SegmentedVector<SparsePackedState> &index_to_state,
    const SparseStatePacker &packer,
    const Task &task);

void print_plan(const std::vector<LiftedOperatorId>& plan, const Task &task);
