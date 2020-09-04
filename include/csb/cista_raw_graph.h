#pragma once

#include "cista/serialization.h"

#include "csb/generate_graph.h"
#include "csb/graph.h"

#include "graph_constants.h"

namespace csb {

struct cista_raw_make_unique {
  template <typename T, typename... Args>
  static cista::raw::unique_ptr<T> make_unique(Args... args) {
    return cista::raw::make_unique<T>(std::forward<Args>(args)...);
  }
};

using cista_raw_graph =
    csb::basic_graph<cista::raw::vector, cista::raw::unique_ptr,
                     cista::raw::ptr, cista::raw::string,
                     cista_raw_make_unique>;

static auto const CISTA_RAW_GRAPH =
    csb::generate_graph<cista_raw_graph>(GRAPH_SIZE, CONNECTEDNESS);

}  // namespace csb