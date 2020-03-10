//
// Created by pp on 03.03.20.
//


#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum _rbuff_options_t {
  RING_BUFFER_OPTION_DEFAULT = 0,
  RING_BUFFER_OPTION_ALLOW_OVERLAPPING = (1 << 0),
} rbuff_options_t;

typedef struct _ring_buffer_t {
  size_t head;
  size_t tail;
  uint8_t* data;
  size_t max;
  size_t capacity;
  uint8_t options;
} rbuff_t;

typedef rbuff_t* rbuff_handle_t;

typedef enum _rbuff_status_t {
  RING_BUFFER_WRITE_ERROR_FULL,
  RING_BUFFER_EMPTY,
  RING_BUFFER_OK
} rbuff_status_t;

inline void ring_buffer_init(rbuff_handle_t rbufhandle, uint8_t* data, size_t max, rbuff_options_t options)
{
  rbufhandle->data = data;
  rbufhandle->max = max;
  rbufhandle->capacity = 0;
  rbufhandle->head = 0;
  rbufhandle->tail = 0;
  rbufhandle->options = options;
}

inline size_t ring_buffer_capacity(rbuff_handle_t rbufhandle)
{
  return rbufhandle->capacity;
}

inline bool ring_buffer_full(rbuff_handle_t rbufhandle)
{
  return rbufhandle->capacity >= rbufhandle->max;
}

inline bool ring_buffer_is_option_set(rbuff_handle_t rbufhandle, rbuff_options_t option)
{
  return rbufhandle->options & option;
}

inline rbuff_status_t ring_buffer_write(rbuff_handle_t rbufhandle, uint8_t byte)
{
  if (!ring_buffer_is_option_set(rbufhandle, RING_BUFFER_OPTION_ALLOW_OVERLAPPING)
      && ring_buffer_full(rbufhandle) == true) {
    return RING_BUFFER_WRITE_ERROR_FULL;
  }
  rbufhandle->data[rbufhandle->head] = byte;
  rbufhandle->head = ((rbufhandle->head + 1) % rbufhandle->max);
  rbufhandle->capacity++;

  return RING_BUFFER_OK;
}

inline rbuff_status_t ring_buffer_read(rbuff_handle_t rbufhandle, uint8_t* dst)
{
  if (ring_buffer_capacity(rbufhandle) == 0) {
    return RING_BUFFER_EMPTY;
  }

  *dst = rbufhandle->data[rbufhandle->tail];
  rbufhandle->tail = ((rbufhandle->tail + 1) % rbufhandle->max);
  rbufhandle->capacity--;

  return RING_BUFFER_OK;
}

inline void ring_buffer_clear(rbuff_handle_t rbufhandle)
{
  for (size_t i = 0; i < rbufhandle->max; i++) {
    rbufhandle->data[0] = 0;
  }
}

inline size_t ring_buffer_head(rbuff_handle_t rbuffhandle)
{
  return rbuffhandle->head;
}

inline size_t ring_buffer_tail(rbuff_handle_t rbuffhandle)
{
  return rbuffhandle->tail;
}