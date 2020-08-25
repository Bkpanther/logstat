
#include <cstring>
#include <ctime>
#include <vector>

#include "log_utils.h"

namespace logstat {

// Data class to represent parsed log data
struct Log {
 public:
  // MARK: Required:
  std::time_t date = -1;

  };

// An abstract class which serves as a superclass for all concrete parser
// implementations, tailored for specific log formats

constexpr int max_tokens = 30;

class LogParser {
 public:
  virtual std::vector<std::string> TokeniseLog(const std::string &log) const {
    std::vector<std::string> tokens;
    tokens.reserve(max_tokens);
    std::istringstream ss(log);
    std::string token;
    while (std::getline(ss, token, ' ')) {
      tokens.push_back(token);
    }
    return tokens;
  };
  virtual Log ParseLog(const std::vector<std::string> &tokens) const = 0;
  virtual Log GetLog(const std::string &log) const {
    return ParseLog(TokeniseLog(log));
  }
};
}  // namespace logstat
