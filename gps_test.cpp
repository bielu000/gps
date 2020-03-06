//
// Created by pp on 03.03.20.
//

#include <gtest/gtest.h>

extern "C" {
  #include <gps.h>
}

TEST(GpsTestSuite, TestInit)
{
  gps_t gps;
  gps_init(&gps);

  ASSERT_NE(gps.working_buffer.max, 0);
}

TEST(GpsTestSuite, TestParsePayload)
{
//  const std::vector<std::string> noFix =
//  {
//    "$GPRMC,,V,,,,,,,,,,N*53\r\n",
//    "$GPVTG,,,,,,,,,N*30\r\n",
//    "$GPGGA,,,,,,0,00,99.99,,,,,,*48\r\n",
//    "$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30\r\n",
//    "$GPGSV,2,1,08,12,,,22,15,,,22,18,,,22,20,,,19*76\r\n",
//    "$GPGSV,2,2,08,21,,,22,22,,,21,23,,,22,24,,,23*77\r\n",
//    "$GPGLL,,,,,,V,N*64\r\n"
//  };
//
//
//  gps_t gps;
//  gps_init(&gps);
//
//  for (auto const& s : noFix[0])
//  {
//    gps_receive_char(&gps, s);
//  }
//
//  gps_task(&gps);
//
//  ASSERT_EQ(gps.hour, 0);
//  ASSERT_EQ(gps.minute, 0);
//  ASSERT_EQ(gps.second, 0);
}

void fill_gps_data(gps_handle_t gps, const std::string& str)
{
  for (auto const& s : str)
  {
    gps_receive_char(gps, s);
  }
}

TEST(GpsTestSuite, TestNoFixGPRMC)
{
  const std::string p = "$GPRMC,,V,,,,,,,,,,N*53\r\n";
  gps_t gps;
  gps_init(&gps);

  fill_gps_data(&gps, p);

  gps_task(&gps);

  ASSERT_EQ(gps.hour, 0);
  ASSERT_EQ(gps.minute, 0);
  ASSERT_EQ(gps.second, 0);
}

TEST(GpsTestSuite, TestFixGPRMC)
{
  const std::string p = "$GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68\r\n";
  gps_t gps;
  gps_init(&gps);

  fill_gps_data(&gps, p);

  gps_task(&gps);

  ASSERT_EQ(gps.hour, 22);
  ASSERT_EQ(gps.minute, 54);
  ASSERT_EQ(gps.second, 46);
}