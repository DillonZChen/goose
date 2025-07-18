#include "ip_constraint_generator.h"

#include "../plugin.h"

namespace gurobi_ip_compilation {

static PluginTypePlugin<GurobiIPConstraintGenerator> _type_plugin(
    "GurobiIPConstraintGenerator",
    // TODO: Replace empty string by synopsis for the wiki page.
    "");
}  // namespace gurobi_ip_compilation
