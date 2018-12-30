project(cpp-serialization-benchmark)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "disable tests" FORCE)
set(CAPNP_LITE OFF CACHE BOOL "no reflection" FORCE)
add_subdirectory(external_lib/flatbuffers EXCLUDE_FROM_ALL)
add_subdirectory(external_lib/google-benchmark EXCLUDE_FROM_ALL)
add_subdirectory(external_lib/cista EXCLUDE_FROM_ALL)
add_subdirectory(external_lib/capnproto EXCLUDE_FROM_ALL)
add_subdirectory(external_lib/cereal EXCLUDE_FROM_ALL)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/graph_generated.h
  COMMAND flatc --cpp ${CMAKE_CURRENT_SOURCE_DIR}/definitions/graph.fbs
  DEPENDS flatc
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/definitions/graph.fbs
)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/graph.capnp.h
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/graph.capnp.c++
  COMMAND
    ${CMAKE_COMMAND}
      -E env PATH="$ENV{PATH}:$<TARGET_FILE_DIR:capnp_tool>"
      $<TARGET_FILE_DIR:capnp_tool>/capnp
        compile
          --output c++
          --src-prefix "${CMAKE_CURRENT_SOURCE_DIR}/definitions/"
          ${CMAKE_CURRENT_SOURCE_DIR}/definitions/graph.capnp
  DEPENDS capnp_tool
  DEPENDS capnpc_cpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/definitions/graph.capnp
)

add_executable(cpp-serialization-benchmark
  ${CMAKE_CURRENT_BINARY_DIR}/graph.capnp.c++
  ${CMAKE_CURRENT_BINARY_DIR}/graph_generated.h
  src/main.cc
)
target_link_libraries(cpp-serialization-benchmark
  benchmark
  cista
  flatbuffers
  capnp
  cereal
)
target_include_directories(cpp-serialization-benchmark PUBLIC
  include
  ${CMAKE_CURRENT_BINARY_DIR}
)