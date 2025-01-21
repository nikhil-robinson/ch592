#ifndef __CH_LOG_H__
#define __CH_LOG_H__


#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "inttypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CH_LOG_NONE,       /*!< No log output */
    CH_LOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
    CH_LOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
    CH_LOG_INFO,       /*!< Information messages which describe normal flow of events */
    CH_LOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    CH_LOG_VERBOSE     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
} ch_log_level_t;


typedef enum {
    CH_LOG_UART,
    CH_LOG_USB,
}ch_log_mode_t;

void ch_set_log_level(ch_log_level_t level);
ch_log_level_t ch_log_get_level_master(void);
void ch_set_log_mode(ch_log_mode_t level);
ch_log_mode_t ch_get_log_mode(void);
void ch_log_write(ch_log_level_t level,const char *tag,const char *format, ...);
void ch_log_buffer_hex_internal(const char *tag, const void *buffer, uint16_t buff_len,ch_log_level_t log_level);
void ch_log_buffer_hexdump_internal(const char *tag, const void *buffer, uint16_t buff_len, ch_log_level_t log_level);
void ch_log_buffer_char_internal(const char *tag, const void *buffer, uint16_t buff_len, ch_log_level_t log_level);

#define LOG_COLOR_BLACK   "30"
#define LOG_COLOR_RED     "31"
#define LOG_COLOR_GREEN   "32"
#define LOG_COLOR_BROWN   "33"
#define LOG_COLOR_BLUE    "34"
#define LOG_COLOR_PURPLE  "35"
#define LOG_COLOR_CYAN    "36"
#define LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define LOG_BOLD(COLOR)   "\033[1;" COLOR "m"
#define LOG_RESET_COLOR   "\033[0m"
#define LOG_COLOR_E       LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W       LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I       LOG_COLOR(LOG_COLOR_GREEN)
#define LOG_COLOR_D
#define LOG_COLOR_V

#define LOG_LOCAL_LEVEL  CH_LOG_VERBOSE


#define ENABLE_COLOURS 0

#if ENABLE_COLOURS
#define LOG_FORMAT(letter, format)  LOG_COLOR_ ## letter #letter " (%" PRIu32 ") %s: " format LOG_RESET_COLOR "\n"
#else
#define LOG_FORMAT(letter, format)   "(%s_%s) :" format"\r\n"
#endif

#define CH_LOG_LEVEL(level, tag, format, ...) do {                     \
        if (level==CH_LOG_ERROR )          { ch_log_write(CH_LOG_ERROR,      tag, LOG_FORMAT(E, format), tag,"ERR", ##__VA_ARGS__); } \
        else if (level==CH_LOG_WARN )      { ch_log_write(CH_LOG_WARN,       tag, LOG_FORMAT(W, format), tag,"WAR", ##__VA_ARGS__); } \
        else if (level==CH_LOG_DEBUG )     { ch_log_write(CH_LOG_DEBUG,      tag, LOG_FORMAT(D, format), tag,"DEB", ##__VA_ARGS__); } \
        else if (level==CH_LOG_VERBOSE )   { ch_log_write(CH_LOG_VERBOSE,    tag, LOG_FORMAT(V, format), tag,"VER", ##__VA_ARGS__); } \
        else                                { ch_log_write(CH_LOG_INFO,      tag, LOG_FORMAT(I, format), tag,"INF", ##__VA_ARGS__); } \
    } while(0)



#define CH_LOG_LEVEL_LOCAL(level, tag, format, ...) do {               \
        if  (ch_log_get_level_master() >= level) CH_LOG_LEVEL(level, tag, format, ##__VA_ARGS__); \
    } while(0)


#define CH_LOGE( tag, format, ... ) CH_LOG_LEVEL_LOCAL(CH_LOG_ERROR,   tag, format, ##__VA_ARGS__)
#define CH_LOGW( tag, format, ... ) CH_LOG_LEVEL_LOCAL(CH_LOG_WARN,    tag, format, ##__VA_ARGS__)
#define CH_LOGI( tag, format, ... ) CH_LOG_LEVEL_LOCAL(CH_LOG_INFO,    tag, format, ##__VA_ARGS__)
#define CH_LOGD( tag, format, ... ) CH_LOG_LEVEL_LOCAL(CH_LOG_DEBUG,   tag, format, ##__VA_ARGS__)
#define CH_LOGV( tag, format, ... ) CH_LOG_LEVEL_LOCAL(CH_LOG_VERBOSE, tag, format, ##__VA_ARGS__)



#define CH_LOG_BUFFER_HEX_LEVEL( tag, buffer, buff_len, level ) \
    do {\
        if ( ch_log_get_level_master() >= (level) ) { \
            ch_log_buffer_hex_internal( tag, buffer, buff_len, level ); \
        } \
    } while(0)

/**
 * @brief Log a buffer of characters at specified level, separated into 16 bytes each line. Buffer should contain only printable characters.
 *
 * @param  tag      description tag
 * @param  buffer   Pointer to the buffer array
 * @param  buff_len length of buffer in bytes
 * @param  level    level of the log
 *
 */
#define CH_LOG_BUFFER_CHAR_LEVEL( tag, buffer, buff_len, level ) \
    do {\
        if ( ch_log_get_level_master() >= (level) ) { \
            ch_log_buffer_char_internal( tag, buffer, buff_len, level ); \
        } \
    } while(0)


#define CH_LOG_BUFFER_HEXDUMP( tag, buffer, buff_len, level ) \
    do { \
        if ( ch_log_get_level_master() >= (level) ) { \
            ch_log_buffer_hexdump_internal( tag, buffer, buff_len, level); \
        } \
    } while(0)


#define CH_LOG_BUFFER_HEX(tag, buffer, buff_len) \
    do { \
        if (ch_log_get_level_master() >= CH_LOG_INFO) { \
            CH_LOG_BUFFER_HEX_LEVEL( tag, buffer, buff_len, CH_LOG_INFO ); \
        }\
    } while(0)


#define CH_LOG_BUFFER_CHAR(tag, buffer, buff_len) \
    do { \
        if (ch_log_get_level_master() >= CH_LOG_INFO) { \
            CH_LOG_BUFFER_CHAR_LEVEL( tag, buffer, buff_len, CH_LOG_INFO ); \
        }\
    } while(0)


#ifdef __cplusplus
}
#endif


#endif /* __CH_LOG_H__ */
