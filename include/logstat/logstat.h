#include <algorithm>
#include <functional>
#include <unordered_map>

#include "cmd_parser.h"

struct Log;
class LogParser;

namespace logstat {

Log FetchDateByOffset(const std::ifstream &infile, const std::streampos offset, const LogParser &parser) noexcept;

std::streampos FindLogPos(std::ifstream &infile, const time_t target, const size_t file_size, const LogParser &parser) noexcept;

std::streampos AdjustLogPos(const std::ifstream &infile, const time_t start, std::streampos offset, const LogParser &parser) noexcept;

void ProcessLogs(const CmdOptions &options, const LogParser &parser) noexcept;

}
