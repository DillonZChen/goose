/*
 * planvis.h
 *
 *  Created on: Jan 22, 2016
 *      Author: aldinger
 */

#ifndef UTILS_PLANVIS_H
#define UTILS_PLANVIS_H

#include "../globals.h" // ap_float
#include "../state_id.h"
#include "timer.h"

namespace utils {
class PlanVisLogger {
	std::string file_name = "plan_vis.data";
	std::string latex_file = "state_trace.data";
	std::string explored_file = "explored_trace.data";
	std::vector<std::string> var_names_latex;
	std::string prune_latex_string(std::string full_state);
public:
	PlanVisLogger();

	void log_duplicate(const StateID &stateid,
			ap_float g_val,
			const StateID &parentid);

	void log_node(const StateID &stateid,
			std::string variables,
			ap_float h_val,
			utils::Timer htime,
			ap_float g_val,
			const StateID &parentid,
			bool is_goal,
			bool is_init);

	void register_latex_var(std::string var_name);

	void log_latex(std::string numeric_vals);

	void log_latex_explored(std::string numeric_vals);

};
}

#endif /* PLANVIS_H */
