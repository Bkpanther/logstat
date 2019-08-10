// Copyright (c) 2019 Oleksandr Karaberov. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <string>

namespace cmd {

static char const * kDefaultLogFilePath = "/var/log/";

struct CmdOptions {
    std::string filepath = "";
    std::string start = "";
    std::string end;
    int skip_n_calls = 0;
    int split_on = 0;
    bool group = false;
    bool silence = false;
    bool no_query_params = false;
};

inline CmdOptions ParseCmdOptions(int argc, char **argv) {
    auto options = CmdOptions();
    const std::string help1 = "Usage: logstat [--name file] [--start yyyy-mm-ddThh:mm:ss] [--end yyyy-mm-ddThh:mm:ss]\n";
    const std::string help2 = "Options are:\n";
    const std::string help3 = "[--g] [--s] [--q] [--n N] [--i N]\n\n";
    const std::string help4 = "--g  group and count requests with the same url\n";
    const std::string help5 = "--s  don't print non HTTP request logs\n";
    const std::string help6 = "--q  group and count requests with the same url and ignore query string\n";
    const std::string help7 = "--n  group and count requests based on the N-th path segment of the url\n";
    const std::string help8 = "--i  skip urls which were called less than N times per a given time range\n";
    const std::string help_msg = help1 + help2 + help3 + help4 + help5 + help6 + help7 + help8;
    if (argc < 7) {
        std::cout << help_msg;
        return {}; 
    }
    const std::vector<std::string> cmd_args(argv + 1, argv + argc);
    if (cmd_args.at(0).compare("--name") == 0) {
        const std::string log_file_name(cmd_args.at(1));
        options.filepath = kDefaultLogFilePath + log_file_name;
    }
    if (cmd_args.at(2).compare("--start") == 0) {
        options.start = cmd_args.at(3);
    } 
    if (cmd_args.at(4).compare("--end") == 0) {
        options.end = cmd_args.at(5);
    }
    if (std::find(cmd_args.begin(), cmd_args.end(), "--g") != cmd_args.end()) {
        options.group = true;
    }
    if (std::find(cmd_args.begin(), cmd_args.end(), "--s") != cmd_args.end()) {
        options.silence = true;
    }
    if (std::find(cmd_args.begin(), cmd_args.end(), "--q") != cmd_args.end()) {
        options.no_query_params = true;
    }
    const auto skip_n_it = std::find(cmd_args.begin(), cmd_args.end(), "--i");
    if (skip_n_it != cmd_args.end()) {
        const auto skip_param_it = std::next(skip_n_it);
        if (skip_param_it != cmd_args.end()) {
            try {
                options.skip_n_calls = std::stoi(*skip_param_it);
            } 
            catch (...) {
                std::cout << "Invalid number of calls for --i argument" << '\n';
                std::cout << help_msg;
                return {};
                }
        } 
        else {
            std::cout << "Missing number of calls for --i argument" << '\n';
            std::cout << help_msg;
            return {};
        }
    }
    const auto split_it = std::find(cmd_args.begin(), cmd_args.end(), "--n");
    if (split_it != cmd_args.end()) {
        const auto split_param_it = std::next(split_it);
        if (split_param_it != cmd_args.end()) {
            try {
                options.split_on = std::stoi(*split_param_it);
            } 
            catch (...) {
                std::cout << "Invalid path segment index for --n argument" << '\n';
                std::cout << help_msg;
                return {};
            }
        } 
        else {
            std::cout << "Missing path segment index for --n argument" << '\n';
            std::cout << help_msg;
            return {};
        }
    }
    return options;
}
}