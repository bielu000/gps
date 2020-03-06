//
// Created by pp on 03.03.20.
//

#pragma once

extern "C" {
  #include <ring_buffer.h>
}

#define GPS_WORKING_BUFFER_SIZE 128

typedef struct _gps_t_ {
  rbuff_t working_buffer;
  uint32_t lines_of_data;

  //
  // Time and date
  //
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t day;
  uint8_t month;
  uint8_t year;

  //
  // Position
  //
  double latitude;
  char latitude_direction;
  double longitude;
  char longitude_direction;

} gps_t;

typedef gps_t* gps_handle_t;

void gps_init(gps_handle_t gps_handle);
void gps_receive_char(gps_handle_t gps, char c);
void gps_task(gps_handle_t gps);