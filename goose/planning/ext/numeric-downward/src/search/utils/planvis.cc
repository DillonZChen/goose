/*
 * planvis.cc
 *
 *  Created on: Jan 22, 2016
 *      Author: aldinger
 */

#include "planvis.h"
#include <fstream>
#include <sstream>
#include <cassert>
using namespace std;

namespace utils {
PlanVisLogger::PlanVisLogger() {
	var_names_latex.clear();
	if (PLAN_VIS_LOG == plan_vis_log) {
	ofstream outfile;
		outfile.open(file_name);
		assert(g_variable_name.size() > 0);
		outfile << "{\"vars\":[{\"VN\":\"" << g_variable_name[0] << "\",\n\"Vals\":[\"" << g_fact_names[0][0] << "\"";
		for (size_t j=1; j < g_fact_names[0].size(); ++j)
			outfile	<< ", \"" <<g_fact_names[0][j] << "\"";
		outfile << "]}";
		for (size_t i= 1; i < g_variable_name.size(); ++i) {
			outfile << ",\n{\"VN\":\"" << g_variable_name[i] << "\",\n\"Vals\":[\"" << g_fact_names[i][0] << "\"";
			for (size_t j=1; j < g_fact_names[i].size(); ++j)
				outfile	<< ", \"" <<g_fact_names[i][j] << "\"";
			outfile << "]}";
		}
		for (size_t i= 0; i < g_numeric_var_names.size(); ++i)
			outfile << ",\n{\"VN\":\"" << g_numeric_var_names[i] << "\"}";
		outfile << "],\"states\":[";
		outfile.close();
	}
}


void PlanVisLogger::log_node(const StateID& stateid, std::string variables,
		ap_float h_val, Timer htime, ap_float g_val, const StateID& parentid, bool is_goal,
		bool is_init) {

	ofstream outfile;
	outfile.open(file_name, std::ofstream::app);
	stringstream ss;
	ss << stateid;
	string converted = ss.str();
	converted.erase(0,1); // remove leading '#'
	outfile << "{\t\"ID\":\"" << converted << "\",\n";
	outfile << "\t\"V\":[" << variables << "],\n";
    outfile << "\t\"H\":" << h_val << ",\n";
    double timeval = htime();
    outfile << "\t\"HT\": " << timeval << ",\n";
    outfile << "\t\"G\":" << g_val << ",\n";
    if (is_goal) {
    	outfile << "\t\"GoalState\": true,\n";
    }
    if (is_init) {
    	outfile << "\t\"InitialState\": true\n},\n";
    } else {
    	ss.str(std::string());
    	ss << parentid;
    	converted = ss.str();
    	converted.erase(0,1); // remove leading '#'
    	outfile << "\t\"P\":\"" << converted << "\"\n},\n";
    }
    outfile.close();
}

void PlanVisLogger::log_duplicate(const StateID& stateid, ap_float g_val,
		const StateID& parentid) {
	ofstream outfile;
	outfile.open(file_name, std::ofstream::app);
	stringstream ss;
	ss << stateid;
	string converted = ss.str();
	converted.erase(0,1); // remove leading '#'
    outfile << "{\t\"ID\":\"" << converted << "\",\n";
    outfile << "\t\"G\":" << g_val << ",\n";
    ss.str(std::string());
    ss << parentid;
    converted = ss.str();
    converted.erase(0,1); // remove leading '#'
    outfile << "\t\"P\":\"" << converted << "\"\n}\n";
    outfile.close();
}

void PlanVisLogger::log_latex(string numeric_vals) {
	ofstream outfile;
		outfile.open(latex_file, std::ofstream::app);
	    outfile << prune_latex_string(numeric_vals) << "\n";
	    outfile.close();
}

void PlanVisLogger::register_latex_var(std::string var_name) {
	var_names_latex.push_back(var_name);
}

void PlanVisLogger::log_latex_explored(string numeric_vals) {
	ofstream outfile;
		outfile.open(explored_file, std::ofstream::app);
		// ignore first two variables (by searching for two whitespaces)
	    outfile << prune_latex_string(numeric_vals) << "\n";
	    outfile.close();
}

string PlanVisLogger::prune_latex_string(string full_state) {
	stringstream ss;
	//stringstream pne;
	//cout << "DEBUG: full state = " << full_state << endl;
	for (string &varname : var_names_latex) {
		size_t position = full_state.find("=",full_state.find("PNE "+varname))+1;
		//cout << varname << " substring = " << full_state.substr(position, full_state.find(";",position)-position) << endl;
		ss << full_state.substr(position, full_state.find(";",position)-position) << " ";
	}
	return ss.str();
}

}
