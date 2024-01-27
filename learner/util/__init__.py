def pair_to_index_map(n, i, j):
    # map pair where 0 <= i < j < n to vec index
    return j - i - 1 + (i * n) - (i * (i + 1)) / 2
