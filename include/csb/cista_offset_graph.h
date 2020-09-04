#pragma once

#include "cista/serialization.h"

#include "csb/generate_graph.h"
#include "csb/graph.h"

#include "graph_constants.h"

namespace csb {

struct cista_offset_make_unique {
  template <typename T, typename... Args>
  static cista::offset::unique_ptr<T> make_unique(Args... args) {
    return cista::offset::make_unique<T>(std::forward<Args>(args)...);
  }
};

using cista_offset_graph =
    csb::basic_graph<cista::offset::vector, cista::offset::unique_ptr,
                     cista::offset::ptr, cista::offset::string,
                     cista_offset_make_unique>;

static auto const CISTA_OFFSET_GRAPH =
    csb::generate_graph<cista_offset_graph>(GRAPH_SIZE, CONNECTEDNESS);

}  // namespace csb