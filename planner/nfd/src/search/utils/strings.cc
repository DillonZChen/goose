#include "math.h"

#include <string>

bool contains_substr(const std::string &str, const std::string &substr) {
  return str.find(substr) != std::string::npos;
}
