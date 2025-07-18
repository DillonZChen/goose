#ifndef SYMMETRIES_PERMUTATION_H
#define SYMMETRIES_PERMUTATION_H
#include "../task_proxy.h"
#include <utility>
#include <vector>
#include <string>
#include <fstream>

class Permutation{
public:
    Permutation();
    Permutation(const unsigned int*);
    Permutation(const Permutation&, bool invert=false);
    Permutation(const Permutation& perm1, const Permutation& perm2);
    ~Permutation();

    Permutation& operator =(const Permutation&);
    bool operator ==(const Permutation&) const;

    bool identity() const;
    void permutation_on_state(std::vector<container_int> &values, std::vector<ap_float> &num_values) const;
    void set_value(int ind, int val);
    int get_value(int ind) const { return value[ind]; }
    int get_inverse_value(int ind) const { return inverse_value[ind]; }

    int n_var_cycles() const { return affected_vars_cycles.size(); }
    int n_num_var_cycles() const { return affected_num_vars_cycles.size(); }

    void print_cycle_notation() const;

    static int get_var_by_index(int ind);
    static int get_value_by_index(int ind, int var);
    static int get_num_var_by_index(int ind);
    static int get_index_by_var(int var);
    static int get_index_by_var_val(int var, int val);
    static int get_index_by_num_var(int num_var);
    static int get_index_by_num_regular_id(int regular_id);

    static int length;
    static std::vector<int> var_by_val;
    static std::vector<int> dom_sum_by_regular_id;
    static std::vector<int> var_to_regular_id;
    static std::vector<int> regular_id_to_var;
    static int dom_sum_num_var;
    static std::vector<int> num_var_to_regular_id;
    static std::vector<int> regular_id_to_num_var;

    bool replace_if_less(std::vector<container_int> &values, std::vector<ap_float> &num_values) const;
    bool replace_if_less(PackedStateBin *buffer, std::vector<ap_float> &num_values) const;

private:
    int* value;
    int* inverse_value;
    std::vector<int> vars_affected;
    std::vector<bool> affected;
    std::vector<int> num_vars_affected;
    std::vector<bool> num_affected;
    // Need to keep the connection between affected vars, ie which var goes into which.
    std::vector<int> from_vars;
    std::vector<int> from_num_vars;
    // Affected vars by cycles
    std::vector<std::vector<int> > affected_vars_cycles;
    std::vector<std::vector<int> > affected_num_vars_cycles;

    void set_affected(int ind, int val);
    bool is_numeric(int ind) const { return ind >= dom_sum_num_var; }
    std::pair<int, int> get_new_var_val_by_old_var_val(int var, int value) const;
    int get_new_num_var_by_old_num_var(int var) const;

    void finalize();
    void _allocate();
    void _deallocate();
    void _copy_value_from_permutation(const Permutation&);
    void _inverse_value_from_permutation(const Permutation &perm);
};

#endif
