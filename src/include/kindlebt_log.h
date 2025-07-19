#ifndef KINDLE_BT_LOG_H
#define KINDLE_BT_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

void kindlebt_set_log_level(int level);
char* append_to_buffer(char* buf, size_t* size, size_t* offset, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // KINDLE_BT_LOG_H
