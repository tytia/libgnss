cmake_minimum_required(VERSION 3.16)
project(libgnss LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_path(LIBSERIALPORT_INCLUDE_DIR libserialport.h REQUIRED)
find_library(LIBSERIALPORT_LIBRARY NAMES serialport REQUIRED)

add_subdirectory(third_party/minmea)
add_subdirectory(third_party/reflex)
add_subdirectory(third_party/ringbuffer)

option(LIBGNSS_BUILD_EXAMPLES "Build libgnss example programs" ON)

add_library(libgnss
  src/gnss_receiver.cpp
  src/nmea_reader.cpp
  src/rtcm.cpp
  src/serial.cpp
)

target_compile_features(libgnss PUBLIC cxx_std_17)

target_include_directories(libgnss
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${LIBSERIALPORT_INCLUDE_DIR}
)

target_link_libraries(libgnss PUBLIC
  ${LIBSERIALPORT_LIBRARY}
  minmea
  reflex
  ringbuffer
)

if(LIBGNSS_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
