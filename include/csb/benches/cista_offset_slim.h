#pragma once

#include "cista.h"

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

struct cista_offset_slim_bench {
  struct graph {
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
      uint32_t id_;
      cista::offset::string name_;
      cista::offset::vector<edge> out_;
      cista::offset::vector<edge> in_;
    };

    using node_t = node const*;
    using edge_t = edge;

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
  };

  void serialize() {
    graph g;
    g.nodes_.resize(GRAPH.nodes_.size());

    auto node_id = 0u;
    for (auto const& node : GRAPH.nodes_) {
      auto& n = g.nodes_[node_id++];
      n.name_.set_non_owning(node->name_.c_str(), node->name_.length());
      n.id_ = node->id_;
      n.out_ = cista::offset::to_vec(node->out_edges_, [](auto&& e) {
        return graph::edge{static_cast<uint16_t>(e->from_->id_),
                           static_cast<uint16_t>(e->to_->id_), e->weight_};
      });
      n.in_ = cista::offset::to_vec(node->in_edges_, [](auto&& e) {
        return graph::edge{static_cast<uint16_t>(e->from_->id_),
                           static_cast<uint16_t>(e->to_->id_), e->weight_};
      });
    }
    buf_ = cista::serialize(g);
  }
  void deserialize() { g_ = cista::offset::deserialize<graph>(buf_); }
  void deserialize_fast() {
    g_ = cista::offset::unchecked_deserialize<graph>(buf_);
  }
  unsigned traverse() { return traverse_forward(*g_); }
  static constexpr bool skip_fast_deserialize() { return true; }
  size_t serialized_size() const { return buf_.size(); }

  std::vector<uint8_t> buf_;
  graph* g_;
};

}  // namespace csb