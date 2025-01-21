/********************************** (C) COPYRIGHT *******************************
* File Name          : ring_buffer.h
* Author             : Oren
* Version            : V1.1
* Date               : 2024/1/24
* Description        : This file contains all the functions prototypes for the 
*                      ring buffer library.
********************************************************************************/
#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * DEFINES
 */

#ifndef RB_SUCCESS
#define RB_SUCCESS          0
#endif
#ifndef RB_FAILURE
#define RB_FAILURE          1
#endif

/*********************************************************************
 * TYPEDEFS
 */

/* define variable types */
typedef unsigned char       rb_uint8_t;
typedef unsigned short      rb_uint16_t;
typedef unsigned long       rb_uint32_t;

/* define structure types */
struct ring_buffer
{
    rb_uint32_t *buf_ptr;    //ring buffer address
    rb_uint16_t buf_size;   //ring buffer size
    rb_uint16_t wr_index;   //offset address of write data
    rb_uint16_t rd_index;   //offset address of read data
    rb_uint16_t dat_len;    //length of saved data
};
typedef struct ring_buffer *ring_buffer_t;

/*********************************************************************
 * FUNCTIONS
 */

void rb_param_init(ring_buffer_t buf, rb_uint32_t *addr, rb_uint16_t size);
rb_uint8_t rb_write_byte(ring_buffer_t buf, rb_uint32_t byte);
rb_uint8_t rb_write_data(ring_buffer_t buf, const rb_uint32_t *src, rb_uint16_t wr_len);
rb_uint8_t rb_write_byte_without_update_len(ring_buffer_t buf, rb_uint32_t byte);
rb_uint8_t rb_write_data_without_update_len(ring_buffer_t buf, rb_uint32_t *src, rb_uint16_t wr_len);
rb_uint8_t rb_write_update_len(ring_buffer_t buf, rb_uint16_t wr_len);
rb_uint8_t rb_read_data(ring_buffer_t buf, rb_uint32_t *dst, rb_uint16_t rd_len);
rb_uint8_t rb_read_data_without_update_len(ring_buffer_t buf, rb_uint32_t *dst, rb_uint16_t rd_len);
rb_uint8_t rb_read_update_len(ring_buffer_t buf, rb_uint16_t rd_len);
rb_uint8_t rb_copy_data(ring_buffer_t buf, rb_uint32_t *dst, rb_uint16_t cp_len);
rb_uint8_t rb_copy_offset_data(ring_buffer_t buf, rb_uint32_t *dst, rb_uint16_t cp_len, rb_uint16_t offset);
rb_uint32_t rb_copy_rd_index_offset_byte(ring_buffer_t buf, rb_uint16_t offset);
rb_uint32_t rb_copy_wr_index_offset_byte(ring_buffer_t buf, rb_uint16_t offset);
rb_uint8_t rb_delete_data(ring_buffer_t buf, rb_uint16_t del_len);
rb_uint8_t rb_delete_data_without_update_len(ring_buffer_t buf, rb_uint16_t del_len);
rb_uint8_t rb_delete_update_len(ring_buffer_t buf, rb_uint16_t del_len);

#ifdef __cplusplus
}
#endif

#endif /*__RING_BUFFER_H__*/
