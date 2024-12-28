#ifndef UTILS_TOKENISE
#define UTILS_TOKENISE

#include <regex>
#include <string>
#include <vector>

namespace utils {
  std::vector<std::string> tokenise(const std::string &str, const char &delim) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string s;

    while (std::getline(iss, s, delim)) {
      tokens.push_back(s);
    }

    return tokens;
  }
}  // namespace utils

#endif  // UTILS_TOKENISE
