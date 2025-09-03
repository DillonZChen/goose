#include "option_parser.h"
#include "search_engine.h"

#include "utils/timer.h"
#include "utils/system.h"

// #include <boost/static_assert.hpp>
// #include <cstdlib> // 32 bit / 64 bit bucktet assertions
#include <climits> // bitsize limits
#include <fstream> // eclipse run
#include <iostream>
#include <unistd.h>
 #define GetCurrentDir getcwd

using namespace std;
using utils::ExitCode;



int main(int argc, const char **argv) {

	/* the state space is bucketed into Bins of 64bit sized "unsigned long long"s.
	 * NFD requires numeric double variables (64bit) to fit into these buckets
	 */
    // TODO (temporary disable, need to re-enable this two lines)
	// BOOST_STATIC_ASSERT(sizeof(double) * CHAR_BIT == 64);
	// BOOST_STATIC_ASSERT(sizeof(unsigned long long) * CHAR_BIT == 64);

    utils::register_event_handlers();

    /*
     * The following block enables to execute search within the eclipse development environment
     * as it is not possible to pipe a file to stdin via the command line of eclipse
     */
    ifstream file("/Users/chiara/Work/Code/numerical-fast-downward/builds/xcode/output");

    if(string(argv[argc - 1]).compare("-eclipserun") == 0) {
    	cout << "Running Fast Downward in the eclipse shell" << endl;
    	cin.rdbuf(file.rdbuf());
        cerr.rdbuf(cout.rdbuf());
        argc--;
    }
//    else {
//    	cout << "running regular fd" << endl;
//    }

    if (argc < 2) {
        cout << OptionParser::usage(argv[0]) << endl;
        utils::exit_with(ExitCode::INPUT_ERROR);
    }

    if (string(argv[1]).compare("--help") != 0)
        read_everything(cin);

    SearchEngine *engine = 0;

    // The command line is parsed twice: once in dry-run mode, to
    // check for simple input errors, and then in normal mode.
    bool unit_cost = is_unit_cost();
    try {
        OptionParser::parse_cmd_line(argc, argv, true, unit_cost);
        engine = OptionParser::parse_cmd_line(argc, argv, false, unit_cost);
//        cout << "debug 1" << endl;
    } catch (ArgError &error) {
        cerr << error << endl;
        OptionParser::usage(argv[0]);
        utils::exit_with(ExitCode::INPUT_ERROR);
    } catch (ParseError &error) {
        cerr << error << endl;
        utils::exit_with(ExitCode::INPUT_ERROR);
    }
//    cout << "debug 2 --> path "<< endl;

    char cCurrentPath[FILENAME_MAX];
     if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
         {
         return errno;
         }
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
    printf ("The current working directory is %s\n", cCurrentPath);

    utils::Timer search_timer;
    engine->search();
    search_timer.stop();
    utils::g_timer.stop();

    engine->save_plan_if_necessary();
    engine->print_statistics();
    cout << "Search time: " << search_timer << endl;
    cout << "Total time: " << utils::g_timer << endl;

    if (engine->found_solution()) {
        utils::exit_with(ExitCode::PLAN_FOUND);
    } else {
        utils::exit_with(ExitCode::UNSOLVED_INCOMPLETE);
    }
}
