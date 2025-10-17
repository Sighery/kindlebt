#ifndef KINDLE_BT_LOG_H
#define KINDLE_BT_LOG_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup KINDLEBT_PUBLIC_API
 * @{
 */

/**
 * @brief Log level for \c log.c
 */
typedef enum {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
} log_level_t;

/**
 * @brief Set the log level for \c log.c
 *
 * @remark We use <a href="https://github.com/rxi/log.c">log.c</a> for logging.
 *
 * If this function is not called, a default \c LOG_LEVEL_TRACE is assumed.
 */
void kindlebt_set_log_level(log_level_t level);

/** @} */ // KINDLEBT_PUBLIC_API

char* append_to_buffer(char* buf, size_t* size, size_t* offset, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // KINDLE_BT_LOG_H
