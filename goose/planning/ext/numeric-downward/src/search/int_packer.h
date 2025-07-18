#ifndef INT_PACKER_H
#define INT_PACKER_H

#include "globals.h" // typedef of ap_float and container_int
#include <vector>

/*
  Utility class to pack lots of unsigned integers (called "variables"
  in the code below) with a small domain {0, ..., range - 1}
  tightly into memory. This works like a bitfield except that the
  fields and sizes don't need to be known at compile time.

  For example, if we have 40 binary variables and 20 variables with
  range 4, storing them would theoretically require at least 80 bits,
  and this class would pack them into 12 bytes (three 4-byte "bins").

  Uses a greedy bin-packing strategy to pack the variables, which
  should be close to optimal in most cases. (See code comments for
  details.)
*/

class IntPacker {
    class VariableInfo;

    std::vector<VariableInfo> var_infos;
    int num_bins;

    int pack_one_bin(const std::vector<container_int> &ranges,
                     std::vector<std::vector<int> > &bits_to_vars);
    void pack_bins(const std::vector<container_int> &ranges);
public:
    typedef container_int Bin; /* container_int is an unsigned integer type which is either 32 bit or 64 bit, according to options (--precision flag) */

    /*
      The constructor takes the range for each variable. The domain of
      variable i is {0, ..., ranges[i] - 1}. Because we are using signed
      ints for the ranges (and generally for the values of variables),
      a variable can take up at most 31 bits if int is 32-bit.
    */
    explicit IntPacker(const std::vector<container_int> &ranges);
    ~IntPacker();

    container_int get(const Bin *buffer, int var) const;
    ap_float getDouble(const Bin *buffer, int var) const;

    void set(Bin *buffer, int var, container_int value) const;
    void setDouble(Bin *buffer, int var, ap_float value) const;

    int get_num_bins() const {return num_bins; }
    std::size_t get_bin_size_in_bytes() const {return sizeof(Bin); }

    container_int packDouble(ap_float plainDouble) const;
    ap_float unpackDouble(container_int packedDouble) const;

};

#endif
