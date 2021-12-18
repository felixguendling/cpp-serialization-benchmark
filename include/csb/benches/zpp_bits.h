#pragma once

#include "utl/to_vec.h"

#include "../../../external_lib/zpp_bits/zpp_bits.h"

#include "csb/generate_graph.h"
#include "csb/std_graph.h"
#include "csb/traverse_graph.h"

#include "graph_constants.h"

namespace csb {

struct zpp_bits_graph {
  struct edge {
    uint16_t from_, to_;
    uint16_t weight_;

    using serialize = zpp::bits::members<3>;
  };

  struct node {
    using serialize = zpp::bits::members<4>;

    uint16_t id_;
    std::string name_;
    std::vector<edge> out_;
    std::vector<edge> in_;
  };

  using node_t = node const*;
  using edge_t = edge;
  using string_t = std::string;

  void make_node(std::string name) {
    nodes_.emplace_back(node{next_node_id_++, std::move(name)});
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

  constexpr static auto serialize(auto& archive, auto& self) {
    return archive(self.nodes_);
  }

  std::vector<node> nodes_;
  uint16_t next_node_id_{0};
};

static zpp_bits_graph zpp_bits_GRAPH =
    generate_graph<zpp_bits_graph>(GRAPH_SIZE, CONNECTEDNESS);

struct zpp_bits_bench {
  void serialize() { (void)zpp::bits::out{buf_}(zpp_bits_GRAPH); }
  void deserialize() { (void)zpp::bits::in{buf_}(deserialized_); }
  void deserialize_fast() { (void)zpp::bits::in{buf_}(deserialized_); }
  unsigned traverse() { return traverse_forward(deserialized_); }
  size_t serialized_size() const { return buf_.size(); }

  void backup() {}
  void restore() {}

  std::vector<uint8_t> buf_;
  zpp_bits_graph deserialized_;
};
#if 0
struct zpp_bits_bench {
  void serialize() {
    ss_.str("");
    zpp_bits::BinaryOutputArchive oarchive(ss_);
    oarchive(zpp_bits_GRAPH);
  }
  void deserialize() {
    zpp_bits::BinaryInputArchive iarchive(ss_);
    iarchive(deserialized_);
  }
  void deserialize_fast() { deserialize(); }
  unsigned traverse() { return traverse_forward(deserialized_); }
  size_t serialized_size() const { return ss_.str().size(); }
  void backup() { backup_ = ss_.str(); }
  void restore() { ss_.str(backup_); }

  std::stringstream ss_;
  std::string backup_;
  zpp_bits_graph deserialized_;
};
#endif

}  // namespace csb
