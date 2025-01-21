/********************************** (C) COPYRIGHT *******************************
* File Name          : ring_buffer.c
* Author             : Oren
* Version            : V1.1
* Date               : 2024/1/24
* Description        : This file provides all the ring buffer functions. 
********************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "ring_buffer.h"

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      rb_param_init
 * @brief   Initialization of ring buffer structure parameters.
 * @param   buf: The address of the ring buffer structure.
 * @param   addr: Ring buffer address.
 * @param   size: The length of the ring buffer.
 * @return  None
 */
void rb_param_init(ring_buffer_t buf, rb_uint32_t *addr, rb_uint16_t size)
{
    /* initialize write\read index and data length */
    buf->wr_index = 0;
    buf->rd_index = 0;
    buf->dat_len = 0;

    /* set buffer address and size */
    buf->buf_ptr = addr;
    buf->buf_size = size;
}

/*********************************************************************
 * @fn      rb_write_byte
 * @brief   Write byte to the ring buffer.
 * @param   buf: The address of the ring buffer structure.
 * @param   byte: The byte to be written.
 * @return  SUCCESS or FAILURE.
 */
rb_uint8_t rb_write_byte(ring_buffer_t buf, rb_uint32_t byte)
{
    if (buf->dat_len == buf->buf_size) /* data overflow */
        return RB_FAILURE;

    buf->buf_ptr[buf->wr_index] = byte;

    /* parameter update */
    ++buf->wr_index;
    if (buf->wr_index == buf->buf_size)
        buf->wr_index = 0;
    ++buf->dat_len;

    return RB_SUCCESS;
}

/*********************************************************************
 * @fn      rb_write_data
 * @brief   Write data to the ring buffer.
 * @param   buf: The address of the ring buffer structure.
 * @param   src: The address of the data to be written.
 * @param   wr_len: The length of the data to be written.
 * @return  SUCCESS or FAILURE.
 */
rb_uint8_t rb_write_data(ring_buffer_t buf, const rb_uint32_t *src, rb_uint16_t wr_len)
{
    rb_uint32_t *dst;
    rb_uint16_t len;
    rb_uint16_t overflow_len;

    if ((buf->dat_len + wr_len) > buf->buf_size) /* data overflow */
        return RB_FAILURE;

    if ((buf->buf_size - wr_len) > buf->wr_index)
    {
        len = wr_len;
        dst = buf->buf_ptr + buf->wr_index;
        while (len--)
            *dst++ = *src++;

        buf->wr_index += wr_len; /* parameter update */
    }
    else /* data exceeds the end of the buffer */
    {
        overflow_len = buf->wr_index - (buf->buf_size - wr_len);

        len = wr_len - overflow_len;
        dst = buf->buf_ptr + buf->wr_index;
        while (len--)
            *dst++ = *src++;

        len = overflow_len;
        dst = buf->buf_ptr;
        while (len--)
            *dst++ = *src++;

        buf->wr_index = overflow_len; /* parameter update */
    }

    buf->dat_len += wr_len; /* parameter update */

    return RB_SUCCESS;
}

/*********************************************************************
 * @fn      rb_write_byte_without_update_len
 * @brief   Write byte to the ring buffer without updating the data length.
 * @param   buf: The address of the ring buffer structure.
 * @param   byte: The byte to be written.
 * @return  SUCCESS or FAILURE.
 */
rb_uint8_t rb_write_byte_without_update_len(ring_buffer_t buf, rb_uint32_t byte)
{
    if (buf->dat_len == buf->buf_size) /* data overflow */
        return RB_FAILURE;

    buf->buf_ptr[buf->wr_index] = byte;

    /* parameter update */
    ++buf->wr_index;
    if (buf->wr_index == buf->buf_size)
        buf->wr_index = 0;

    return RB_SUCCESS;
}

/*********************************************************************
 * @fn      rb_write_data_without_update_len
 * @brief   Write data to the ring buffer without updating the data length.
 * @param   buf: The address of the ring buffer structure.
 * @param   src: The address of the data to be written.
 * @param   wr_len: The length of the data to be written.
 * @return  SUCCESS or FAILURE.
 */
rb_uint8_t rb_write_data_without_update_len(ring_buffer_t buf, rb_uint32_t *src, rb_uint16_t wr_len)
{
    rb_uint32_t *dst;
    rb_uint16_t len;
    rb_uint16_t overflow_len;

    if ((buf->dat_len + wr_len) > buf->buf_size) /* data overflow */
        return RB_FAILURE;

    if ((buf->buf_size - wr_len) > buf->wr_index)
    {
        len = wr_len;
        dst = buf->buf_ptr + buf->wr_index;
        while (len--)
            *dst++ = *src++;

        buf->wr_index += wr_len; /* parameter update */
    }
    else /* data exceeds the end of the buffer */
    {
        overflow_len = buf->wr_index - (buf->buf_size - wr_len);

        len = wr_len - overflow_len;
        dst = buf->buf_ptr + buf->wr_index;
        while (len--)
            *dst++ = *src++;

        len = overflow_len;
        dst = buf->buf_ptr;
        while (len--)
            *dst++ = *src++;

        buf->wr_index = overflow_len; /* parameter update */
    }

    return RB_SUCCESS;
}

/*********************************************************************
 * @fn      rb_write_update_len
 * @brief   Update the data length of the ring buffer.
 * @param   buf: The address of the ring buffer structure.
 * @param   wr_len: The length of the data to be written.
 * @return  SUCCESS or FAILURE.
 */
rb_uint8_t rb_write_update_len(ring_buffer_t buf, rb_uint16_t wr_len)
{
    buf->dat_len += wr_len; /* parameter update */

    return RB_SUCCESS;
}

/*********************************************************************
 * @fn      rb_read_data
 * @brief   Read data from the ring buffer.
 * @param   buf: The address of the ring buffer structure.
 * @param   dst: The storage address of the read data.
 * @param   rd_len: The length of the data to be read.
 * @return  SUCCESS or FAILURE.
 */
rb_uint8_t rb_read_data(ring_buffer_t buf, rb_uint32_t *dst, rb_uint16_t rd_len)
{
    rb_uint32_t *src;
    rb_uint16_t len;
    rb_uint16_t overflow_len;

    if (rd_len > buf->dat_len) /* data overflow */
        return RB_FAILURE;

    if ((buf->buf_size - rd_len) > buf->rd_index)
    {
        len = rd_len;
        src = buf->buf_ptr + buf->rd_index;
        while (len--)
            *dst++ = *src++;

        buf->rd_index += rd_len; /* parameter update */
    }
    else /* data exceeds the end of the buffer */
    {
        overflow_len = buf->rd_index - (buf->buf_size - rd_len);

        len = rd_len - overflow_len;
        src = buf->buf_ptr + buf->rd_index;
        while (len--)
            *dst++ = *src++;

        len = overflow_len;
        src = buf->buf_ptr;
        while (len--)
            *dst++ = *src++;

        buf->rd_index = overflow_len; /* parameter update */
    }

    buf->dat_len -= rd_len; /* parameter update */

    return RB_SUCCESS;
}

/*********************************************************************
 * @fn      rb_read_data_without_update_len
 * @brief   Read data from the ring buffer without updating the data length.
 * @param   buf: The address of the ring buffer structure.
 * @param   dst: The storage address of the read data.
 * @param   rd_len: The length of the data to be read.
 * @return  SUCCESS or FAILURE.
 */
rb_uint8_t rb_read_data_without_update_len(ring_buffer_t buf, rb_uint32_t *dst, rb_uint16_t rd_len)
{
    rb_uint32_t *src;
    rb_uint16_t len;
    rb_uint16_t overflow_len;

    if (rd_len > buf->dat_len) /* data overflow */
        return RB_FAILURE;

    if ((buf->buf_size - rd_len) > buf->rd_index)
    {
        len = rd_len;
        src = buf->buf_ptr + buf->rd_index;
        while (len--)
            *dst++ = *src++;

        buf->rd_index += rd_len; /* parameter update */
    }
    else /* data exceeds the end of the buffer */
    {
        overflow_len = buf->rd_index - (buf->buf_size - rd_len);

        len = rd_len - overflow_len;
        src = buf->buf_ptr + buf->rd_index;
        while (len--)
            *dst++ = *src++;

        len = overflow_len;
        src = buf->buf_ptr;
        while (len--)
            *dst++ = *src++;

        buf->rd_index = overflow_len; /* parameter update */
    }

    return RB_SUCCESS;
}

/*********************************************************************
 * @fn      rb_read_update_len (same as rb_delete_update_len)
 * @brief   Update the data length of the ring buffer.
 * @param   buf: The address of the ring buffer structure.
 * @param   rd_len: The length of the data to be read.
 * @return  SUCCESS or FAILURE.
 */
rb_uint8_t rb_read_update_len(ring_buffer_t buf, rb_uint16_t rd_len)
{
    buf->dat_len -= rd_len; /* Parameter update */

    return RB_SUCCESS;
}

/*********************************************************************
 * @fn      rb_copy_data
 * @brief   Copy data from rd_index location (forward) of the ring buffer.
 * @param   buf: The address of the ring buffer structure.
 * @param   dst: The storage address of the copied data.
 * @param   cp_len: The length of the data to be copied.
 * @return  SUCCESS or FAILURE.
 */
rb_uint8_t rb_copy_data(ring_buffer_t buf, rb_uint32_t *dst, rb_uint16_t cp_len)
{
    rb_uint32_t *src;
    rb_uint16_t len;
    rb_uint16_t overflow_len;

    if (cp_len > buf->dat_len) /* data overflow */
        return RB_FAILURE;

    if ((buf->buf_size - cp_len) > buf->rd_index)
    {
        len = cp_len;
        src = buf->buf_ptr + buf->rd_index;
        while (len--)
            *dst++ = *src++;
    }
    else /* data exceeds the end of the buffer */
    {
        overflow_len = buf->rd_index - (buf->buf_size - cp_len);

        len = cp_len - overflow_len;
        src = buf->buf_ptr + buf->rd_index;
        while (len--)
            *dst++ = *src++;

        len = overflow_len;
        src = buf->buf_ptr;
        while (len--)
            *dst++ = *src++;
    }

    return RB_SUCCESS;
}

/*********************************************************************
 * @fn      rb_copy_offset_data
 * @brief   Copy data from rd_index offset location (forward) of the ring buffer.
 * @param   buf: The address of the ring buffer structure.
 * @param   dst: The storage address of the copied data.
 * @param   cp_len: The length of the data to be copied.
 * @param   offset: The offset of the byte to be copied.
 * @return  SUCCESS or FAILURE.
 */
rb_uint8_t rb_copy_offset_data(ring_buffer_t buf, rb_uint32_t *dst, rb_uint16_t cp_len, rb_uint16_t offset)
{
    rb_uint32_t *src;
    rb_uint16_t len;
    rb_uint16_t overflow_len;

    if ((cp_len + offset) > buf->dat_len) /* data overflow */
        return RB_FAILURE;

    if ((buf->buf_size - cp_len - offset) < buf->rd_index) /* data exceeds the end of the buffer */
    {
        overflow_len = buf->rd_index - (buf->buf_size - cp_len - offset);

        len = cp_len - overflow_len;
        src = buf->buf_ptr + buf->rd_index + offset;
        while (len--)
            *dst++ = *src++;

        len = overflow_len;
        src = buf->buf_ptr;
        while (len--)
            *dst++ = *src++;
    }
    else
    {
        len = cp_len;
        src = buf->buf_ptr + buf->rd_index + offset;
        while (len--)
            *dst++ = *src++;
    }

    return RB_SUCCESS;
}

/*********************************************************************
 * @fn      rb_copy_rd_index_offset_byte
 * @brief   Copy byte from rd_index offset location (forward) of the ring buffer.
 * @param   buf: The address of the ring buffer structure.
 * @param   offset: The offset of the byte to be copied.
 * @return  Copied byte.
 */
rb_uint32_t rb_copy_rd_index_offset_byte(ring_buffer_t buf, rb_uint16_t offset)
{
    rb_uint32_t byte;
    rb_uint16_t overflow_len;

    if ((buf->buf_size - offset) > buf->rd_index)
    {
        byte = buf->buf_ptr[buf->rd_index + offset];
    }
    else /* data exceeds the end of the buffer */
    {
        overflow_len = buf->rd_index - (buf->buf_size - offset);
        byte = buf->buf_ptr[overflow_len];
    }

    return byte;
}

/*********************************************************************
 * @fn      rb_copy_wr_index_offset_byte
 * @brief   Copy byte from wr_index offset location (backward) of the ring buffer.
 * @param   buf: The address of the ring buffer structure.
 * @param   offset: The offset of the byte to be copied.
 * @return  Copied byte.
 */
rb_uint32_t rb_copy_wr_index_offset_byte(ring_buffer_t buf, rb_uint16_t offset)
{
    rb_uint32_t byte;
    rb_uint16_t overflow_len;

    if (buf->wr_index < offset) // data exceeds the end of the buffer
    {
        overflow_len = buf->buf_size - offset + buf->wr_index;
        byte = buf->buf_ptr[overflow_len];
    }
    else
    {
        byte = buf->buf_ptr[buf->wr_index - offset];
    }

    return byte;
}

/*********************************************************************
 * @fn      rb_delete_data
 * @brief   Delete data from the ring buffer.
 * @param   buf: The address of the ring buffer structure.
 * @param   del_len: The length of the data to be deleted.
 * @return  SUCCESS or FAILURE.
 */
rb_uint8_t rb_delete_data(ring_buffer_t buf, rb_uint16_t del_len)
{
    rb_uint16_t overflow_len;

    if (del_len > buf->dat_len) /* data overflow */
        return RB_FAILURE;

    if ((buf->buf_size - del_len) > buf->rd_index)
    {
        buf->rd_index += del_len; /* parameter update */
    }
    else /* data exceeds the end of the buffer */
    {
        overflow_len = buf->rd_index - (buf->buf_size - del_len);
        buf->rd_index = overflow_len; /* parameter update */
    }

    buf->dat_len -= del_len; /* parameter update */

    return RB_SUCCESS;
}

/*********************************************************************
 * @fn      rb_delete_data_without_update_len
 * @brief   Delete data from the ring buffer without updating the data length.
 * @param   buf: The address of the ring buffer structure.
 * @param   del_len: The length of the data to be deleted.
 * @return  SUCCESS or FAILURE.
 */
rb_uint8_t rb_delete_data_without_update_len(ring_buffer_t buf, rb_uint16_t del_len)
{
    rb_uint16_t overflow_len;

    if (del_len > buf->dat_len) /* data overflow */
        return RB_FAILURE;

    if ((buf->buf_size - del_len) > buf->rd_index)
    {
        buf->rd_index += del_len; /* parameter update */
    }
    else /* data exceeds the end of the buffer */
    {
        overflow_len = buf->rd_index - (buf->buf_size - del_len);
        buf->rd_index = overflow_len; /* parameter update */
    }

    return RB_SUCCESS;
}

/*********************************************************************
 * @fn      rb_delete_update_len (same as rb_read_update_len)
 * @brief   Update the data length of the ring buffer.
 * @param   buf: The address of the ring buffer structure.
 * @param   del_len: The length of the data to be deleted.
 * @return  SUCCESS or FAILURE.
 */
rb_uint8_t rb_delete_update_len(ring_buffer_t buf, rb_uint16_t del_len)
{
    buf->dat_len -= del_len; /* parameter update */

    return RB_SUCCESS;
}
