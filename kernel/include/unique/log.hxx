#ifndef LIBK_UNIQUE_LOG_HXX
#define LIBK_UNIQUE_LOG_HXX

#include <sys/log.hxx>

#define LOG(args...)                               \
    do {                                           \
        Sys::log(get_logger_prefix(), ": ", args); \
    } while (false);

#endif
