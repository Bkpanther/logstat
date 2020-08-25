#include "couchdb_log_parser.h"

namespace logstat {

 enum LogToken {
  DATE_TOK,
  TIME_TOK
};
//Basic templeate structure
template <typename TokType>
inline TokType ExtractToken(const LogToken index, const std::vector<std::string> &tokens, TokType def_value) 
  {
      return tokens.size() > index ? tokens[index] : def_value;
  };


template <>
inline std::time_t ExtractToken<std::time_t>(
    const LogToken index, const std::vector<std::string> &tokens,
    std::time_t def_value) {
  return tokens.size() > index
             ? log_utils::GetEpoch(tokens[index], kCouchdbLogDateFormat)
             : def_value;
};

Log CouchDBLogParser::ParseLog(const std::vector<std::string> &tokens) const 
{
  const auto log_date = ExtractToken<std::time_t>(DATE_TOK, tokens, -1);
  Log couchdb_log = {};
  return couchdb_log;
};

}  // namespace logstat
