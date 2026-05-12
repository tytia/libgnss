//
// Created by Terry on 5/12/2026.
//

#ifndef LIBGNSS_NMEA_TYPES_HPP
#define LIBGNSS_NMEA_TYPES_HPP

#include <string>
#include <limits>
#include <chrono>

#include "minmea.h"

namespace libgnss::nmea
{

enum class FixStatus : uint8_t
{
  NO_FIX,
  GPS_FIX,
  DGPS_FIX,
  PPS_MODE,
  RTK_FIX,
  RTK_FLOAT,
  DEAD_RECKONING,
  MANUAL_INPUT,
  SIM
};

struct Fix
{
  double latitude = std::numeric_limits<double>::quiet_NaN();
  double longitude = std::numeric_limits<double>::quiet_NaN();
  double altitude = std::numeric_limits<double>::quiet_NaN();
  double speed = std::numeric_limits<double>::quiet_NaN();
  double heading = std::numeric_limits<double>::quiet_NaN();
  FixStatus status = FixStatus::NO_FIX;
  std::chrono::system_clock::time_point timestamp;
};

struct Date
{
  uint8_t day;
  uint8_t month;
  uint16_t year;

  explicit Date(const minmea_date& date)
    : day(date.day)
    , month(date.month)
    , year(date.year)
  {
  }
};

struct Time
{
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint32_t microseconds;

  explicit Time(const minmea_time& time)
    : hours(time.hours)
    , minutes(time.minutes)
    , seconds(time.seconds)
    , microseconds(time.microseconds)
  {
  }
};

struct ID
{
  std::string_view talker;
  std::string_view sentence;
  std::string_view full;

  explicit ID(const minmea_type& type)
    : talker(type.talker_id, 2)
    , sentence(type.sentence_id, 3)
    , full(type.buf, 5)
  {
  }
};

struct SentenceGBS
{
  ID id;
  Time time;
  float err_latitude;
  float err_longitude;
  float err_altitude;
  int svid;
  float prob;
  float bias;
  float stddev;

  explicit SentenceGBS(const minmea_sentence_gbs& sentence)
    : id(sentence.type)
    , time(sentence.time)
    , err_latitude(minmea_tofloat(&sentence.err_latitude))
    , err_longitude(minmea_tofloat(&sentence.err_longitude))
    , err_altitude(minmea_tofloat(&sentence.err_altitude))
    , svid(sentence.svid)
    , prob(minmea_tofloat(&sentence.prob))
    , bias(minmea_tofloat(&sentence.bias))
    , stddev(minmea_tofloat(&sentence.stddev))
  {
  }
};

struct SentenceGGA
{
  ID id;
  Time time;
  float latitude;
  float longitude;
  FixStatus status;
  unsigned int sats;
  float hdop;
  float altitude; char altitude_units;
  float height; char height_units;
  float dgps_age;
};

}  // namespace libgnss::nmea

#endif  // LIBGNSS_NMEA_TYPES_HPP
