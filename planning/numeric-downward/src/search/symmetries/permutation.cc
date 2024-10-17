#include "permutation.h"
#include "../globals.h"
#include <iomanip>
#include <iostream>
#include <vector>
#include <algorithm>
//#include <assert.h>
#include <cassert>

using namespace std;
#include <sstream>

int Permutation::length;
vector<int> Permutation::var_by_val;
vector<int> Permutation::dom_sum_by_regular_id;
vector<int> Permutation::var_to_regular_id;
vector<int> Permutation::regular_id_to_var;
int Permutation::dom_sum_num_var;
vector<int> Permutation::num_var_to_regular_id;
vector<int> Permutation::regular_id_to_num_var;


void Permutation::_allocate() {
    value = new int[length];
    inverse_value = new int[length];
    affected.assign(var_to_regular_id.size(), false);
    num_affected.assign(num_var_to_regular_id.size(), false);
    vars_affected.clear();
    num_vars_affected.clear();
    from_vars.assign(var_to_regular_id.size(), -1);
    from_num_vars.assign(num_var_to_regular_id.size(), -1);
    affected_vars_cycles.clear();
    affected_num_vars_cycles.clear();
}

void Permutation::_deallocate() {
    delete[] value;
    delete[] inverse_value;
}

void Permutation::_copy_value_from_permutation(const Permutation &perm) {
    for (int i = 0; i < length; i++)
        set_value(i, perm.get_value(i));
}

void Permutation::_inverse_value_from_permutation(const Permutation &perm) {
    for (int i = 0; i < length; i++)
        set_value(perm.get_value(i), i);
}

Permutation &Permutation::operator=(const Permutation &other) {
    if (this != &other) {
        affected.assign(var_to_regular_id.size(), false);
        num_affected.assign(num_var_to_regular_id.size(), false);
        vars_affected.clear();
        num_vars_affected.clear();
        from_vars.assign(regular_id_to_var.size(), -1);
        from_num_vars.assign(regular_id_to_num_var.size(), -1);
        affected_vars_cycles.clear();
        affected_num_vars_cycles.clear();
        _copy_value_from_permutation(other);
    }
    this->finalize();
    return *this;
}

Permutation::Permutation(){
    _allocate();
    for (int i = 0; i < length; i++)
        set_value(i,i);
    finalize();
}


Permutation::Permutation(const unsigned int* full_permutation){
    _allocate();
    for (int i = 0; i < length; i++){
        set_value(i,full_permutation[i]);
    }
    finalize();
}

Permutation::Permutation(const Permutation& perm, bool invert){
    _allocate();
    if (invert) {
        /*

        for (int i = 0; i < length; i++)
            set_value(perm.get_value(i), i);
        */
        _inverse_value_from_permutation(perm);
    } else {
        _copy_value_from_permutation(perm);
    }
    finalize();
}

// New constructor to use instead of * operator
Permutation::Permutation(const Permutation& perm1, const Permutation& perm2){
    _allocate();

    for (int i = 0; i < length; i++) {
        set_value(i, perm2.get_value(perm1.get_value(i)));
    }

//	cout << "Finalizing" << endl;
    finalize();
}



Permutation::~Permutation(){
    _deallocate();
}

void Permutation::finalize(){
    // Sorting the vector of affected variables
    ::sort(vars_affected.begin(), vars_affected.end());

    // Going over the vector from_vars of the mappings of the variables and finding cycles
//	affected_vars_cycles.clear();
    vector<bool> marked;
    marked.assign(var_to_regular_id.size(), false);
    for (int i = 0, n = from_vars.size(); i < n; i++) {
        if (marked[i] || from_vars[i] == -1)
            continue;

        int current = i;
        marked[current] = true;
        vector<int> cycle;
        cycle.push_back(current);

        while (from_vars[current] != i) {
            current = from_vars[current];
            marked[current] = true;
            cycle.insert(cycle.begin(), current);
        }
        // Get here when from_vars[current] == i.
        affected_vars_cycles.push_back(cycle);
    }

    ::sort(num_vars_affected.begin(), num_vars_affected.end());

    vector<bool> num_marked;
    num_marked.assign(num_var_to_regular_id.size(), false);
    for (int i = 0, n = from_num_vars.size(); i < n; ++i) {
        if (num_marked[i] || from_num_vars[i] == -1)
            continue;

        int current = i;
        num_marked[current] = true;
        vector<int> cycle;
        cycle.push_back(current);

        while (from_num_vars[current] != i) {
            current = from_num_vars[current];
            num_marked[current] = true;
            cycle.insert(cycle.begin(), current);
        }
        affected_num_vars_cycles.push_back(cycle);
    }
}

bool Permutation::identity() const{
    return vars_affected.size() == 0 && num_vars_affected.size() == 0;
}

bool Permutation::operator ==(const Permutation &other) const{

    for(int i = 0; i < length; i++) {
        if (get_value(i) != other.get_value(i)) return false;
    }

    return true;
}


void Permutation::permutation_on_state(std::vector<container_int> &values, std::vector<ap_float> &num_values) const {
    std::vector<container_int> new_values = values;
    std::vector<ap_float> new_num_values = num_values;

    for (int i = 0, n = vars_affected.size(); i < n; i++) {
        int var = vars_affected[i];
        int value = values[var];
        pair<int, int> new_var_val = get_new_var_val_by_old_var_val(var, value);
        int new_var = new_var_val.first;
        int new_val = new_var_val.second;
        if (var == new_var && value == new_val){
            new_values[var] = value;
        } else {
            //cout << "Value " << new_val << " for variable " << new_var << endl;
            if (new_val< 0 || new_val >= static_cast<int>(g_variable_domain[new_var]))
                cout << "Value " << new_val << " for variable " << new_var << endl;
            new_values[new_var] = new_val;
        }
    }

    for (int i = 0, n = num_vars_affected.size(); i < n; i++) {
        int var = num_vars_affected[i];
        int new_var = get_new_num_var_by_old_num_var(var);
        if (new_var == var){
            new_num_values[var] = num_values[var];
        } else {
            new_num_values[new_var] = num_values[var];
        }
    }

    values = new_values;
    num_values = new_num_values;
}

int Permutation::get_var_by_index(int ind) {
    if (ind < static_cast<int>(regular_id_to_var.size())) {
        cout << "=====> WARNING!!!! Check that this is done on purpose!" << endl;
        return ind;
    }

    return var_by_val[ind - regular_id_to_var.size()];
}

int Permutation::get_value_by_index(int ind, int var) {
    if (ind < static_cast<int>(regular_id_to_var.size()) || ind >= dom_sum_num_var) {
        cout << "=====> WARNING!!!! Check that this is done on purpose!" << endl;
        return ind;
    }

    int regular_id = var_to_regular_id[var];

    return ind - dom_sum_by_regular_id[regular_id];
}

int Permutation::get_num_var_by_index(int ind) {
    if (ind < dom_sum_num_var) {
        cout << "=====> WARNING!!!! Check that this is done on purpose!" << endl;
        return ind;
    }

    return regular_id_to_num_var[ind - dom_sum_num_var];
}

int Permutation::get_index_by_var(int var) {
    return var_to_regular_id[var];
}

int Permutation::get_index_by_var_val(int var, int val) {
    int regular_id = var_to_regular_id[var];

    return dom_sum_by_regular_id[regular_id] + val;
}

int Permutation::get_index_by_num_var(int num_var) {
    int regular_id = num_var_to_regular_id[num_var];

    return dom_sum_num_var + regular_id;
}

int Permutation::get_index_by_num_regular_id(int regular_id) {
    return dom_sum_num_var + regular_id;
}

pair<int, int> Permutation::get_new_var_val_by_old_var_val(int var, int value) const {
    int regular_id = var_to_regular_id[var];
    int ind = get_value(dom_sum_by_regular_id[regular_id] + value);
    int new_var = get_var_by_index(ind);
    int new_value = get_value_by_index(ind, new_var);

    return make_pair(new_var, new_value);
}

int Permutation::get_new_num_var_by_old_num_var(int num_var) const {
    int regular_id = num_var_to_regular_id[num_var];
    assert(regular_id != -1);
    int ind = get_value(get_index_by_num_regular_id(regular_id));

    return get_num_var_by_index(ind);
}

void Permutation::set_value(int ind, int val) {
    value[ind] = val;
    inverse_value[val] = ind;
    set_affected(ind, val);
}

void Permutation::set_affected(int ind, int val) {
    if (ind < static_cast<int>(regular_id_to_var.size()) || ind == val)
        return;

    if (ind < dom_sum_num_var) {
        int var = get_var_by_index(ind);
        int to_var = get_var_by_index(val);
        if (!affected[var]) {
            vars_affected.push_back(var);
            affected[var] = true;
        }
        if (!affected[to_var]) {
            vars_affected.push_back(to_var);
            affected[to_var] = true;
        }
        from_vars[to_var] = var;
    } else if (ind < dom_sum_num_var + static_cast<int>(regular_id_to_num_var.size())) {
        int num_var = get_num_var_by_index(ind);
        int to_num_var = get_num_var_by_index(val);
        if (!num_affected[num_var]) {
            num_vars_affected.push_back(num_var);
            num_affected[num_var] = true;
        }
        if (!num_affected[to_num_var]) {
            num_vars_affected.push_back(to_num_var);
            num_affected[to_num_var] = true;
        }
        from_num_vars[to_num_var] = num_var;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
// This method compares the state to the state resulting from permuting it.
// If the original state is bigger than the resulted one, it is rewritten with the latter and true is returned.
bool Permutation::replace_if_less(std::vector<container_int> &values, std::vector<ap_float> &num_values) const {
    if (identity())
        return false;

    bool values_same = true;
    // Going over the affected variables, comparing the resulted values with the state values.
    for (int i = vars_affected.size() - 1; i >= 0; i--) {
        int to_var = vars_affected[i];
        int from_var = from_vars[to_var];
        int from_val = values[from_var];
        pair<int, int> to_var_val = get_new_var_val_by_old_var_val(from_var, from_val);
        assert(to_var == to_var_val.first);
        int to_val = to_var_val.second;

        // Check if the values are the same, then continue to the next aff. var.
        if (to_val < static_cast<int>(values[to_var])) {
            values_same = false;
            break;
        } else if (to_val > static_cast<int>(values[to_var])) {
            return false;
        }
    }
    if (!values_same) {
        for (int i = 0, n = affected_vars_cycles.size(); i < n; i++) {
            if (affected_vars_cycles[i].size() == 1) {
                int var = affected_vars_cycles[i][0];
                int from_val = values[var];
                pair<int, int> to_var_val = get_new_var_val_by_old_var_val(var, from_val);
                assert(var == to_var_val.first);
                values[var] = to_var_val.second;
                continue;
            }
            // Remembering one value to be rewritten last
            int last_var = affected_vars_cycles[i][affected_vars_cycles[i].size() - 1];
            int last_val = values[last_var];

            for (int j = affected_vars_cycles[i].size() - 1; j > 0; j--) {
                // writing into variable affected_vars_cycles[i][j]
                int to_var = affected_vars_cycles[i][j];
                int from_var = affected_vars_cycles[i][j - 1];
                int from_val = values[from_var];
                pair<int, int> to_var_val = get_new_var_val_by_old_var_val(from_var, from_val);
                assert(to_var == to_var_val.first);
                values[to_var] = to_var_val.second;
            }
            // writing the last one
            pair<int, int> to_var_val = get_new_var_val_by_old_var_val(last_var, last_val);
            values[affected_vars_cycles[i][0]] = to_var_val.second;
        }
    } 

    bool num_values_same = true;
    for (int i = num_vars_affected.size() - 1; i >= 0; i--) {
        int to_var = num_vars_affected[i];
        int from_var = from_num_vars[to_var];

        // Check if the values are the same, then continue to the next aff. var.
        if (values_same && num_values[from_var] > num_values[to_var]) {
            return false;
        } else if (num_values[from_var] != num_values[to_var]) {
            num_values_same = false;
            break;
        }
    }
    if (!num_values_same) {
        for (int i = 0, n = affected_num_vars_cycles.size(); i < n; i++) {
            int last_var = affected_num_vars_cycles[i][affected_num_vars_cycles[i].size() - 1];
            ap_float last_val = num_values[last_var];

            for (int j = affected_num_vars_cycles[i].size() - 1; j > 0; j--) {
                int to_num_var = affected_num_vars_cycles[i][j];
                int from_num_var = affected_num_vars_cycles[i][j - 1];
                num_values[to_num_var] = num_values[from_num_var];
            }

            num_values[affected_num_vars_cycles[i][0]] = last_val;
        }
    }

    return !values_same || !num_values_same;
}

bool Permutation::replace_if_less(PackedStateBin *buffer, std::vector<ap_float> &num_values) const {
    if (identity())
        return false;

    bool values_same = true;
    // Going over the affected variables, comparing the resulted values with the state values.
    for (int i = vars_affected.size() - 1; i >= 0; i--) {
        int to_var = vars_affected[i];
        int from_var = from_vars[to_var];
        int from_val = g_state_packer->get(buffer, from_var);
        pair<int, int> to_var_val = get_new_var_val_by_old_var_val(from_var, from_val);
        assert(to_var == to_var_val.first);
        int to_val = to_var_val.second;
        int current_to_val = g_state_packer->get(buffer, to_var);

        // Check if the values are the same, then continue to the next aff. var.
        if (to_val < current_to_val) {
            values_same = false;
            break;
        } else if (to_val > current_to_val) {
            return false;
        }
    }
    if (!values_same) {
        for (int i = 0, n = affected_vars_cycles.size(); i < n; i++) {
            if (affected_vars_cycles[i].size() == 1) {
                int var = affected_vars_cycles[i][0];
                int from_val = g_state_packer->get(buffer, var);
                pair<int, int> to_var_val = get_new_var_val_by_old_var_val(var, from_val);
                assert(var == to_var_val.first);
                g_state_packer->set(buffer, var, to_var_val.second);
                continue;
            }
            // Remembering one value to be rewritten last
            int last_var = affected_vars_cycles[i][affected_vars_cycles[i].size() - 1];
            int last_val = g_state_packer->get(buffer, last_var);

            for (int j = affected_vars_cycles[i].size() - 1; j > 0; j--) {
                // writing into variable affected_vars_cycles[i][j]
                int to_var = affected_vars_cycles[i][j];
                int from_var = affected_vars_cycles[i][j - 1];
                int from_val = g_state_packer->get(buffer, from_var);
                pair<int, int> to_var_val = get_new_var_val_by_old_var_val(from_var, from_val);
                assert(to_var == to_var_val.first);
                g_state_packer->set(buffer, to_var, to_var_val.second);
            }
            // writing the last one
            pair<int, int> to_var_val = get_new_var_val_by_old_var_val(last_var, last_val);
            g_state_packer->set(buffer, affected_vars_cycles[i][0], to_var_val.second);
        }
    }

    bool num_values_same = true;
    for (int i = num_vars_affected.size() - 1; i >= 0; i--) {
        int to_var = num_vars_affected[i];
        int from_var = from_num_vars[to_var];

        // Check if the values are the same, then continue to the next aff. var.
        if (values_same && num_values[from_var] > num_values[to_var]) {
            return false;
        } else if (num_values[from_var] != num_values[to_var]) {
            num_values_same = false;
            break;
        }
    }
    if (!num_values_same) {
        for (int i = 0, n = affected_num_vars_cycles.size(); i < n; i++) {
            int last_var = affected_num_vars_cycles[i][affected_num_vars_cycles[i].size() - 1];
            ap_float last_val = num_values[last_var];

            for (int j = affected_num_vars_cycles[i].size() - 1; j > 0; j--) {
                int to_num_var = affected_num_vars_cycles[i][j];
                int from_num_var = affected_num_vars_cycles[i][j - 1];
                num_values[to_num_var] = num_values[from_num_var];
            }

            num_values[affected_num_vars_cycles[i][0]] = last_val;
        }
    }

    return !values_same || !num_values_same;
}

void Permutation::print_cycle_notation() const {
    vector<int> done;
    int id_min = static_cast<int>(regular_id_to_var.size());
    for (int i = id_min; i < dom_sum_num_var; i++) {
        if (find(done.begin(), done.end(), i) == done.end()) { 
            int current = i;
            if(get_value(i) == i) continue; //don't print cycles of size 1

            int var = get_var_by_index(i);
            int value = get_value_by_index(i, var);
//	        cout<<"("<< varval.first << "=" << (int) varval.second <<" ";
            cout<<"("<< g_fact_names[var][value]  <<" ";

//	        cout<<"("<<i<<" ";

            while(get_value(current) != i){
                done.push_back(current);
                current = get_value(current);

                int currvar = get_var_by_index(current);
                int currvalue = get_value_by_index(current, currvar);
                cout<< g_fact_names[currvar][currvalue] <<" ";
//	            cout<< currvarval.first << "=" << (int) currvarval.second <<" ";
//	            cout<<current<<" ";

            }
            done.push_back(current);
            cout<<") ";
        }
    }
    int id_max = dom_sum_num_var + static_cast<int>(regular_id_to_num_var.size());
    for (int i = dom_sum_num_var; i < id_max; i++) {
        if (find(done.begin(), done.end(), i) == done.end()) {
            int current = i;
            if(get_value(i) == i) continue; //don't print cycles of size 1

            int num_var = get_num_var_by_index(i);
            cout<<"("<< g_numeric_var_names[num_var] <<" ";

            while(get_value(current) != i){
                done.push_back(current);
                current = get_value(current);

                int currnum_var = get_num_var_by_index(current);
                cout<< g_numeric_var_names[currnum_var] <<" ";
            }
            done.push_back(current);
            cout<<") ";
        }
    }
    std::cout << std::endl;
}