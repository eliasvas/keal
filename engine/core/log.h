#ifndef NLOG_H
#define NLOG_H
#include "base/base_inc.h"
// TODO -- make it so that there is a configurable max log level and lesser messages are not printed

typedef enum nLogLevel nLogLevel;
enum nLogLevel {
    NLOG_LEVEL_DEBUG,
    NLOG_LEVEL_INFO,
    NLOG_LEVEL_WARNING,
    NLOG_LEVEL_ERROR,
};
void nlog(nLogLevel level, const char* format, ...);

#define NLOG_ERR(format, ...) nlog(NLOG_LEVEL_ERROR, format, ##__VA_ARGS__)
#define NLOG_DBG(format, ...) nlog(NLOG_LEVEL_DEBUG, format, ##__VA_ARGS__)
#define NLOG_INFO(format, ...) nlog(NLOG_LEVEL_INFO, format, ##__VA_ARGS__)
#define NLOG_WARN(format, ...) nlog(NLOG_LEVEL_WARNING, format, ##__VA_ARGS__)

#endif