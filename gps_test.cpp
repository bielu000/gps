//
// Created by pp on 03.03.20.
//

#include <gtest/gtest.h>

extern "C" {
  #include <gps.h>
}

void fill_gps_data(gps_handle_t gps, const std::string& str)
{
  for (auto const& s : str)
  {
    gps_receive_char(gps, s);
  }
}


TEST(GpsTestSuite, TestInit)
{
  gps_t gps;
  gps_init(&gps);

  ASSERT_NE(gps.working_buffer.max, 0);
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

TEST(GpsTestSuite, TestNoFixGPVTG)
{
  const std::string p = "$GPVTG,,,,,,,,,N*30\r\n";
  gps_t gps;
  gps_init(&gps);

  fill_gps_data(&gps, p);
  gps_task(&gps);

  ASSERT_EQ(gps.speed_knots, 0);
  ASSERT_EQ(gps.speed_kilomiters, 0);
}

TEST(GpsTestSuite, TestFixGPVTG)
{
  const std::string p = "$GPVTG,054.7,T,034.4,M,005.5,N,010.2,K\r\n";
  gps_t gps;
  gps_init(&gps);

  fill_gps_data(&gps, p);
  gps_task(&gps);

  ASSERT_EQ(gps.speed_knots, 5.5);
  ASSERT_EQ(gps.speed_kilomiters, 10.2);
}

TEST(GpsTestSuite, TestNoFixGPGGA)
{
  const std::string p = "$GPGGA,,,,,,0,00,99.99,,,,,,*48\r\n";
  gps_t gps;
  gps_init(&gps);

  fill_gps_data(&gps, p);
  gps_task(&gps);

  ASSERT_EQ(gps.quality, GPS_NO_FIX);
  ASSERT_EQ(gps.altitude, 0);
  ASSERT_EQ(gps.satelites_in_use, 0);
}

TEST(GpsTestSuite, TestFixGPGGA)
{
  const std::string p = "$GPGGA,134658.00,5106.9792,N,11402.3003,W,2,09,1.0,1048.47,M,-16.27,M,08,AAAA*60\r\n";
  gps_t gps;
  gps_init(&gps);

  fill_gps_data(&gps, p);
  gps_task(&gps);

  ASSERT_EQ(gps.quality, DGPS_FIX);
  ASSERT_EQ(gps.altitude, 1048.47);
  ASSERT_EQ(gps.satelites_in_use, 9);
}

TEST(GpsTestSuite, TestNoFixGPGSA)
{
  const std::string p = "$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30\r\n";
  gps_t gps;
  gps_init(&gps);

  fill_gps_data(&gps, p);
  gps_task(&gps);

  ASSERT_EQ(gps.mode, GPS_FIX_MODE_NOT_AVAILABLE);
  ASSERT_EQ(gps.pdop, 99.99);
  ASSERT_EQ(gps.hdop, 99.99);
  ASSERT_EQ(gps.vdop, 99.99);
}

TEST(GpsTestSuite, TestFixGPGSA)
{
  const std::string p = "$GPGSA,A,3,19,28,14,18,27,22,31,39,,,,,1.7,1.0,1.3*35\r\n";
  gps_t gps;
  gps_init(&gps);

  fill_gps_data(&gps, p);
  gps_task(&gps);

  ASSERT_EQ(gps.mode, GPS_FIX_MODE_3D);
  ASSERT_EQ(gps.pdop, 1.7);
  ASSERT_EQ(gps.hdop, 1.0);
  ASSERT_EQ(gps.vdop, 1.3);
}

TEST(GpsTestSuite, TestParseLinesWithFix)
{
  std::vector<std::string> p {
    "$GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68\r\n",
    "$GPVTG,054.7,T,034.4,M,005.5,N,010.2,K\r\n",
    "$GPGGA,34658.00,5106.9792,N,11402.3003,W,2,09,1.0,1048.47,M,-16.27,M,08,AAAA*60\r\n",
    "$GPGSA,A,3,19,28,14,18,27,22,31,39,,,,,1.7,1.0,1.3*35\r\n"
  };

  gps_t gps;
  gps_init(&gps);

  for (auto const& l : p)
  {
    for (auto const& c : l)
    {
      gps_receive_char(&gps, c);
    }
  }

  //Assert GPRMC
  gps_task(&gps);
  ASSERT_EQ(gps.hour, 22);
  ASSERT_EQ(gps.minute, 54);
  ASSERT_EQ(gps.second, 46);

  //Assert GPVTG
  gps_task(&gps);
  ASSERT_EQ(gps.speed_knots, 5.5);
  ASSERT_EQ(gps.speed_kilomiters, 10.2);

  //Assert GPGGA
  gps_task(&gps);
  ASSERT_EQ(gps.quality, DGPS_FIX);
  ASSERT_EQ(gps.altitude, 1048.47);
  ASSERT_EQ(gps.satelites_in_use, 9);

  //Assert GPGSA
  gps_task(&gps);
  ASSERT_EQ(gps.mode, GPS_FIX_MODE_3D);
  ASSERT_EQ(gps.pdop, 1.7);
  ASSERT_EQ(gps.hdop, 1.0);
  ASSERT_EQ(gps.vdop, 1.3);
}

TEST(GpsTestSuite, TestParseLinesWithNoFix)
{
  std::vector<std::string> p {
    "$GPRMC,,V,,,,,,,,,,N*53\r\n",
    "$GPVTG,,,,,,,,,N*30\r\n",
    "$GPGGA,,,,,,0,00,99.99,,,,,,*48\r\n",
    "$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30\r\n"
  };

  gps_t gps;
  gps_init(&gps);

  for (auto const& l : p)
  {
    for (auto const& c : l)
    {
      gps_receive_char(&gps, c);
    }
  }

  //Assert GPRMC
  gps_task(&gps);
  ASSERT_EQ(gps.hour, 0);
  ASSERT_EQ(gps.minute, 0);
  ASSERT_EQ(gps.second, 0);

  //Assert GPVTG
  gps_task(&gps);
  ASSERT_EQ(gps.speed_knots, 0);
  ASSERT_EQ(gps.speed_kilomiters, 0);

  //Assert GPGGA
  gps_task(&gps);
  ASSERT_EQ(gps.quality, GPS_NO_FIX);
  ASSERT_EQ(gps.altitude, 0);
  ASSERT_EQ(gps.satelites_in_use, 0);

  //Assert GPGSA
  gps_task(&gps);
  ASSERT_EQ(gps.mode, GPS_FIX_MODE_NOT_AVAILABLE);
  ASSERT_EQ(gps.pdop, 99.99);
  ASSERT_EQ(gps.hdop, 99.99);
  ASSERT_EQ(gps.vdop, 99.99);
}