// Copyright (c) 2019 Oleksandr Karaberov. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "couchdb_log_parser.h"

namespace log_parsers {

Log CouchDBLogParser::ParseLog(const std::vector<std::string> &tokens) { 
    if (tokens.size() > 1) {
        const auto level = log_utils::GetLogLevel(tokens.at(0));
        const std::time_t log_date = log_utils::GetEpoch(tokens.at(1), log_parsers::kCouchdbLogDateFormat);
        if (level != log_utils::LogSeverityLevel::UNDEF && log_date != -1) {
            const std::string host = tokens.at(2);
            const std::string pid = tokens.at(3);
            const auto method = log_utils::GetHTTPMethod(tokens.at(8));
            if (tokens.at(4).compare(log_parsers::kCouchdbNonReqMarker) == 0) {
                Log couchdb_log = {level, log_date, host, pid};
                couchdb_log.is_generic_msg = true;
                return couchdb_log;
            } 
            else if (method != log_utils::HTTPMethod::UNDEF_ERROR) {
                const std::string msg_id = tokens.at(4);
                const std::string domain_name = tokens.at(5);
                const std::string ip_address = tokens.at(6);
                const std::string user = tokens.at(7);
                const std::string request_url = tokens.at(9);
                const std::string http_code = tokens.at(10);
                const int req_time = std::stoi(tokens.at(12), nullptr);
                Log couchdb_log = {level, log_date, host, pid, msg_id, domain_name, ip_address, user,
                                         method, request_url, http_code, req_time};
                couchdb_log.short_req_url = request_url.substr(0, request_url.find('?'));                         
                couchdb_log.http_method_str = tokens.at(8);                       
                return couchdb_log;
            }
            else {
                Log couchdb_log = {level, log_date, host, pid};
                couchdb_log.is_stacktrace = true;
                return couchdb_log;
            }
        }
    }
    return {};
};

} // namespace log_parsers