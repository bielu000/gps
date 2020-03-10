//
// Created by pp on 03.03.20.
//

#pragma once

extern "C" {
  #include <ring_buffer.h>
}

// Useful info about nmea communicates:
//   - http://aprs.gids.nl/nmea/#gll
//   - http://aprs.gids.nl/nmea

#define GPS_WORKING_BUFFER_SIZE 500

//Fix
#define GPS_NO_FIX 0
#define GPS_FIX    1
#define DGPS_FIX   2

//Mode
#define GPS_FIX_MODE_NOT_AVAILABLE 1
#define GPS_FIX_MODE_2D            2
#define GPS_FIX_MODE_3D            3

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

  //
  // Speed -> GPVTG
  //
  double speed_knots;
  double speed_kilomiters;

  //
  // Fix, altitude -> GPGGA
  //
  uint8_t quality;
  uint8_t satelites_in_use;
  double altitude;

  //Mode, pdop, gdop, vdop -> GPGSA
  uint8_t mode;
  double pdop;
  double hdop;
  double vdop;


} gps_t;

typedef gps_t* gps_handle_t;

void gps_init(gps_handle_t gps_handle);
void gps_receive_char(gps_handle_t gps, char c);
void gps_task(gps_handle_t gps);