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

log_parsers::Log FetchDateByOffset(std::ifstream &infile, const std::streampos offset, 
                                   log_parsers::LogParser &parser) noexcept;

std::streampos FindLogPos(std::ifstream &infile, const time_t target, 
                          size_t file_size,
                          log_parsers::LogParser &parser) noexcept;

std::streampos AdjustLogPos(std::ifstream &infile, const time_t start, 
                            std::streampos offset, log_parsers::LogParser &parser) noexcept;

void ProcessLogs(const cmd::CmdOptions &options,
                      log_parsers::LogParser &parser) noexcept;

}