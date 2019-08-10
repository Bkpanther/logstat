// Copyright (c) 2019 Oleksandr Karaberov. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cmath>


namespace log_utils {

enum class LogSeverityLevel {
    UNDEF,
    DEBUG,
    INFO,
    NOTICE,
    WARNING,
    ERROR,
    CRITICAL,
    ALERT,
    EMERGENCY
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
        return LogSeverityLevel::DEBUG;
    }
    else if (log.compare("[info]") == 0) {
        return LogSeverityLevel::INFO;
    }
    else if (log.compare("[notice]") == 0) {
        return LogSeverityLevel::NOTICE;
    }
    else if (log.compare("[warning]") == 0) {
        return LogSeverityLevel::WARNING;
    }
    else if (log.compare("[error]") == 0 || log.compare("[err]") == 0) {
        return LogSeverityLevel::ERROR;
    }
    else if (log.compare("[critical]") == 0 || log.compare("[crit]") == 0) {
        return LogSeverityLevel::CRITICAL;
    }
    else if (log.compare("[alert]") == 0) {
        return LogSeverityLevel::ALERT;
    }
    else if (log.compare("[emergency]") == 0 || log.compare("[emerg]") == 0 || log.compare("[panic]") == 0) {
        return LogSeverityLevel::EMERGENCY;
    }
    return LogSeverityLevel::UNDEF;
}

inline HTTPMethod GetHTTPMethod(const std::string &log) {
    if (log.compare("GET") == 0) {
        return HTTPMethod::GET;
    }
    else if (log.compare("PUT") == 0) {
        return HTTPMethod::PUT;
    }
    else if (log.compare("POST") == 0) {
        return HTTPMethod::POST;
    }
    else if (log.compare("HEAD") == 0) {
        return HTTPMethod::HEAD;
    }
    else if (log.compare("DELETE") == 0) {
        return HTTPMethod::DELETE;
    }
    else if (log.compare("PATCH") == 0) {
        return HTTPMethod::PATCH;
    }
    else if (log.compare("COPY") == 0) {
        return HTTPMethod::COPY;
    }
    else if (log.compare("CONNECT") == 0) {
        return HTTPMethod::CONNECT;
    }
    else if (log.compare("TRACE") == 0) {
        return HTTPMethod::TRACE;
    }
    else if (log.compare("OPTIONS") == 0) {
        return HTTPMethod::OPTIONS;
    }
    return HTTPMethod::UNDEF_ERROR;
}

inline time_t GetEpoch(const std::string &datestr, const char * format) noexcept {
        std::istringstream iss;
        iss.str(datestr);
        std::tm tm = {0};
        iss >> std::get_time(&tm, format);
        return mktime(&tm);
    }

} // namespace log_utils