# logstat

Fast filter and aggregator for logs by a datetime range and HTTP path segments. It allows to effectively "zoom" huge unstructured logs thus enabling focused work within precise time ranges (up to 1 second). This makes further processing of logs with standard UNIX tools manyfold faster and more productive.

`logstat` is snappier than `grep -E` because it uses optimisations specifically tailored for a date filtering notably `fseek` binary search based on timestamp ranges whereas `grep` reads raw data into a large buffer and then searches the buffer using Boyer-Moore algorithm.

Normally it takes form 5 to 10 milliseconds to process a 4GB log file with this tool. Filtered output is then printed to `stdout` hence `log_stats` is easily pipeable and can be utilised with `grep`, `awk`, `tee`, `sort`, `uniq`, `parallel` and whatnot. Having a fast way to produce a small window out of enormous unstructured multi-GBs log files is an indispensable timesaver.

*Note:* This tool can't be used without datetime ranges. Futhermore it is effective only on large 1GB+ noisy log files and narrow time ranges otherwise `grep -E` is recommended and will be faster.

Compatible with C++14 and verified on Clang 10.x and GCC `g++` 6.x compilers under Linux and macOS. Doesn't use any 3-rd party dependencies.

## Extensibility

This tool is easily extendable so that to support any unstructured or structured log formats. Presently only CouchDB log format is supported. In order to add support for another log format you should inherit your custom parser class (`NginxLogParser`, `HAProxyLogParser`, et cetera) from a `LogParser` and override `ParseLog()` member function with your own implementation which shall initialise a `Log` POD type from a raw vector of log tokens. `Log` data class contains fields to represent the most commonly logged entries. However in case something is missing for your particular system you might want to extend this POD type. After this merely initialise a new parser class and pass its instance to a `logstat::ProcessLogs()` function within `main`.


## Usage

```bash
make
cp bin/logstat /usr/bin # or /usr/local/bin
logstat -p /var/log/couchdb/couchdb.log -b 2019-12-12T09:27:28 -e 2019-12-12T09:27:29
```

Help message is self-explanatory:
```
Usage: logstat [-p /path] [-b datetime] [-e datetime]
Options are:
[-g] [-H] [-n] [-s N] [-l N] [-h]

datetime must be in format: yyyy-mm-ddThh:mm:ss
-p, --path      filesystem path to a log file
-b, --begin     begin of the datetime range to process logs
-e, --end       end of the datetime to process logs
-g, --group     group and count requests with the same url
-H, --http      print only HTTP request logs
-n, --no-query  group and count requests ignoring query string
-s, --segment   group and count requests based on the N-th path segment of the url
-l, --limit     skip urls which were called less than N times per a given time range
-h, --help      print help message

```
