// Copyright (c) 2019 Oleksandr Karaberov. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <cstring>
#include <ctime>
#include <vector>

#include "log_utils.h"

namespace logstat {

struct Log {
 public:
  log_utils::LogSeverityLevel log_level = {};
  std::time_t date = -1;
  std::string host;
  std::string pid;
  std::string msg_id;
  std::string domain_name;
  std::string ip_address;
  std::string user;
  log_utils::HTTPMethod http_method;
  std::string request_url;
  std::string http_code;
  int req_time = 0;
  std::string short_req_url;
  std::string http_method_str;
  bool is_stacktrace = false;
  bool is_generic_msg = false;
};

struct LogParser {
 public:
  virtual std::vector<std::string> TokeniseLog(const std::string &log) const {
    std::vector<std::string> tokens;
    tokens.reserve(30);
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
