cmake_minimum_required(VERSION 3.16)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  project(libgnss_reflex LANGUAGES CXX)
endif()

include(CheckCXXSourceRuns)

option(USE_AVX512BW "Enable AVX512BW intrinsics when available" ON)
option(USE_AVX2 "Enable AVX2 intrinsics when available" ON)
option(USE_SSE2 "Enable SSE2 intrinsics when available" ON)
option(USE_NEON "Enable NEON intrinsics when available" ON)

set(REFLEX_VENDOR_ROOT "${CMAKE_CURRENT_LIST_DIR}")
set(REFLEX_INCLUDE_ROOT "${REFLEX_VENDOR_ROOT}/include")
set(REFLEX_SOURCE_ROOT "${REFLEX_VENDOR_ROOT}/src/reflex")

set(reflex_sources
  ${REFLEX_SOURCE_ROOT}/debug.cpp
  ${REFLEX_SOURCE_ROOT}/error.cpp
  ${REFLEX_SOURCE_ROOT}/input.cpp
  ${REFLEX_SOURCE_ROOT}/matcher.cpp
  ${REFLEX_SOURCE_ROOT}/pattern.cpp
  ${REFLEX_SOURCE_ROOT}/simd.cpp
  ${REFLEX_SOURCE_ROOT}/utf8.cpp
)

set(reflex_compile_definitions)
set(reflex_simd_sources)
set(reflex_target_compile_options)

if(MSVC)
  set(_reflex_avx512_flag "/arch:AVX512")
  set(_reflex_avx2_flag "/arch:AVX2")
  set(_reflex_sse2_flag "/arch:SSE2")
else()
  set(_reflex_avx512_flag "-mavx512bw")
  set(_reflex_avx2_flag "-mavx2")
  set(_reflex_sse2_flag "-msse2")
endif()

function(_libgnss_reflex_check_source out_var compile_flag source_text)
  set(_saved_required_flags "${CMAKE_REQUIRED_FLAGS}")
  set(CMAKE_REQUIRED_FLAGS "${compile_flag}")
  check_cxx_source_runs("${source_text}" ${out_var})
  set(CMAKE_REQUIRED_FLAGS "${_saved_required_flags}" PARENT_SCOPE)
  set(${out_var} "${${out_var}}" PARENT_SCOPE)
endfunction()

set(_reflex_avx512_test_source [=[
  #include <immintrin.h>
  int main()
  {
    __m512i value = _mm512_set1_epi8(42);
    return static_cast<int>(_mm512_cmpeq_epi8_mask(value, value));
  }
]=])

set(_reflex_avx2_test_source [=[
  #include <immintrin.h>
  int main()
  {
    __m256i value = _mm256_set1_epi8(42);
    return _mm256_movemask_epi8(_mm256_cmpeq_epi8(value, value));
  }
]=])

set(_reflex_sse2_test_source [=[
  #include <emmintrin.h>
  int main()
  {
    __m128i value = _mm_set1_epi8(42);
    return _mm_movemask_epi8(_mm_cmpeq_epi8(value, value));
  }
]=])

set(_reflex_neon_test_source [=[
  #include <arm_neon.h>
  int main()
  {
    uint8x16_t value = vdupq_n_u8(42);
    return vgetq_lane_u8(value, 0);
  }
]=])

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|X86_64|amd64|AMD64|x86|X86|i[3-6]86)$")
  if(USE_AVX512BW)
    _libgnss_reflex_check_source(REFLEX_HAVE_AVX512BW "${_reflex_avx512_flag}" "${_reflex_avx512_test_source}")
  endif()

  if(REFLEX_HAVE_AVX512BW)
    list(APPEND reflex_compile_definitions HAVE_AVX512BW)
    list(APPEND reflex_target_compile_options "${_reflex_avx512_flag}")
    list(APPEND reflex_simd_sources
      ${REFLEX_SOURCE_ROOT}/matcher_avx2.cpp
      ${REFLEX_SOURCE_ROOT}/matcher_avx512bw.cpp
      ${REFLEX_SOURCE_ROOT}/simd_avx2.cpp
      ${REFLEX_SOURCE_ROOT}/simd_avx512bw.cpp
    )
  else()
    if(USE_AVX2)
      _libgnss_reflex_check_source(REFLEX_HAVE_AVX2 "${_reflex_avx2_flag}" "${_reflex_avx2_test_source}")
    endif()

    if(REFLEX_HAVE_AVX2)
      list(APPEND reflex_compile_definitions HAVE_AVX2)
      list(APPEND reflex_target_compile_options "${_reflex_avx2_flag}")
      list(APPEND reflex_simd_sources
        ${REFLEX_SOURCE_ROOT}/matcher_avx2.cpp
        ${REFLEX_SOURCE_ROOT}/simd_avx2.cpp
      )
    else()
      if(USE_SSE2)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
          set(REFLEX_HAVE_SSE2 TRUE)
        else()
          _libgnss_reflex_check_source(REFLEX_HAVE_SSE2 "${_reflex_sse2_flag}" "${_reflex_sse2_test_source}")
        endif()
      endif()

      if(REFLEX_HAVE_SSE2)
        list(APPEND reflex_compile_definitions HAVE_SSE2)
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
          list(APPEND reflex_target_compile_options "${_reflex_sse2_flag}")
        elseif(NOT MSVC)
          list(APPEND reflex_target_compile_options "${_reflex_sse2_flag}")
        endif()
      endif()
    endif()
  endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|ARM64|aarch64|AARCH64|arm|ARM)" AND NOT MSVC)
  if(USE_NEON)
    _libgnss_reflex_check_source(REFLEX_HAVE_NEON "" "${_reflex_neon_test_source}")
  endif()

  if(REFLEX_HAVE_NEON)
    list(APPEND reflex_compile_definitions HAVE_NEON)
  endif()
endif()

add_library(reflex STATIC
  ${reflex_sources}
  ${reflex_simd_sources}
)

add_library(Reflex::reflex ALIAS reflex)

target_compile_features(reflex PUBLIC cxx_std_11)

target_include_directories(reflex
  PUBLIC
    $<BUILD_INTERFACE:${REFLEX_INCLUDE_ROOT}>
)

if(WIN32)
  target_link_libraries(reflex PRIVATE ws2_32)
endif()

target_compile_definitions(reflex PRIVATE ${reflex_compile_definitions})
target_compile_options(reflex PRIVATE ${reflex_target_compile_options})

set_target_properties(reflex PROPERTIES OUTPUT_NAME reflex)