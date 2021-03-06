#pragma once

#include "cista/serialization.h"

#include "csb/cista_offset_graph.h"
#include "csb/traverse_graph.h"

namespace csb {

struct cista_offset_bench {
  void serialize() { buf_ = cista::serialize(CISTA_OFFSET_GRAPH); }
  void deserialize() {
    g_ = cista::offset::deserialize<cista_offset_graph>(buf_);
  }
  void deserialize_fast() {
    g_ = cista::offset::unchecked_deserialize<cista_offset_graph>(buf_);
  }
  unsigned traverse() { return traverse_forward(*g_); }
  size_t serialized_size() const { return buf_.size(); }

  void backup() {}
  void restore() {}

  std::vector<uint8_t> buf_;
  cista_offset_graph* g_;
};

}  // namespace csb