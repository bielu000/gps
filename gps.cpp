//
// Created by pp on 03.03.20.
//

#include <stdlib.h>
#include <string.h>

extern "C" {
  #include <gps.h>
}

#define CR 13
#define LF 10
#define NULL_TERM 0

char* strtoke(char *str, const char *delim)
{
  static char *start = NULL; /* stores string str for consecutive calls */
  char *token = NULL; /* found token */
  if (str) start = str;
  if (!start) return NULL;
  token = start;
  start = strpbrk(start, delim);
  if (start) *start++ = '\0';
  return token;
}

// PRIVATE FUNCTIONS DECLARATIONS //
void __gps_parse_line(gps_handle_t gps);
void __gps_parse_gprmc(gps_handle_t gps);

static uint8_t working_buffer_data[GPS_WORKING_BUFFER_SIZE];

void gps_init(gps_handle_t gps_handle)
{
  ring_buffer_init(&gps_handle->working_buffer,
    working_buffer_data,
    GPS_WORKING_BUFFER_SIZE,
    RING_BUFFER_OPTION_DEFAULT
  );

  gps_handle->lines_of_data = 0;

  //Data - time
  gps_handle->hour = 0;
  gps_handle->minute = 0;
  gps_handle->second = 0;
}

void gps_receive_char(gps_handle_t gps, char c)
{
  if (c == LF || c == NULL_TERM) {
    return;
  }

  if (c == CR) {
    gps->lines_of_data++;
  }

  rbuff_handle_t rbuff_handle = &gps->working_buffer;

  ring_buffer_write(rbuff_handle, (uint8_t)c);
}

void gps_task(gps_handle_t gps)
{
  if (gps->lines_of_data)
  {
    __gps_parse_line(gps);
  }
}

// PRIVATE FUNCTIONS IMPLEMENTATION //
void __gps_parse_line(gps_handle_t gps)
{
  rbuff_handle_t rbuff_h = &gps->working_buffer;

  char* token = strtoke((char*)rbuff_h->data, ",");

  if (strcmp(token, "$GPRMC") == 0) __gps_parse_gprmc(gps);

  int x = 10;
}

void __gps_parse_gprmc(gps_handle_t gps)
{
  char* token;
  uint32_t tmp;

  token = strtoke(NULL, ",");

  if (strlen(token) > 0) {
    tmp = atoi(token);

    gps->second = (uint8_t)(tmp % 100);
    gps->minute = (uint8_t)((tmp / 100) % 100);
    gps->hour = (uint8_t)((tmp / 10000) % 100);
  }
}