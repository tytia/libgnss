cmake_minimum_required(VERSION 3.16)

add_executable(reflex_test
  reflex_test.cpp
)

target_link_libraries(reflex_test PRIVATE reflex)
