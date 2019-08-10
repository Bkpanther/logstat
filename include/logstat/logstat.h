// Copyright (c) 2019 Oleksandr Karaberov. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <sys/stat.h>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include "cmd_parser.h"

class Log;

namespace logstat {

log_parsers::Log FetchDateByOffset(const std::ifstream &infile, const std::streampos offset, 
                                   const log_parsers::LogParser &parser) noexcept;

std::streampos FindLogPos(std::ifstream &infile, const time_t target, 
                          const size_t file_size,
                          const log_parsers::LogParser &parser) noexcept;

std::streampos AdjustLogPos(const std::ifstream &infile, const time_t start, 
                            std::streampos offset, const log_parsers::LogParser &parser) noexcept;

void ProcessLogs(const cmd::CmdOptions &options,
                 const log_parsers::LogParser &parser) noexcept;

}