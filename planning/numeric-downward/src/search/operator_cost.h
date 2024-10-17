#ifndef OPERATOR_COST_H
#define OPERATOR_COST_H

#include  "globals.h"

class GlobalOperator;

namespace options {
class OptionParser;
}

enum OperatorCost {NORMAL = 0, ONE = 1, PLUSONE = 2, MAX_OPERATOR_COST};

ap_float get_adjusted_action_cost(ap_float cost, OperatorCost cost_type);
ap_float get_adjusted_action_cost(const GlobalOperator &op, OperatorCost cost_type);
void add_cost_type_option_to_parser(options::OptionParser &parser);

#endif
