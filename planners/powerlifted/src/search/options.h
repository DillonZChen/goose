#ifndef SEARCH_OPTIONS_H
#define SEARCH_OPTIONS_H

#include <iostream>

// #include <boost/program_options.hpp>
#include "../ext/cxxopts/cxxopts.h"

// namespace po = boost::program_options;

class Options {
    std::string filename;
    std::string domainfile;
    std::string instancefile;
    std::string modelpath;
    std::string generator;
    std::string search_engine;
    std::string evaluator;
    std::string state_representation;
    std::string plan_file;
    bool only_effects_opt;
    bool novelty_early_stop;
    unsigned seed;

public:
    Options(int argc, char** argv) {
      cxxopts::Options options("PWL");
      options.add_options()
        ("h,help", "Help text")
        ("f,filename", "Lifted task file name.", cxxopts::value<std::string>()->default_value("output.lifted"))
        ("d,domain-file", "Domain file name.", cxxopts::value<std::string>())
        ("i,instance-file", "Instance file name.", cxxopts::value<std::string>())
        ("m,model-path", "Saved GNN model path.", cxxopts::value<std::string>()->default_value(""))
        ("seed", "Random seed.", cxxopts::value<unsigned>()->default_value("1"))
        ("e,evaluator", "Heuristic evaluator.", cxxopts::value<std::string>())
        ("g,generator", "Successor generator method.", cxxopts::value<std::string>())
        ("s,search", "Search engine.", cxxopts::value<std::string>())
        ("r,state-representation", "State representation.", cxxopts::value<std::string>()->default_value("sparse"))
        ("plan-file", "Plan file.", cxxopts::value<std::string>()->default_value("FilePathUndefined"))
        ("only-effects-novelty-check", "Check only effects of applied actions when evaluation novelty of a state.", cxxopts::value<bool>()->default_value("false"))
        ("novelty-early-stop", "Stop evaluating novelty as soon as w-value is defined.", cxxopts::value<bool>()->default_value("false"))
        ;

      auto vm = options.parse(argc, argv);

      if (vm.count("help") || argc < 3) {  // evaluator, search, generator required
          std::cout << options.help() << std::endl;
          exit(0);
      }

      filename = vm["filename"].as<std::string>();
      domainfile = vm["domain-file"].as<std::string>();
      instancefile = vm["instance-file"].as<std::string>();
      modelpath = vm["model-path"].as<std::string>();
      generator = vm["generator"].as<std::string>();
      evaluator = vm["evaluator"].as<std::string>();
      search_engine = vm["search"].as<std::string>();
      state_representation = vm["state-representation"].as<std::string>();
      plan_file = vm["plan-file"].as<std::string>();
      only_effects_opt = vm["only-effects-novelty-check"].as<bool>();
      novelty_early_stop = vm["novelty-early-stop"].as<bool>();
      seed = vm["seed"].as<unsigned>();

    // does not work on gadi
    // Options(int argc, char** argv) {
    //     po::options_description description("Allowed options");
    //     description.add_options()
    //         ("filename,f", po::value<std::string>()->default_value("output.lifted"), "Lifted task file name.")
    //         ("domain-file,d", po::value<std::string>(), "Domain file name.")
    //         ("instance-file,i", po::value<std::string>(), "Instance file name.")
    //         ("model-path,m", po::value<std::string>()->default_value(""), "Saved GNN model path.")
    //         ("help,h", "Display this help message.")
    //         ("seed", po::value<unsigned>()->default_value(1), "Random seed.")
    //         ("evaluator,e", po::value<std::string>()->required(), "Heuristic evaluator.")
    //         ("generator,g", po::value<std::string>()->required(), "Successor generator method.")
    //         ("search,s", po::value<std::string>()->required(), "Search engine.")
    //         ("state-representation,r", po::value<std::string>()->default_value("sparse"), "State representation.")
    //         ("plan-file", po::value<std::string>()->default_value("FilePathUndefined"), "Plan file.")
    //         ("only-effects-novelty-check", po::value<bool>()->default_value(false), "Check only effects of applied actions when evaluation novelty of a state.")
    //         ("novelty-early-stop", po::value<bool>()->default_value(false), "Stop evaluating novelty as soon as w-value is defined.")
    //         ;

    //     po::variables_map vm;

    //     try {
    //         po::store(po::command_line_parser(argc, argv).options(description).run(), vm);

    //         if (vm.count("help")) {
    //             std::cout << description << "\n";
    //             exit(0);
    //         }
    //         po::notify(vm);
    //     } catch(const std::exception& ex) {
    //         std::cout << "Error with command-line options:" << ex.what() << std::endl;
    //         std::cout << std::endl << description << std::endl;
    //         exit(1);
    //     }

    //     filename = vm["filename"].as<std::string>();
    //     domainfile = vm["domain-file"].as<std::string>();
    //     instancefile = vm["instance-file"].as<std::string>();
    //     modelpath = vm["model-path"].as<std::string>();
    //     generator = vm["generator"].as<std::string>();
    //     evaluator = vm["evaluator"].as<std::string>();
    //     search_engine = vm["search"].as<std::string>();
    //     state_representation = vm["state-representation"].as<std::string>();
    //     plan_file = vm["plan-file"].as<std::string>();
    //     only_effects_opt = vm["only-effects-novelty-check"].as<bool>();
    //     novelty_early_stop = vm["novelty-early-stop"].as<bool>();
    //     seed = vm["seed"].as<unsigned>();

    }

    const std::string &get_filename() const {
        return filename;
    }

    const std::string &get_domain_file() const {
        return domainfile;
    }

    const std::string &get_instance_file() const {
        return instancefile;
    }

    const std::string &get_model_path() const {
        return modelpath;
    }

    const std::string &get_successor_generator() const {
        return generator;
    }

    const std::string &get_search_engine() const {
        return search_engine;
    }

    const std::string &get_evaluator() const {
        return evaluator;
    }

    const std::string &get_state_representation() const {
        return state_representation;
    }

    const std::string &get_plan_file() const {
        return plan_file;
    }

    bool get_only_effects_opt() const {
        return only_effects_opt;
    }

    bool get_novelty_early_stop() const {
        return novelty_early_stop;
    }

    unsigned get_seed() const {
        return seed;
    }


};

#endif //SEARCH_OPTIONS_H
