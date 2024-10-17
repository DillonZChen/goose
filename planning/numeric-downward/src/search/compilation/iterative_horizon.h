#ifndef IP_ITERATIVE_SEARCH_H
#define IP_ITERATIVE_SEARCH_H

#include "../search_engine.h"
#include "../task_proxy.h"
#include "../lp/lp_solver.h"
#include "../utils/language.h"
#include "../utils/system.h"
#include "ip_compilation.h"
#include "../utils/countdown_timer.h"


namespace options {
    class Options;
}

namespace ip_compilation{
    
    class IPCompilation;
    
    class IterativeHorizon : public SearchEngine {
    protected:
        virtual void initialize() override;
        virtual SearchStatus step() override {};
        int initial_t;
    public:
        explicit IterativeHorizon(const options::Options &opts);
        virtual ~IterativeHorizon() override;
        virtual void print_statistics() const override{};
        utils::CountdownTimer timer;
        IPCompilation* model;
    };

    void add_model_option_to_parser(options::OptionParser &parser);
}

#endif
