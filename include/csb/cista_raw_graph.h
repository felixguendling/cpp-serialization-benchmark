#pragma once

#include "cista.h"

namespace csb::cista::raw {

namespace data = ::cista::raw;

struct node;

using node_id_t = uint32_t;

struct edge {
  data::ptr<node> from_{nullptr}, to_{nullptr};
  unsigned weight_{0u};
};

struct node {
  node_id_t id_{0};
  node_id_t fill_{0};
  data::string name_;
  data::vector<data::ptr<edge>> out_edges_;
  data::vector<data::ptr<edge>> in_edges_;
};

struct graph {
  using node_t = node const*;
  using edge_t = edge const*;
  using string = data::string;

  template <typename Fn>
  static void for_each_out_edge(node const* n, Fn&& f) {
    for (auto const& e : n->out_edges_) {
      f(e);
    }
  }

  template <typename Fn>
  static void for_each_in_edge(node const* n, Fn&& f) {
    for (auto const& e : n->in_edges_) {
      f(e);
    }
  }

  node const* get_target(edge const* e) const { return e->to_; }

  node const* get_node(unsigned node_id) const { return nodes_[node_id].get(); }

  node* make_node(data::string name) {
    return nodes_
        .emplace_back(
            data::make_unique<node>(node{next_node_id_++, 0, std::move(name),
                                         data::vector<data::ptr<edge>>{0u},
                                         data::vector<data::ptr<edge>>{0u}}))
        .get();
  }

  edge* make_edge(node_id_t const from_id, node_id_t const to_id,
                  unsigned weight) {
    auto const from = nodes_[from_id].get();
    auto const to = nodes_[to_id].get();
    auto const e =
        edges_.emplace_back(data::make_unique<edge>(edge{from, to, weight}))
            .get();
    from->in_edges_.emplace_back(e);
    to->out_edges_.emplace_back(e);
    return e;
  }

  template <typename T, typename Container>
  static T* deserialize(Container& c) {
    return data::deserialize<T>(c);
  }

  template <typename T, typename Container>
  static T* deserialize_unchecked(Container& c) {
    return data::unchecked_deserialize<T>(c);
  }

  static constexpr bool skip_fast_deserialize() { return false; }

  data::vector<data::unique_ptr<node>> nodes_;
  data::vector<data::unique_ptr<edge>> edges_;
  node_id_t next_node_id_{0};
  node_id_t fill_{0};
};

}  // namespace csb::cista::raw