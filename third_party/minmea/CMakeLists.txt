add_library(minmea STATIC
  src/minmea.c
)

target_include_directories(minmea PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
