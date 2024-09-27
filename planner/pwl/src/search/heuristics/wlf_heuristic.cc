#include "wlf_heuristic.h"
#include "../task.h"

#include <cassert>

namespace py = pybind11;

using namespace std;

WlfHeuristic::WlfHeuristic(const Options &opts, const Task &task)
{
    // NGOOSE should point to root of this repository.
    // This is handled in a run.py script.
    auto gnn_path = std::getenv("NGOOSE");
    if (!gnn_path) {
        std::cout << "NGOOSE env variable not found. Aborting." << std::endl;
        exit(-1);
    }
    std::string path(gnn_path);
    std::cout << "NGOOSE path is " << path << std::endl;
    if (access(path.c_str(), F_OK) == -1) {
        std::cout << "NGOOSE points to non-existent path. Aborting." << std::endl;
        exit(-1);
    }

    // Append python module directory to the path
    py::module sys = py::module::import("sys");
    sys.attr("path").attr("append")(path);

    // Force all output being printed to stdout. Otherwise INFO logging from
    // python will be printed to stderr, even if it is not an error.
    sys.attr("stderr") = sys.attr("stdout");

    // Read paths
    model_path = opts.get_goose_model_path();
    domain_path = opts.get_domain_path();
    problem_path = opts.get_problem_path();

    std::chrono::duration<double> start_walltime =
        std::chrono::high_resolution_clock::now().time_since_epoch();
    std::cout << "Initialising NGOOSE heuristic." << std::endl;

    load_and_init_model("FeatureGenerationModel");

    // init timers
    graph_time = 0;
    wl_time = 0;
    linear_time = 0;

    // init ILG
    graph = std::make_shared<ngoose_wlf_graph::WlfGraph>(
        ngoose_wlf_graph::WlfGraph(std::make_shared<pybind11::object>(model), task));

    // init other model items
    iterations_ = model.attr("get_cat_iterations")().cast<int>();
    weights_ = model.attr("get_weights_single")().cast<std::vector<double>>();
    hash_ = model.attr("get_hash")().cast<std::unordered_map<std::string, int>>();
    n_init_features_ = model.attr("get_n_init_features")().cast<int>();
    n_features_ = model.attr("get_n_cat_features")().cast<int>();

    std::chrono::duration<double> end_walltime =
        std::chrono::high_resolution_clock::now().time_since_epoch();
    std::cout << "NGOOSE initialisation time: " << end_walltime.count() - start_walltime.count()
              << "s\n";
}


void WlfHeuristic::load_and_init_model(std::string py_model_class)
{
    std::cout << "Trying to load model from file " << model_path << " ..." << std::endl;
    std::cout << "Importing module..." << std::endl;
    pybind11::module model_module = pybind11::module::import("learner");
    std::cout << "Module imported." << std::endl;
    model_module.attr("print_torch_version")();
    std::cout << "Initialising model object..." << std::endl;
    model = model_module.attr(py_model_class.c_str())();
    std::cout << "Model object initialised." << std::endl;
    std::cout << "Loading..." << std::endl;
    model.attr("load")(model_path);
    std::cout << "Loaded." << std::endl;
    model.attr("dump")();
    std::string dummy = "";
    std::cout << "Setting domain and problem..." << std::endl;
    model.attr("set_domain_problem")(domain_path, problem_path, dummy);
    std::cout << "Domain and problem set." << std::endl;

    std::cout << "Base model initialisation complete!" << std::endl;
}


const ngoose_wlf_graph::WlfGraph WlfHeuristic::state_to_graph(const DBState &s, const Task &task)
{

    start_time = get_time();
    ngoose_wlf_graph::WlfGraph ret = graph->state_to_graph(s, task);
    end_time = get_time();

    graph_time += std::chrono::duration<double>(end_time - start_time).count();

    return ret;
}

std::vector<int> WlfHeuristic::compute_features(const ngoose_wlf_graph::WlfGraph &state_graph)
{
    const std::vector<int> x_cat = state_graph.get_x();
    const std::vector<std::vector<std::pair<int, int>>> neighbours = state_graph.get_neighbours();
    const size_t n_nodes = x_cat.size();

    start_time = get_time();
    std::vector<int> prv_x(n_nodes, 0);
    std::vector<int> cur_x(n_nodes, 0);
    std::vector<int> x(n_features_, 0);

    int cat;
    std::string new_colour;
    for (size_t u = 0; u < n_nodes; u++) {
        new_colour = std::to_string(x_cat[u]);
        if (hash_.count(new_colour)) {
            cat = hash_.at(new_colour);
            x[cat]++;
        }
        else {
            cat = -1;
        }
        prv_x[u] = cat;
    }

    for (int itr = 0; itr < iterations_; itr++) {
        for (size_t u = 0; u < n_nodes; u++) {
            const std::vector<std::pair<int, int>> &neigh = neighbours[u];
            std::set<std::pair<int, int>> neighbour_cats;  // sorted set

            if (prv_x[u] == -1) {
                cat = -1;
                goto end_of_loop;
            }

            // collect colours from neighbours
            for (size_t i = 0; i < neigh.size(); i++) {
                int v = neigh[i].first;
                cat = prv_x[v];
                if (cat == -1) {
                    goto end_of_loop;
                }
                neighbour_cats.insert(std::make_pair(cat, neigh[i].second));
            }

            // add current colour and sorted neighbours into sorted colour key
            new_colour = std::to_string(prv_x[u]);
            for (const auto &ne_pair : neighbour_cats) {
                new_colour +=
                    "," + std::to_string(ne_pair.first) + "," + std::to_string(ne_pair.second);
            }

            // hash seen colours
            if (hash_.count(new_colour)) {
                cat = hash_.at(new_colour);
                x[cat]++;
            }
            else {
                cat = -1;
            }

        end_of_loop:
            cur_x[u] = cat;
        }
        cur_x.swap(prv_x);
    }

    end_time = get_time();
    wl_time += end_time.count() - start_time.count();
    return x;
}

int WlfHeuristic::estimate(const std::vector<int> x)
{
    double h = 0;

    start_time = get_time();
    for (int i = 0; i < n_features_; i++) {
        h += weights_[i] * x[i];
    }
    end_time = get_time();
    linear_time += end_time.count() - start_time.count();

    return round(h);
}

int WlfHeuristic::compute_heuristic(const DBState &s, const Task &task)
{
    const ngoose_wlf_graph::WlfGraph graph = state_to_graph(s, task);
    std::vector<int> features = compute_features(graph);
    int h = estimate(features);

    return h;
}

void WlfHeuristic::print_statistics()
{
    std::cout << "WlfHeuristic graph time: " << graph_time << "s\n";
    std::cout << "WlfHeuristic feature time: " << wl_time << "s\n";
    std::cout << "WlfHeuristic linear time: " << linear_time << "s\n";
}
