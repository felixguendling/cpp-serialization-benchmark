#pragma once

#include "utl/to_map.h"
#include "utl/to_vec.h"

#include "csb/std_graph.h"
#include "csb/traverse_graph.h"

#include "graph_generated.h"

namespace csb {

struct fbs_bench {
  struct wrapper {
    using node_t = graph::Node const*;
    using edge_t = graph::Edge const*;

    wrapper(uint8_t const* serialized) : graph_{graph::GetGraph(serialized)} {}

    template <typename Fn>
    static void for_each_out_edge(node_t n, Fn&& f) {
      for (auto const& e : *n->out_edges()) {
        f(e);
      }
    }

    template <typename Fn>
    static void for_each_in_edge(node_t n, Fn&& f) {
      for (auto const& e : *n->in_edges()) {
        f(e);
      }
    }

    node_t get_target(edge_t e) const {
      return graph_->nodes()->Get(e->to_node_id());
    }

    node_t get_node(unsigned node_id) const {
      return graph_->nodes()->Get(node_id);
    }

    graph::Graph const* graph_;
  };

  void serialize() {
    auto const edges = utl::to_map(GRAPH.edges_, [&](auto&& e) {
      return std::pair{e.get(), graph::CreateEdge(fbb_, e->from_->id_,
                                                  e->to_->id_, e->weight_)};
    });
    fbb_.Finish(graph::CreateGraph(
        fbb_, fbb_.CreateVector(utl::to_vec(GRAPH.nodes_, [&](auto&& n) {
          return graph::CreateNode(
              fbb_, n->id_, fbb_.CreateString(n->name_),
              fbb_.CreateVector(utl::to_vec(
                  n->out_edges_, [&](auto&& e) { return edges.at(e); })),
              fbb_.CreateVector(utl::to_vec(
                  n->in_edges_, [&](auto&& e) { return edges.at(e); })));
        }))));
  }

  void deserialize() {
    auto v = flatbuffers::Verifier{fbb_.GetBufferPointer(), fbb_.GetSize()};
    graph::VerifyGraphBuffer(v);
  }

  void deserialize_fast() {}

  unsigned traverse() {
    return traverse_forward(wrapper(fbb_.GetBufferPointer()));
  }

  size_t serialized_size() const { return fbb_.GetSize(); }

  void backup() {}
  void restore() {}

  flatbuffers::FlatBufferBuilder fbb_;
};

}  // namespace csb