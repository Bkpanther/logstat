// Copyright (c) 2019 Oleksandr Karaberov. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "couchdb_log_parser.h"
#include "logstat.h"


int main(int argc, char **argv) {
    auto options = cmd::ParseCmdOptions(argc, argv);
    if (!options.filepath.empty() && !options.start.empty()) {
        auto parser = log_parsers::CouchDBLogParser();
        logstat::ProcessLogs(options, parser);
    }
    return 0;
}