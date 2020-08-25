#include "log_parser.h"
namespace logstat {

static char const *kCouchdbLogDateFormat = "%Y-%m-%dT%H:%M:%S";
static const std::string kCouchdbNonReqMarker = "--------";

class CouchDBLogParser : public LogParser {
 public:
  Log ParseLog(const std::vector<std::string> &tokens) const override;
};
}
