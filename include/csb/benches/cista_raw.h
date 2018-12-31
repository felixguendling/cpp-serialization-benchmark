#pragma once

#include "cista.h"

#include "csb/cista_raw_graph.h"
#include "csb/traverse_graph.h"

namespace csb {

struct cista_raw_bench {
  void serialize() { buf_ = cista::serialize(CISTA_RAW_GRAPH); }
  void deserialize() { g_ = cista::raw::deserialize<cista_raw_graph>(buf_); }
  void deserialize_fast() {
    g_ = cista::raw::unchecked_deserialize<cista_raw_graph>(buf_);
  }
  unsigned traverse() { return traverse_forward(*g_); }
  static constexpr bool skip_fast_deserialize() { return false; }
  size_t serialized_size() const { return buf_.size(); }

  std::vector<uint8_t> buf_;
  cista_raw_graph* g_;
};

}  // namespace csb