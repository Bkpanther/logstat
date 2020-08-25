#include "couchdb_log_parser.h"
#include "logstat.h"

int main(int argc, char **argv) {
  auto options = logstat::ParseCmdOptions(argc, argv);
  if (!options.filepath.empty() && !options.start.empty()) {
    auto parser = logstat::CouchDBLogParser();
    logstat::ProcessLogs(options, parser);
  }
  return 0;
}
