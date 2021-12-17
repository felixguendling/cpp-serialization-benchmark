project(cpp-serialization-benchmark)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

set(GRAPH_SIZE 1870 CACHE STRING "Node count")
set(CONNECTEDNESS 90 CACHE STRING "To how many nodes is a node connected [0-100]")
configure_file(include/csb/graph_constants.h.in graph_constants.h)

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
  ${CMAKE_CURRENT_BINARY_DIR}/graph.capnp.h
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

add_executable(cista-compile-time EXCLUDE_FROM_ALL src/cista.cc)
target_link_libraries(cista-compile-time cista)

add_executable(capnp-compile-time EXCLUDE_FROM_ALL
  ${CMAKE_CURRENT_BINARY_DIR}/graph.capnp.h
  ${CMAKE_CURRENT_BINARY_DIR}/graph.capnp.c++
  src/capnp.cc)
target_link_libraries(capnp-compile-time capnp)
target_include_directories(capnp-compile-time PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR})

add_executable(cereal-compile-time EXCLUDE_FROM_ALL src/cereal.cc)
target_link_libraries(cereal-compile-time cereal)

add_executable(fbs-compile-time EXCLUDE_FROM_ALL
  src/fbs.cc
  ${CMAKE_CURRENT_BINARY_DIR}/graph_generated.h
)
target_link_libraries(fbs-compile-time flatbuffers)
target_include_directories(fbs-compile-time PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR})
