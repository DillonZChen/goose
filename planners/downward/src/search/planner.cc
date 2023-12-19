#include "command_line.h"
#include "search_engine.h"

#include "tasks/root_task.h"
#include "task_utils/task_properties.h"
#include "utils/logging.h"
#include "utils/system.h"
#include "utils/timer.h"

#include <iostream>
#include <chrono>

using namespace std;
using utils::ExitCode;

int main(int argc, const char **argv) {
    utils::register_event_handlers();

    if (argc < 2) {
        utils::g_log << usage(argv[0]) << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }

    bool unit_cost = false;
    if (static_cast<string>(argv[1]) != "--help") {
        utils::g_log << "reading input..." << endl;
        tasks::read_root_task(cin);
        utils::g_log << "done reading input!" << endl;
        TaskProxy task_proxy(*tasks::g_root_task);
        unit_cost = task_properties::is_unit_cost(task_proxy);
    }

    shared_ptr<SearchEngine> engine = parse_cmd_line(argc, argv, unit_cost);

    // timer is inaccurate because of pybind11?
    // 3 seconds would be counted as 30 seconds during search...
    std::chrono::time_point<std::chrono::system_clock> start_search = std::chrono::system_clock::now();
    engine->search();
    std::chrono::time_point<std::chrono::system_clock> end_search = std::chrono::system_clock::now();
    utils::g_timer.stop();

    double search_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_search - start_search).count();
    search_time /= 1000;

    engine->save_plan_if_necessary();
    engine->print_statistics();
    std::cout << "[Computation by walltime] Search time: " << search_time << "s" << std::endl;

    ExitCode exitcode = engine->found_solution()
        ? ExitCode::SUCCESS
        : ExitCode::SEARCH_UNSOLVED_INCOMPLETE;
    utils::report_exit_code_reentrant(exitcode);
    return static_cast<int>(exitcode);
}
