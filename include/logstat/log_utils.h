// Copyright (c) 2019 Oleksandr Karaberov. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <sys/stat.h>

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace log_utils {

enum class LogSeverityLevel {
  LOG_UNDEF,
  LOG_DEBUG,
  LOG_INFO,
  LOG_NOTICE,
  LOG_WARNING,
  LOG_ERROR,
  LOG_CRITICAL,
  LOG_ALERT,
  LOG_EMERGENCY
};

enum class HTTPMethod {
  UNDEF_ERROR,
  GET,
  PUT,
  POST,
  HEAD,
  DELETE,
  PATCH,
  COPY,
  CONNECT,
  TRACE,
  OPTIONS
};

// Severity levels as defined by the syslog rfc5424
inline LogSeverityLevel GetLogLevel(const std::string &log) {
  if (log.compare("[debug]") == 0) {
    return LogSeverityLevel::LOG_DEBUG;
  } else if (log.compare("[info]") == 0) {
    return LogSeverityLevel::LOG_INFO;
  } else if (log.compare("[notice]") == 0) {
    return LogSeverityLevel::LOG_NOTICE;
  } else if (log.compare("[warning]") == 0) {
    return LogSeverityLevel::LOG_WARNING;
  } else if (log.compare("[error]") == 0 || log.compare("[err]") == 0) {
    return LogSeverityLevel::LOG_ERROR;
  } else if (log.compare("[critical]") == 0 || log.compare("[crit]") == 0) {
    return LogSeverityLevel::LOG_CRITICAL;
  } else if (log.compare("[alert]") == 0) {
    return LogSeverityLevel::LOG_ALERT;
  } else if (log.compare("[emergency]") == 0 || log.compare("[emerg]") == 0 ||
             log.compare("[panic]") == 0) {
    return LogSeverityLevel::LOG_EMERGENCY;
  }
  return LogSeverityLevel::LOG_UNDEF;
}

inline HTTPMethod GetHTTPMethod(const std::string &log) {
  if (log.compare("GET") == 0) {
    return HTTPMethod::GET;
  } else if (log.compare("PUT") == 0) {
    return HTTPMethod::PUT;
  } else if (log.compare("POST") == 0) {
    return HTTPMethod::POST;
  } else if (log.compare("HEAD") == 0) {
    return HTTPMethod::HEAD;
  } else if (log.compare("DELETE") == 0) {
    return HTTPMethod::DELETE;
  } else if (log.compare("PATCH") == 0) {
    return HTTPMethod::PATCH;
  } else if (log.compare("COPY") == 0) {
    return HTTPMethod::COPY;
  } else if (log.compare("CONNECT") == 0) {
    return HTTPMethod::CONNECT;
  } else if (log.compare("TRACE") == 0) {
    return HTTPMethod::TRACE;
  } else if (log.compare("OPTIONS") == 0) {
    return HTTPMethod::OPTIONS;
  }
  return HTTPMethod::UNDEF_ERROR;
}

inline time_t GetEpoch(const std::string &datestr,
                       const char *format) noexcept {
  std::istringstream iss;
  iss.str(datestr);
  std::tm tm = {0};
  iss >> std::get_time(&tm, format);
  return mktime(&tm);
}

inline size_t FileSize(const std::string &filepath) noexcept {
  // Better way to get file size without seeking to the end, tellg() and then
  // going back Moreover tellg() does not report the size of the file, nor the
  // offset from the beginning in bytes, it just reports a token value which is
  // a current position in the stream:
  // https://en.cppreference.com/w/cpp/io/basic_istream/tellg
  struct stat filestatus;
  stat(filepath.data(), &filestatus);
  return filestatus.st_size;
}

}  // namespace log_utils
