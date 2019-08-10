// Copyright (c) 2019 Oleksandr Karaberov. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

/*********************************************************
*  Log filter and aggregator by a date range (10ms to process a 4GB log file and stdout all the matching logs) 
*  Not recommended to be used without small datetime ranges (in this case grep+awk might be more suitable and faster) 
*/

#include "couchdb_log_parser.h"
#include "logstat.h"

namespace logstat {

log_parsers::Log FetchDateByOffset(std::ifstream &infile, const std::streampos offset, 
                                   const log_parsers::LogParser &parser) noexcept {
    infile.seekg(offset);
    std::string fin_date;
    log_parsers::Log log; 
    for(;std::getline(infile, fin_date);) {
        log = parser.GetLog(fin_date);
        if (log.log_level != log_utils::LogSeverityLevel::UNDEF && log.date != -1) break;
    }
    return log;
}

std::streampos FindLogPos(std::ifstream &infile, const time_t target, 
                          const size_t file_size,
                          const log_parsers::LogParser &parser) noexcept {
    long right = file_size;
    long left = 0;
    while (left <= right) {
        long middle = static_cast<long>(std::floor((left+right)/2));
        time_t current = FetchDateByOffset(infile, middle, parser).date;
        if (std::difftime(current, target) < 0) {
            left = middle + 1;
        } 
        else if (std::difftime(current, target) > 0) {
            right = middle-1;
        }
        else return middle;
    }    
    return 0;
}

/** Production logs tend to be quite noisy and dense under heavy utilisation.
 *  This might cause multiple log messages to be logged per one second and lead to duplicate timestamps.
 *  Therefore after we found a target timestamp chances are there are duplicates which we also need */
std::streampos AdjustLogPos(std::ifstream &infile, const time_t start, 
                            std::streampos offset, 
                            const log_parsers::LogParser &parser) noexcept {
    // window size determines how many bytes to jump back in the file. 
    // Bigger values provide better performance but for the higher risk of possible range mistakes
    constexpr int adj_window_sz = 100;              
    while(std::difftime(start, FetchDateByOffset(infile, offset, parser).date) == 0) { offset -= adj_window_sz;}
    return offset;
}

void ProcessLogs(const cmd::CmdOptions &options,
                 const log_parsers::LogParser &parser) noexcept {
    const std::string filepath = options.filepath;
    std::ifstream infile(filepath, std::ios::in);
    if (!infile) {
        std::cerr << "Error opening " << filepath << ": " << strerror(errno) << '\n';
        return;
    }
    // Better way to get file size without seeking to the end, tellg() and then going back 
    // Moreover tellg() does not report the size of the file, nor the offset from the beginning in bytes, 
    // it just reports a token value which is a current position in the stream:
    // https://en.cppreference.com/w/cpp/io/basic_istream/tellg
    struct stat filestatus;
    stat(filepath.data(), &filestatus);
    const size_t file_size = filestatus.st_size;
    std::streampos logpos = 0;
    const auto start = log_utils::GetEpoch(options.start, log_parsers::kCouchdbLogDateFormat);
    logpos = FindLogPos(infile, start, file_size, parser);
    if (logpos == 0) {
        std::cerr << "Wasn't able to find a specified start time" << '\n';
        return;
    }
    logpos = AdjustLogPos(infile, start, logpos, parser);
    std::string cur_log;
    std::unordered_map<std::string, int> grouped_logs;
    const auto end = log_utils::GetEpoch(options.end, log_parsers::kCouchdbLogDateFormat);
    if (options.group) {
        // Guesstimate maximum possible number of logs in order to reserve capacity and avoid exspensive
        // rehashing and reallocating. Worst case scenario: all log messages are unique, 
        // and we have 150 log messages logged per second
        const double num_of_secs = std::difftime(end, start);
        constexpr int logs_per_sec = 150;
        const long cap_size = static_cast<long>(logs_per_sec * num_of_secs);
        grouped_logs.reserve(cap_size);
    }
    while (std::getline(infile, cur_log)) {
        const log_parsers::Log log = parser.GetLog(cur_log);
        const auto date = log.date;
        if (date != -1) {
            if (date > end) break;
            else {
                if (!options.group) {
                    std::cout << cur_log << '\n';
                } 
                else {
                    if (!options.silence && (log.is_generic_msg || log.is_stacktrace)) {
                        std::cout << cur_log << '\n';
                    } 
                    else if (!log.is_generic_msg && !log.is_stacktrace) {
                        if (options.split_on != 0) {
                            std::vector<std::string> paths;
                            paths.reserve(10);
                            std::istringstream ss(log.request_url);
                            std::string token;
                            while(std::getline(ss, token, '/')) {
                                paths.push_back(token);
                            }
                            if (paths.size() > options.split_on) {
                                const std::string rel_path = paths.at(options.split_on);
                                grouped_logs[rel_path]++; 
                            }    
                        } else {
                            const std::string req = options.no_query_params ? log.short_req_url : log.request_url;
                            const std::string url = log.http_method_str + ' ' + req;
                            grouped_logs[url]++;
                        }
                    }
                }
            }
        }
    }
    if (options.group) {
        std::cout << "\n____________________\nLogs count" << " (total unique: " << grouped_logs.size() << ")\n";
        typedef std::function<bool(std::pair<std::string, int>, std::pair<std::string, int>)> Comparator;
        const Comparator comparator = [](const std::pair<std::string, int> &fst, const std::pair<std::string, int> &snd) {
            return fst.second > snd.second;
        };
        std::vector<std::pair<std::string, int>> log_entries(grouped_logs.begin(), grouped_logs.end());
        std::sort(log_entries.begin(), log_entries.end(), comparator);
        for (const auto &element : log_entries) {
            if(options.skip_n_calls == 0) {
                const std::string msg = (element.second == 1) ? " call" : " calls";
                std::cout << element.first << " : " << element.second << msg << '\n';
            } else {
                if (element.second > options.skip_n_calls) {
                    std::cout << element.first << " : " << element.second << " calls" << '\n';
                }
            }
        }
    }
}
} // namespace log_stats



