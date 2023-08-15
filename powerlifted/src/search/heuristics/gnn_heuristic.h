#ifndef SEARCH_GNN_HEURISTIC_H
#define SEARCH_GNN_HEURISTIC_H

// #include <torch/script.h>
// #include <torchscatter/scatter.h>
// #include <torchsparse/sparse.h>

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/pybind11.h>

#include "heuristic.h"

class GNNHeuristic : public Heuristic {

    bool lifted_state_input;

    // torch::jit::script::Module model;

    // Required for pybind. Once this goes out of scope python interaction is no
    // longer possible.
    //
    // IMPORTANT: since member variables are destroyed in reverse order of
    // construction it is important that the scoped_interpreter_guard is listed as
    // first member variable (therefore destroyed last), otherwise calling the
    // destructor of this class will lead to a segmentation fault.
    pybind11::scoped_interpreter guard;

    pybind11::object model;

    pybind11::list lifted_state_to_python(const DBState &s, const Task &task);
    pybind11::list grounded_state_to_python(const DBState &s, const Task &task);


public:
    GNNHeuristic(const Task &task,
                 const std::string &model_path,
                 const std::string &domain_file,
                 const std::string &instance_file);

    int compute_heuristic(const DBState &s, const Task &task) override;

    std::vector<int> compute_heuristic_batch(const std::vector<DBState> &states, const Task &task);

};


#endif  // SEARCH_GNN_HEURISTIC_H
