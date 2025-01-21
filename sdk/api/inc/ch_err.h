#ifndef __CH_ERR_H__
#define __CH_ERR_H__


#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef int ch_err_t;

/* Definitions for error constants. */
#define CH_OK          0       /*!< ch_err_t value indicating success (no error) */
#define CH_FAIL        -1      /*!< Generic ch_err_t code indicating failure */

#define CH_ERR_NO_MEM              0x101   /*!< Out of memory */
#define CH_ERR_INVALID_ARG         0x102   /*!< Invalid argument */
#define CH_ERR_INVALID_STATE       0x103   /*!< Invalid state */
#define CH_ERR_INVALID_SIZE        0x104   /*!< Invalid size */
#define CH_ERR_NOT_FOUND           0x105   /*!< Requested resource not found */
#define CH_ERR_NOT_SUPPORTED       0x106   /*!< Operation or feature not supported */
#define CH_ERR_TIMEOUT             0x107   /*!< Operation timed out */
#define CH_ERR_INVALID_RCHONSE    0x108   /*!< Received rchonse was invalid */




#ifdef __cplusplus
}
#endif


#endif /* __CH_ERR_H__ */
