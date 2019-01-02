#pragma once

#include "capnp/message.h"
#include "capnp/serialize.h"

#include "csb/std_graph.h"
#include "csb/traverse_graph.h"

#include "graph.capnp.h"

inline bool operator<(Graph::Node::Reader const& a,
                      Graph::Node::Reader const& b) {
  return a.getId() < b.getId();
}

namespace csb {

struct capnp_bench {
  struct wrapper {
    using node_t = Graph::Node::Reader;
    using edge_t = Graph::Edge::Reader;

    wrapper(Graph::Reader g) : graph_{g} {}

    template <typename Fn>
    static void for_each_out_edge(node_t n, Fn&& f) {
      for (auto const& e : n.getOutEdges()) {
        f(e);
      }
    }

    template <typename Fn>
    static void for_each_in_edge(node_t n, Fn&& f) {
      for (auto const& e : n.getInEdges()) {
        f(e);
      }
    }

    node_t get_target(edge_t e) const { return graph_.getNodes()[e.getTo()]; }

    node_t get_node(unsigned node_id) const {
      return graph_.getNodes()[node_id];
    }

    Graph::Reader graph_;
  };

  void serialize() {
    capnp::MallocMessageBuilder message;
    auto graph = message.initRoot<Graph>();
    auto nodes = graph.initNodes(GRAPH.nodes_.size());

    auto node_idx = 0u;
    for (auto const& n : GRAPH.nodes_) {
      auto node = nodes[node_idx++];

      node.setId(n->id_);
      node.setName(n->name_);

      auto edge_idx = 0u;
      auto in_edges = node.initInEdges(n->in_edges_.size());
      for (auto const& e : n->in_edges_) {
        auto edge = in_edges[edge_idx++];
        edge.setFrom(e->from_->id_);
        edge.setTo(e->to_->id_);
        edge.setWeight(e->weight_);
      }

      edge_idx = 0;
      auto out_edges = node.initOutEdges(n->out_edges_.size());
      for (auto const& e : n->out_edges_) {
        auto edge = out_edges[edge_idx++];
        edge.setFrom(e->from_->id_);
        edge.setTo(e->to_->id_);
        edge.setWeight(e->weight_);
      }
    }

    serialized_ = capnp::messageToFlatArray(message);
  }

  void deserialize() {
    message_ = std::make_unique<capnp::FlatArrayMessageReader>(
        serialized_.asPtr(),
        capnp::ReaderOptions{std::numeric_limits<uint64_t>::max()});
    reader_ = message_->getRoot<Graph>();
  }

  void deserialize_fast() {
    // Not working.
    // reader_ = capnp::readMessageUnchecked<Graph>(serialized_.begin());

    deserialize();
  }

  unsigned traverse() { return traverse_forward(wrapper{reader_}); }

  size_t serialized_size() const { return serialized_.asBytes().size(); }

  void backup() {}
  void restore() {}

  kj::Array<capnp::word> serialized_;
  std::unique_ptr<capnp::FlatArrayMessageReader> message_;
  Graph::Reader reader_;
};

}  // namespace csb