project(cista)

add_library(cista INTERFACE)
target_include_directories(cista SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})