#pragma once

#include "utl/to_vec.h"

#include "cereal/archives/binary.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

#include "csb/std_graph.h"
#include "csb/traverse_graph.h"

namespace csb {

struct cereal_bench {
  struct graph {
    struct edge {
      template <class A>
      void serialize(A& a) {
        a(from_, to_, weight_);
      }

      uint64_t from_, to_;
      uint32_t weight_;
    };
    struct node {
      template <class A>
      void serialize(A& a) {
        a(name_, id_, out_edges_, in_edges_);
      }

      std::string name_;
      uint32_t id_;
      std::vector<edge> out_edges_;
      std::vector<edge> in_edges_;
    };

    using node_t = node const*;
    using edge_t = edge;

    template <typename Fn>
    static void for_each_out_edge(node_t n, Fn&& f) {
      for (auto const& e : n->out_edges_) {
        f(e);
      }
    }

    template <typename Fn>
    static void for_each_in_edge(node_t n, Fn&& f) {
      for (auto const& e : n->in_edges_) {
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
  };

  void serialize() {
    cereal::BinaryOutputArchive oarchive(ss_);
    graph g;
    g.nodes_.resize(GRAPH.nodes_.size());

    auto node_id = 0u;
    for (auto const& node : GRAPH.nodes_) {
      auto& n = g.nodes_[node_id++];
      n.name_ = node->name_;
      n.id_ = node->id_;
      n.out_edges_ = utl::to_vec(node->out_edges_, [](auto&& e) {
        return graph::edge{e->from_->id_, e->to_->id_, e->weight_};
      });
      n.in_edges_ = utl::to_vec(node->in_edges_, [](auto&& e) {
        return graph::edge{e->from_->id_, e->to_->id_, e->weight_};
      });
    }

    oarchive(g);
  }

  void deserialize() {
    cereal::BinaryInputArchive iarchive(ss_);
    iarchive(deserialized_);
  }

  void deserialize_fast() { deserialize(); }

  unsigned traverse() { return traverse_forward(deserialized_); }

  static constexpr bool skip_fast_deserialize() { return false; }

  size_t serialized_size() const { return ss_.str().size(); }

  std::stringstream ss_;
  graph deserialized_;
};

}  // namespace csb