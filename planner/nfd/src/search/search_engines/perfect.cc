#include "perfect.h"

#include "search_common.h"

#include "../evaluation_context.h"
#include "../globals.h"
#include "../heuristic.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../pruning_method.h"
#include "../successor_generator.h"
#include "../utils/planvis.h"
#include "../utils/timer.h"

#include "../open_lists/open_list_factory.h"

#include <cassert>
#include <cstdlib>
#include <memory>
#include <set>
#include <sstream>

using namespace std;

namespace perfect {
Perfect::Perfect(const Options &opts) : SearchEngine(opts) {
  g_initial_state().dump_facts_and_fluents();
}

static SearchEngine *_parse(OptionParser &parser) {
  parser.document_synopsis("Parse NFD facts and fluents", "");
  SearchEngine::add_options_to_parser(parser);
  Options opts = parser.parse();
  Perfect *engine = nullptr;
  if (!parser.dry_run()) {
    engine = new Perfect(opts);
  }
  return engine;
}

static Plugin<SearchEngine> _plugin("perfect", _parse);
}  // namespace perfect
