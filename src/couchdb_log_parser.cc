// Copyright (c) 2019 Oleksandr Karaberov. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "couchdb_log_parser.h"

namespace logstat {

Log CouchDBLogParser::ParseLog(const std::vector<std::string> &tokens) const {
  const auto size = tokens.size();
  const auto level = size > 0 ? log_utils::GetLogLevel(tokens[0])
                              : log_utils::LogSeverityLevel::LOG_UNDEF;
  const std::time_t log_date =
      size > 1 ? log_utils::GetEpoch(tokens[1], kCouchdbLogDateFormat) : -1;
  if (level != log_utils::LogSeverityLevel::LOG_UNDEF && log_date != -1) {
    const std::string host = size > 2 ? tokens[2] : "";
    const std::string pid = size > 3 ? tokens[3] : "";
    const auto method = size > 8 ? log_utils::GetHTTPMethod(tokens[8])
                                 : log_utils::HTTPMethod::UNDEF_ERROR;
    const std::string msg_id = size > 4 ? tokens[4] : kCouchdbNonReqMarker;
    if (msg_id.compare(kCouchdbNonReqMarker) == 0) {
      Log couchdb_log = {level, log_date, host, pid};
      couchdb_log.is_generic_msg = true;
      return couchdb_log;
    } else if (method != log_utils::HTTPMethod::UNDEF_ERROR) {
      const std::string domain_name = size > 5 ? tokens[5] : "";
      const std::string ip_address = size > 6 ? tokens[6] : "";
      const std::string user = size > 7 ? tokens[7] : "";
      const std::string request_url = size > 9 ? tokens[9] : "";
      const std::string http_code = size > 10 ? tokens[10] : "";
      const int req_time = std::stoi(size > 12 ? tokens[12] : "0", nullptr);
      Log couchdb_log = {level,  log_date,    host,       pid,
                         msg_id, domain_name, ip_address, user,
                         method, request_url, http_code,  req_time};
      couchdb_log.short_req_url = request_url.substr(0, request_url.find('?'));
      couchdb_log.http_method_str = size > 8 ? tokens[8] : "";
      return couchdb_log;
    } else {
      Log couchdb_log = {level, log_date, host, pid};
      couchdb_log.is_stacktrace = true;
      return couchdb_log;
    }
  }
  return {};
};

}  // namespace logstat
