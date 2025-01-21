#pragma once

#include "ch_err.h"
#include "ch_log.h"

#ifdef __cplusplus
extern "C" {
#endif



/**
 * Macro which can be used to check the error code. If the code is not CH_OK, it prints the message and returns.
 */
#define CH_RETURN_ON_ERROR(x, log_tag, format, ...) do {                                       \
        ch_err_t err_rc_ = (x);                                                                \
        if (err_rc_ != CH_OK) {                                                      \
            CH_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);        \
            return err_rc_;                                                                     \
        }                                                                                       \
    } while(0)

/**
 * A version of CH_RETURN_ON_ERROR() macro that can be called from ISR.
 */
#define CH_RETURN_ON_ERROR_ISR(x, log_tag, format, ...) do {                                   \
        ch_err_t err_rc_ = (x);                                                                \
        if (err_rc_ != CH_OK) {                                                      \
            CH_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
            return err_rc_;                                                                     \
        }                                                                                       \
    } while(0)

/**
 * Macro which can be used to check the error code. If the code is not CH_OK, it prints the message,
 * sets the local variable 'ret' to the code, and then exits by jumping to 'goto_tag'.
 */
#define CH_GOTO_ON_ERROR(x, goto_tag, log_tag, format, ...) do {                               \
        ch_err_t err_rc_ = (x);                                                                \
        if (err_rc_ != CH_OK) {                                                      \
            CH_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);        \
            ret = err_rc_;                                                                      \
            goto goto_tag;                                                                      \
        }                                                                                       \
    } while(0)

/**
 * A version of CH_GOTO_ON_ERROR() macro that can be called from ISR.
 */
#define CH_GOTO_ON_ERROR_ISR(x, goto_tag, log_tag, format, ...) do {                           \
        ch_err_t err_rc_ = (x);                                                                \
        if (err_rc_ != CH_OK) {                                                      \
            CH_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
            ret = err_rc_;                                                                      \
            goto goto_tag;                                                                      \
        }                                                                                       \
    } while(0)

/**
 * Macro which can be used to check the condition. If the condition is not 'true', it prints the message
 * and returns with the supplied 'err_code'.
 */
#define CH_RETURN_ON_FALSE(a, err_code, log_tag, format, ...) do {                             \
        if (!(a)) {                                                                   \
            CH_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);        \
            return err_code;                                                                    \
        }                                                                                       \
    } while(0)

/**
 * A version of CH_RETURN_ON_FALSE() macro that can be called from ISR.
 */
#define CH_RETURN_ON_FALSE_ISR(a, err_code, log_tag, format, ...) do {                         \
        if (!(a)) {                                                                   \
            CH_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
            return err_code;                                                                    \
        }                                                                                       \
    } while(0)

/**
 * Macro which can be used to check the condition. If the condition is not 'true', it prints the message,
 * sets the local variable 'ret' to the supplied 'err_code', and then exits by jumping to 'goto_tag'.
 */
#define CH_GOTO_ON_FALSE(a, err_code, goto_tag, log_tag, format, ...) do {                     \
        if (!(a)) {                                                                   \
            CH_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);        \
            ret = err_code;                                                                     \
            goto goto_tag;                                                                      \
        }                                                                                       \
    } while (0)

/**
 * A version of CH_GOTO_ON_FALSE() macro that can be called from ISR.
 */
#define CH_GOTO_ON_FALSE_ISR(a, err_code, goto_tag, log_tag, format, ...) do {                 \
        if (!(a)) {                                                                   \
            CH_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
            ret = err_code;                                                                     \
            goto goto_tag;                                                                      \
        }                                                                                       \
    } while (0)


#ifdef __cplusplus
}
#endif
