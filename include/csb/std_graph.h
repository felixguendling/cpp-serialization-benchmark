#pragma once

#include <memory>
#include <string>
#include <vector>

#include "csb/generate_graph.h"
#include "csb/graph.h"
#include "csb/graph_constants.h"

namespace csb {

template <typename T>
using raw_ptr = T*;

struct std_make_unique {
  template <typename T, typename... Args>
  static std::unique_ptr<T> make_unique(Args... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }
};

template <typename T>
using std_vector = std::vector<T>;

using std_string = std::string;

template <typename T>
using std_unique_ptr = std::unique_ptr<T>;

using std_graph = basic_graph<std_vector, std_unique_ptr, raw_ptr, std_string,
                              std_make_unique>;

static auto const GRAPH = generate_graph<std_graph>(GRAPH_SIZE, CONNECTEDNESS);

}  // namespace csb