# logstat

Fast log filter by a date range. It allows to effectively "zoom" huge unstructured logs thus enabling focused work within precise time ranges (up to 1 second). This makes querying logs with standard UNIX tools manyfold faster.

`logstat` is snappier than `grep -E` because it uses optimisations specifically tailored for a date filtering notably `fseek`s on timestamp ranges whereas `grep` reads raw data into a large buffer and then searches the buffer using Boyer-Moore algorithm.

Normally it takes 5-10ms to process a 4GB log file with this tool. Filtered output is printed to `stdout` hence `log_stats` is easily pipeable and can be utilised with `grep`, `awk`, `tee`, `sed`, `sort`, `xargs`, `uniq -c` and so on as complements. Having a fast way to produce a small window out of enormous unstructured multi-GBs logs is an indispensable timesaver.

*Note:* This tool can't  be used without datetime ranges. Futhermore it is effective only on large 1GB+ log files otherwise `grep -E` + `awk` will be faster.

Compatible with C++14 and verified on Clang 10.x and GCC `g++` 6.x compilers under Debian 9 and macOS. Doesn't use any 3-rd party dependencies.

## Extensibility

This tool is easily extendable so that to support any unstructured log format. Presently only CouchDB log format is supported together with CouchDB logs time format. In order to add another log format (for instance `nginx`) you have to inherit from `LogParser` and override `ParseLog()` member function with your implementation. After this just initialise your custom `NginxLogParser` and pass it to `logstat::ProcessLogs()` function in the `main`.


# Usage

```bash
make
./bin/logstat --name couchdb/couchdb.log --start 2019-07-05T15:30:16 --end 2019-07-05T15:30:18 --g --s
```

Help message:
```
Usage: logstat [--name file] [--start yyyy-mm-ddThh:mm:ss] [--end yyyy-mm-ddThh:mm:ss]
Options are:
[--g] [--s] [--q] [--n N] [--i N]

--g  group and count requests with the same url
--s  don't print non HTTP request logs
--q  group and count requests with the same url and ignore query string
--n  group and count requests based on the N-th path segment of the url
--i  skip urls which were called less than N times per a given time range
```

Tool uses `/var/log/` by default as an absolute path to lookup logs.