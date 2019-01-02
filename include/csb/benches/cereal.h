#pragma once

#include "utl/to_vec.h"

#include "cereal/archives/binary.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

#include "csb/generate_graph.h"
#include "csb/std_graph.h"
#include "csb/traverse_graph.h"

#include "graph_constants.h"

namespace csb {

struct cereal_graph {
  struct edge {
    template <class A>
    void serialize(A& a) {
      a(from_, to_, weight_);
    }

    uint16_t from_, to_;
    uint16_t weight_;
  };
  struct node {
    template <class A>
    void serialize(A& a) {
      a(id_, name_, out_, in_);
    }

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

  template <class A>
  void serialize(A& a) {
    a(nodes_);
  }

  std::vector<node> nodes_;
  uint16_t next_node_id_{0};
};

static cereal_graph CEREAL_GRAPH =
    generate_graph<cereal_graph>(GRAPH_SIZE, CONNECTEDNESS);

struct cereal_bench {
  void serialize() {
    ss_.str("");
    cereal::BinaryOutputArchive oarchive(ss_);
    oarchive(CEREAL_GRAPH);
  }
  void deserialize() {
    cereal::BinaryInputArchive iarchive(ss_);
    iarchive(deserialized_);
  }
  void deserialize_fast() { deserialize(); }
  unsigned traverse() { return traverse_forward(deserialized_); }
  size_t serialized_size() const { return ss_.str().size(); }
  void backup() { backup_ = ss_.str(); }
  void restore() { ss_.str(backup_); }

  std::stringstream ss_;
  std::string backup_;
  cereal_graph deserialized_;
};

}  // namespace csb