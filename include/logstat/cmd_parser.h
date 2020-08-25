
#include <getopt.h>

#include <iostream>
#include <string>
#include <vector>

namespace logstat {

struct CmdOptions {
  std::string filepath = {};
  std::string start = {};
  std::string end = {};
};


inline CmdOptions ParseCmdOptions(int argc, char **argv) {
  CmdOptions options = {};
  int c;
  const char *const short_opts = "p:b:e";
  const option long_opts[] = {{"path", required_argument, nullptr, 'p'},
                              {"begin", required_argument, nullptr, 'b'},
                              {"end", required_argument, nullptr, 'e'}};
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
}
