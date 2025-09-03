#include "int_packer.h"
#include "globals.h" // required for the number of numeric Variables

#include<iostream>
#include <limits>
#include <cassert>
using namespace std;


static const int BITS_PER_BIN = sizeof(IntPacker::Bin) * 8;


static IntPacker::Bin get_bit_mask(int from, int to) {
    // Return mask with all bits in the range [from, to) set to 1.
    assert(from >= 0);
    assert(to >= from);
    assert(to <= BITS_PER_BIN);
    int length = to - from;
    if (length == BITS_PER_BIN) {
        // 1U << BITS_PER_BIN has undefined behaviour in C++; e.g.
        // 1U << 32 == 1 (not 0) on 32-bit Intel platforms. Hence this
        // special case.
        assert(from == 0 && to == BITS_PER_BIN);
        return ~IntPacker::Bin(0);
    } else {
        return ((IntPacker::Bin(1) << length) - 1) << from;
    }
}

static int get_bit_size_for_range(container_int range) {
	if (range == numeric_limits<container_int>::max())
		return BITS_PER_BIN; // HACK for doubles (10000000000 is never > 11111111111111) as a 65 bit 100000000000 is a 000000000000 on 64 bit representation
    int num_bits = 0;
    while ((1ULL << num_bits) < static_cast<unsigned int>(range))
        ++num_bits;
    return num_bits;
}

class IntPacker::VariableInfo {
    container_int range;
    int bin_index;
    int shift;
    Bin read_mask;
    Bin clear_mask;
public:
    VariableInfo(container_int range_, int bin_index_, int shift_)
        : range(range_),
          bin_index(bin_index_),
          shift(shift_) {
        int bit_size = get_bit_size_for_range(range);
        read_mask = get_bit_mask(shift, shift + bit_size);
        clear_mask = ~read_mask;
    }

    VariableInfo()
        : bin_index(-1), shift(0), read_mask(0), clear_mask(0) {
        // Default constructor needed for resize() in pack_bins.
    }

    ~VariableInfo() {
    }

    container_int get(const Bin *buffer) const {
//    	cout << "about to return the variable in Bin #" << bin_index << " with the readmask " << read_mask << endl;
//    	cout << "Im Buffer befindet sich folgendes: " << buffer[bin_index] << endl;
//    	cout << "verundet ist das dann : " <<(buffer[bin_index] & read_mask) << endl;
//    	cout << "und rübergeshiftet das finale ergebnis = " << ((buffer[bin_index] & read_mask) >> shift) << endl;
        return (buffer[bin_index] & read_mask) >> shift;
    }

    void set(Bin *buffer, container_int value) const {
//        assert(value >= 0 && value < range);
//    	if(value >= range) cout <<"Assertion fails, value = " << value << " while range = " << range << endl;
        assert(value <= range);
        Bin &bin = buffer[bin_index];
        bin = (bin & clear_mask) | (value << shift);
    }
};


IntPacker::IntPacker(const vector<container_int> &ranges)
    : num_bins(0) {
    pack_bins(ranges);
}

IntPacker::~IntPacker() {
}

container_int IntPacker::get(const Bin *buffer, int var) const {
//	container_int result = var_infos[var].get(buffer);
//	cout << "IntPacker gets the following result : " << result << endl;
//	return result;
    return var_infos[var].get(buffer);
}

ap_float IntPacker::getDouble(const Bin *buffer, int var) const {
	assert (var >= (int) g_initial_state_data.size()); // numeric variables are stored after logic variables
	container_int packedDouble = var_infos[var].get(buffer);
//	if (DEBUG) cout << " packed double = " << packedDouble << " unpacked: " << unpackDouble(packedDouble) << endl;
    return unpackDouble(packedDouble);
}


void IntPacker::set(Bin *buffer, int var, container_int value) const {
//	if (DEBUG) cout << "set buffer var " << var << " to value " << value << endl;
    assert((int) var_infos.size() > var);
	var_infos[var].set(buffer, value);
}

void IntPacker::setDouble(Bin *buffer, int var, ap_float value) const {
	assert(var >= (int) g_initial_state_data.size()); // numeric vars are stored after regular ones
//	if (DEBUG) cout << "set numeric buffer var " << var << " to value " << value;
	container_int packedDouble = packDouble(value);
//	if (DEBUG) cout << " packed double = " << packedDouble << endl;
    var_infos[var].set(buffer, packedDouble);
}

void IntPacker::pack_bins(const vector<container_int> &ranges) {
    assert(var_infos.empty());

    int num_vars = ranges.size();
    var_infos.resize(num_vars);

    // bits_to_vars[k] contains all variables that require exactly k
    // bits to encode. Once a variable is packed into a bin, it is
    // removed from this index.
    // Loop over the variables in reverse order to prefer variables with
    // low indices in case of ties. This might increase cache-locality.
    vector<vector<int> > bits_to_vars(BITS_PER_BIN + 1);
    for (int var = num_vars - 1; var >= 0; --var) {
        int bits = get_bit_size_for_range(ranges[var]);
        assert(bits <= BITS_PER_BIN);
        bits_to_vars[bits].push_back(var);
    }

    int packed_vars = 0;
    while (packed_vars != num_vars)
        packed_vars += pack_one_bin(ranges, bits_to_vars);

}

int IntPacker::pack_one_bin(const vector<container_int> &ranges,
                            vector<vector<int> > &bits_to_vars) {
    // Returns the number of variables added to the bin. We pack each
    // bin with a greedy strategy, always adding the largest variable
    // that still fits.

    ++num_bins;
    int bin_index = num_bins - 1;
    int used_bits = 0;
    int num_vars_in_bin = 0;

    while (true) {
        // Determine size of largest variable that still fits into the bin.
        int bits = BITS_PER_BIN - used_bits;
        while (bits > 0 && bits_to_vars[bits].empty())
            --bits;

        if (bits == 0) {
            // No more variables fit into the bin.
            // (This also happens when all variables have been packed.)
            return num_vars_in_bin;
        }

        // We can pack another variable of size bits into the current bin.
        // Remove the variable from bits_to_vars and add it to the bin.
        vector<int> &best_fit_vars = bits_to_vars[bits];
        int var = best_fit_vars.back();
        best_fit_vars.pop_back();

        var_infos[var] = VariableInfo(ranges[var], bin_index, used_bits);
        used_bits += bits;
        ++num_vars_in_bin;
    }
}

container_int IntPacker::packDouble(ap_float plainDouble) const {
	ap_float* pointer = &plainDouble;
	return *reinterpret_cast<container_int*>(pointer);
}

ap_float IntPacker::unpackDouble(container_int packedDouble) const {
	container_int* pointer = &packedDouble;
	return *reinterpret_cast<ap_float*>(pointer);
}
