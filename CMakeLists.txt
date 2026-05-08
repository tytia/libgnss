cmake_minimum_required(VERSION 4.2)
project(libgnss)

set(CMAKE_CXX_STANDARD 17)

add_executable(libgnss
  third_party/minmea/src/minmea.c
  src/nmea_reader.cpp
  src/rtcm_handler.cpp
  src/gnss_receiver.cpp
)

target_include_directories(libgnss PUBLIC
  include
  third_party/minmea/include
  third_party/ringbuffer/include
)
