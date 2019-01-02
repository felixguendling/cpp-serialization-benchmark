#pragma once

#include "cista.h"

#include "csb/generate_graph.h"
#include "csb/graph_constants.h"
#include "csb/std_graph.h"
#include "csb/traverse_graph.h"

namespace cista::offset {

template <typename Container, typename UnaryOperation>
inline auto to_vec(Container const& c, UnaryOperation&& op)
    -> vector<decltype(op(*std::begin(c)))> {
  vector<decltype(op(*std::begin(c)))> v;
  v.reserve(std::distance(std::begin(c), std::end(c)));
  std::transform(std::begin(c), std::end(c), std::back_inserter(v), op);
  return v;
}

}  // namespace cista::offset

namespace csb {

struct slim_graph {
  using string_t = cista::offset::string;

  struct edge {
    uint32_t from_ : 11;
    uint32_t to_ : 11;
    uint32_t weight_ : 10;
  };

  template <typename Ctx>
  friend void serialize(Ctx&, edge const*, cista::offset_t const) {}
  friend void deserialize(cista::deserialization_context const&, edge*) {}
  friend void unchecked_deserialize(cista::deserialization_context const&,
                                    edge*) {}

  struct node {
    uint16_t id_;
    uint16_t __fill_0_{0};
    uint32_t __fill_1_{0};
    cista::offset::string name_;
    cista::offset::vector<edge> out_;
    cista::offset::vector<edge> in_;
  };

  using node_t = node const*;
  using edge_t = edge;

  void make_node(cista::offset::string name) {
    nodes_.emplace_back(node{next_node_id_++, 0, 0, std::move(name)});
  }

  void make_edge(uint16_t const from_id, uint16_t const to_id,
                 uint16_t const weight) {
    nodes_[from_id].out_.emplace_back(edge{from_id, to_id, weight});
    nodes_[to_id].in_.emplace_back(edge{from_id, to_id, weight});
  }

  template <typename Fn>
  static void for_each_out_edge(node_t n, Fn&& f) {
    for (auto const& e : n->out_) {
      f(e);
    }
  }

  template <typename Fn>
  static void for_each_in_edge(node_t n, Fn&& f) {
    for (auto const& e : n->in_) {
      f(e);
    }
  }

  node_t get_target(edge_t e) const { return &nodes_[e.to_]; }
  node_t get_node(unsigned node_id) const { return &nodes_[node_id]; }

  cista::offset::vector<node> nodes_;
  uint16_t next_node_id_{0};
};

static slim_graph SLIM_GRAPH =
    generate_graph<slim_graph>(GRAPH_SIZE, CONNECTEDNESS);

struct cista_offset_slim_bench {
  void serialize() { buf_ = cista::serialize(SLIM_GRAPH); }
  void deserialize() { g_ = cista::offset::deserialize<slim_graph>(buf_); }
  void deserialize_fast() {
    g_ = cista::offset::unchecked_deserialize<slim_graph>(buf_);
  }
  unsigned traverse() { return traverse_forward(*g_); }
  size_t serialized_size() const { return buf_.size(); }

  void backup() {}
  void restore() {}

  std::vector<uint8_t> buf_;
  slim_graph* g_;
};

}  // namespace csb