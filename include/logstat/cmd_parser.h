// Copyright (c) 2019 Oleksandr Karaberov. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <getopt.h>

#include <iostream>
#include <string>
#include <vector>

namespace logstat {

struct CmdOptions {
  std::string filepath = {};
  std::string start = {};
  std::string end = {};
  bool group = false;
  bool http_only = false;
  bool no_query_params = false;
  int split_on = 0;
  int limit = 0;
};

inline void PrintUsage(const int status) {
  const auto help =
      "Usage: logstat [-p /path] [-b datetime] [-e datetime]\n"
      "Options are:\n"
      "[-g] [-H] [-n] [-s N] [-l N] [-h]\n\n"
      "datetime must be in format: yyyy-mm-ddThh:mm:ss\n"
      "-p, --path      filesystem path to a log file\n"
      "-b, --begin     begin of the datetime range to process logs\n"
      "-e, --end       end of the datetime to process logs\n"
      "-g, --group     group and count requests with the same url\n"
      "-H, --http      print only HTTP request logs\n"
      "-n, --no-query  group and count requests ignoring query string\n"
      "-s, --segment   group and count requests based on the N-th path "
      "segment of the url\n"
      "-l, --limit     skip urls which were called less than N times per a "
      "given time range\n"
      "-h, --help      print help message\n";
  std::cout << help;
  exit(status);
}

inline CmdOptions ParseCmdOptions(int argc, char **argv) {
  CmdOptions options = {};
  int c;
  const char *const short_opts = "p:b:e:gHns:l:h";
  const option long_opts[] = {{"path", required_argument, nullptr, 'p'},
                              {"begin", required_argument, nullptr, 'b'},
                              {"end", required_argument, nullptr, 'e'},
                              {"group", no_argument, nullptr, 'g'},
                              {"http", no_argument, nullptr, 'H'},
                              {"no-query", no_argument, nullptr, 'n'},
                              {"segment", required_argument, nullptr, 's'},
                              {"limit", required_argument, nullptr, 'l'},
                              {"help", no_argument, nullptr, 'h'},
                              {nullptr, no_argument, nullptr, 0}};
  while ((c = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1) {
    switch (c) {
      case 'p':
        options.filepath = optarg;
        break;
      case 'b':
        options.start = optarg;
        break;
      case 'e':
        options.end = optarg;
        break;
      case 'g':
        options.group = true;
        break;
      case 'H':
        options.http_only = true;
        break;
      case 'n':
        options.no_query_params = true;
        break;
      case 's':
        try {
          int index = std::stoi(optarg);
          if (index < 0) {
            std::cout << "logstat: illegal argument for --segment\n";
            PrintUsage(EXIT_FAILURE);
          }
          options.split_on = index;
          break;
        } catch (...) {
          std::cout << "logstat: illegal argument for --segment\n";
          PrintUsage(EXIT_FAILURE);
        }
      case 'l':
        try {
          options.limit = std::stoi(optarg);
          break;
        } catch (...) {
          std::cout << "logstat: illegal argument for --limit\n";
          PrintUsage(EXIT_FAILURE);
        }
      case 'h':
      case '?':
      default:
        PrintUsage(EXIT_FAILURE);
    }
  }
  if (options.filepath.empty() || options.start.empty() ||
      options.end.empty()) {
    std::cout << "logstat: missing required arguments (path, begin, end)\n";
    PrintUsage(EXIT_FAILURE);
  }
  return options;
}
}  // namespace logstat
