// Copyright (c) 2019 Oleksandr Karaberov. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "log_parser.h"

namespace logstat {

static char const *kCouchdbLogDateFormat = "%Y-%m-%dT%H:%M:%S";
static char const *kCouchdbNonReqMarker = "--------";

struct CouchDBLogParser : public LogParser {
 public:
  Log ParseLog(const std::vector<std::string> &tokens) const override;
};
}  // namespace logstat
