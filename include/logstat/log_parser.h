// Copyright (c) 2019 Oleksandr Karaberov. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

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

  // MARK: Optional:
  log_utils::LogSeverityLevel log_level = {};

  // Network related
  int req_time = 0;
  unsigned long long bytes_sent;
  unsigned long long bytes_received;
  std::string host;
  std::string upstream_host;
  std::string domain_name;
  std::string ip_address;

  // HTTP related
  std::string http_referer;
  log_utils::HTTPMethod http_method = {};
  std::string request_url;
  std::string http_code;
  std::string short_req_url;
  std::string http_method_str;

  // CouchDB
  std::string pid;
  std::string msg_id;
  std::string user;
  bool is_stacktrace = false;
  bool is_generic_msg = false;

  // Nginx
  unsigned int connection_requests = 0;
  unsigned long request_length = 0;
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
