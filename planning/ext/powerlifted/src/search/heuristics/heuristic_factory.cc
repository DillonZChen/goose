#include "heuristic_factory.h"

#include "../goose/qb_wl_heuristic.h"
#include "../goose/qb_pn_heuristic.h"
#include "../goose/wlgoose_heuristic.h"
#include "add_heuristic.h"
#include "blind_heuristic.h"
#include "ff_heuristic.h"
#include "goalcount.h"
#include "hmax_heuristic.h"
#include "rff_heuristic.h"

#include "datalog_transformation_options.h"

#include <fstream>
#include <iostream>

#include <boost/algorithm/string.hpp>

Heuristic *HeuristicFactory::create(const Options &opt, const Task &task)
{
    const std::string &method = opt.get_evaluator();

    std::cout << "Creating heuristic factory..." << std::endl;

    if (boost::iequals(method, "blind")) {
        return new BlindHeuristic();
    }
    else if (boost::iequals(method, "add")) {
        return new AdditiveHeuristic(task, DatalogTransformationOptions());
    }
    else if (boost::iequals(method, "ff")) {
        return new FFHeuristic(task, DatalogTransformationOptions());
    }
    else if (boost::iequals(method, "goalcount")) {
        return new Goalcount();
    }
    else if (boost::iequals(method, "hmax")) {
        return new HMaxHeuristic(task, DatalogTransformationOptions());
    }
    else if (boost::iequals(method, "rff")) {
        return new RFFHeuristic(task, DatalogTransformationOptions());
    }
    else if (boost::iequals(method, "wlgoose")) {
        return new WlGooseHeuristic(opt, task);
    }
    else if (boost::iequals(method, "qbwlgc")) {
        std::shared_ptr<Heuristic> h = std::make_shared<Goalcount>();
        return new QbWlHeuristic(opt, task, h);
    }
    else if (boost::iequals(method, "qbwladd")) {
        std::shared_ptr<Heuristic> h =
            std::make_shared<AdditiveHeuristic>(task, DatalogTransformationOptions());
        return new QbWlHeuristic(opt, task, h);
    }
    else if (boost::iequals(method, "qbwlff")) {
        std::shared_ptr<Heuristic> h =
            std::make_shared<FFHeuristic>(task, DatalogTransformationOptions());
        return new QbWlHeuristic(opt, task, h);
    }
    else if (boost::iequals(method, "qbpngc")) {
        std::shared_ptr<Heuristic> h = std::make_shared<Goalcount>();
        return new QbPnHeuristic(opt, task, h);
    }
    else if (boost::iequals(method, "qbpnadd")) {
        std::shared_ptr<Heuristic> h =
            std::make_shared<AdditiveHeuristic>(task, DatalogTransformationOptions());
        return new QbPnHeuristic(opt, task, h);
    }
    else if (boost::iequals(method, "qbpnff")) {
        std::shared_ptr<Heuristic> h =
            std::make_shared<FFHeuristic>(task, DatalogTransformationOptions());
        return new QbPnHeuristic(opt, task, h);
    }
    else {
        std::cerr << "Invalid heuristic \"" << method << "\"" << std::endl;
        exit(-1);
    }
}

Heuristic *HeuristicFactory::create_delete_free_heuristic(const std::string &method,
                                                          const Task &task)
{
    if (boost::iequals(method, "add")) {
        return new AdditiveHeuristic(task);
    }
    else if (boost::iequals(method, "ff")) {
        return new FFHeuristic(task);
    }
    else if (boost::iequals(method, "hmax")) {
        return new HMaxHeuristic(task, DatalogTransformationOptions());
    }
    else if (boost::iequals(method, "rff")) {
        return new RFFHeuristic(task);
    }
    else {
        std::cerr << "Invalid delete-free heuristic \"" << method << "\"" << std::endl;
        exit(-1);
    }
}
