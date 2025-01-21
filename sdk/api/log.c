#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "ch_log.h"
#include "usb.h"
#include "ch_spinlock.h"

static volatile ch_log_level_t g_master_log_level = CH_LOG_VERBOSE;
static volatile ch_log_mode_t g_master_log_mode = CH_LOG_UART;

static inline bool should_output(ch_log_level_t level_for_message)
{
    return level_for_message <= g_master_log_level;
}

void ch_set_log_level(ch_log_level_t level)
{
    g_master_log_level = level;
}

ch_log_level_t ch_log_get_level_master(void)
{
    return g_master_log_level;
}

void ch_set_log_mode(ch_log_mode_t level)
{
    g_master_log_mode = level;
}

ch_log_mode_t ch_get_log_mode(void)
{
    return g_master_log_mode;
}

void ch_log_writev(ch_log_level_t level,
                   const char *tag,
                   const char *format,
                   va_list args)
{
    if (!should_output(level))
    {
        return;
    }
    vprintf(format, args);
}

void ch_log_write(ch_log_level_t level,
                  const char *tag,
                  const char *format, ...)
{
    if (!should_output(level))
    {
        return;
    }

    if (CH_LOG_UART == ch_get_log_mode())
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
    else
    {
        va_list arg;
        int len;
        va_start(arg, format);
        len = vsnprintf(NULL, 0, format, arg); // Get required buffer size
        va_end(arg);

        if (len)
        {
            char data[len];
            va_start(arg, format);
            vsnprintf(data, len + 1, format, arg); // Write formatted string into buffer, casting to char*
            va_end(arg);
            usb_write_buffer(data, len);
        }
    }
}