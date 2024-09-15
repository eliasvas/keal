#include "log.h"

#define NLOG_PRINT_FUNC_IMPL printf
#define NLOG_VPRINT_FUNC_IMPL vprintf
#define NLOG_MAX_LEN 128



const char* nlog_level_to_cstring(nLogLevel level) {
    switch (level) {
        case NLOG_LEVEL_DEBUG:   return "DEBUG";
        case NLOG_LEVEL_INFO:    return "INFO";
        case NLOG_LEVEL_WARNING: return "WARNING";
        case NLOG_LEVEL_ERROR:   return "ERROR";
        default:                 return "UNKNOWN";
    }
}

void nlog(nLogLevel level, const char* format, ...) {
    NLOG_PRINT_FUNC_IMPL("[%s] ", nlog_level_to_cstring(level));
    va_list vl;
    va_start(vl, format);
    NLOG_VPRINT_FUNC_IMPL(format, vl);
    va_end(vl);
    NLOG_PRINT_FUNC_IMPL("\n");
}