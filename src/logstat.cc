#include "logstat.h"
#include "couchdb_log_parser.h"

namespace logstat {

typedef std::function<bool(std::pair<std::string, int>,std::pair<std::string, int>)> Comparator;

Log FetchDateByOffset(std::ifstream &infile, const std::streampos offset, const LogParser &parser) noexcept {
  infile.seekg(offset);
  std::string fin_date;
  Log log;
  for (; std::getline(infile, fin_date);) {
    log = parser.GetLog(fin_date);
    if (log.log_level != log_utils::LogSeverityLevel::LOG_UNDEF &&
        log.date != -1)
      break;
  }
  return log;
}

std::streampos FindLogPos(std::ifstream &infile, const time_t target, const size_t file_size, const LogParser &parser) noexcept {
  long right = file_size;
  long left = 0;
  //using binary search to find the log positon in log file
  while (left <= right) {
    long middle = static_cast<long>(std::floor((left + right) / 2));
    time_t current = FetchDateByOffset(infile, middle, parser).date;
    if (std::difftime(current, target) < 0) {
      left = middle + 1;
    } else if (std::difftime(current, target) > 0) {
      right = middle - 1;
    } else
      return middle;
  }
  return 0;
}

std::streampos AdjustLogPos(std::ifstream &infile, const time_t start,std::streampos offset, const LogParser &parser) noexcept {
  constexpr int adj_window_sz = 100;
  while (std::difftime(start, FetchDateByOffset(infile, offset, parser).date) == 0)
  {
    offset -= adj_window_sz;
  }
  return offset;
}

std::unordered_map<std::string, unsigned long> StreamLogs(std::ifstream &infile, const LogParser &parser, const time_t start, const CmdOptions options) 
{
  std::unordered_map<std::string, unsigned long> grouped_logs;
  const auto end = log_utils::GetEpoch(options.end, kCouchdbLogDateFormat);
  if (options.group) {
    const double num_of_secs = std::difftime(end, start);
    // Guesstimate maximum possible number of logs in order to reserve capacity
    // and avoid exspensive rehashing and reallocating. Worst case scenario: all
    // log messages are unique, and we have 150 log messages logged per second
    constexpr int logs_per_sec = 150;
    const unsigned long cap_size =
        static_cast<unsigned long>(logs_per_sec * num_of_secs);
    grouped_logs.reserve(cap_size);
  }
  std::string cur_log;
  while (std::getline(infile, cur_log)) {
    const Log log = parser.GetLog(cur_log);
    const auto date = log.date;
    if (date != -1) {
      if (date > end)
        break;
      else {
        if (!options.group) {
          std::cout << cur_log << '\n';
        } else {
          if (!options.http_only && (log.is_generic_msg || log.is_stacktrace)) {
            std::cout << cur_log << '\n';
          } else if (!log.is_generic_msg && !log.is_stacktrace) {
            if (options.split_on > 0) {
              std::vector<std::string> paths;
              paths.reserve(10);
              std::istringstream ss(log.request_url);
              std::string token;
              while (std::getline(ss, token, '/')) {
                paths.push_back(token);
              }
              if (paths.size() > options.split_on) {
                const std::string rel_path = paths.at(options.split_on);
                grouped_logs[rel_path]++;
              }
            } else {
              const std::string req =
                  options.no_query_params ? log.short_req_url : log.request_url;
              const std::string url = log.http_method_str + ' ' + req;
              grouped_logs[url]++;
            }
          }
        }
      }
    }
  }
  return grouped_logs;
}

void GroupLogs(const CmdOptions options,const std::unordered_map<std::string, unsigned long> grouped_logs) {
  const auto header = "\n_________\nLogs count (total unique: ";
  std::cout << header << grouped_logs.size() << ")\n";
  const Comparator comparator = [](const std::pair<std::string, int> &fst, const std::pair<std::string, int> &snd)
  {
    return fst.second > snd.second;
  };
  std::vector<std::pair<std::string, int>> log_entries(grouped_logs.begin(),grouped_logs.end());
  std::sort(log_entries.begin(), log_entries.end(), comparator);
  for (const auto &element : log_entries) {
    if (options.limit == 0) {
      const std::string msg = (element.second == 1) ? " call" : " calls";
      std::cout << element.first << " : " << element.second << msg << '\n';
    } else {
      if (element.second > options.limit) {
        std::cout << element.first << " : " << element.second << " calls"
                  << '\n';
      }
    }
  }
}

void ProcessLogs(const CmdOptions &options, const LogParser &parser) noexcept {
  const std::string filepath = options.filepath;
  std::ifstream infile(filepath, std::ios::in);
  if (!infile) {
    std::cerr << "Error opening " << filepath << ": " << strerror(errno)
              << '\n';
    return;
  }
  std::streampos logpos = 0;
  const auto start = log_utils::GetEpoch(options.start, kCouchdbLogDateFormat);
  logpos = FindLogPos(infile, start, log_utils::FileSize(filepath), parser);
  if (logpos == 0) {
    std::cerr << "Wasn't able to find a specified start time" << '\n';
    return;
  }
  logpos = AdjustLogPos(infile, start, logpos, parser);
  auto logs = StreamLogs(infile, parser, start, options);
  if (options.group) {
    GroupLogs(options, logs);
  }
}
}  // namespace logstat
