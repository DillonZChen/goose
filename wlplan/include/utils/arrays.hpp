#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>

namespace py = pybind11;

namespace utils {
  template <typename T> void print_1d(const std::vector<T> &v) {
    std::cout << "[";
    for (size_t i = 0; i < v.size(); i++) {
      std::cout << v[i];
      if (i < v.size() - 1) {
        std::cout << ", ";
      }
    }
    std::cout << "]";
    std::cout << std::endl;
  }

  template <typename T> void print_2d(const std::vector<std::vector<T>> &v) {
    std::cout << "[";
    for (size_t i = 0; i < v.size(); i++) {
      if (i > 0) {
        std::cout << "  ";
      }
      print_1d(v[i]);
      if (i < v.size() - 1) {
        std::cout << "\n";
      }
    }
    std::cout << "]";
    std::cout << std::endl;
  }
}  // namespace utils

#endif  // UTILS_HPP
