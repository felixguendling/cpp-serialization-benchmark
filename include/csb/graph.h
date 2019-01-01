#pragma once

namespace csb {

template <typename Node>
struct basic_edge {
  using node_ptr = typename Node::template ptr_t<Node>;
  node_ptr from_{nullptr}, to_{nullptr};
  uint16_t weight_{0u};
};

template <typename String,  //
          template <typename> typename Vector,  //
          template <typename> typename Ptr>
struct basic_node {
  using edge_t = basic_edge<basic_node>;

  template <typename T>
  using ptr_t = Ptr<T>;

  uint16_t id_{0u};
  String name_;
  Vector<Ptr<edge_t>> out_edges_;
  Vector<Ptr<edge_t>> in_edges_;
};

template <template <typename> typename Vector,  //
          template <typename> typename UniquePtr,  //
          template <typename> typename Ptr,  //
          typename String,  //
          typename MakeUnique>
struct basic_graph {
  using string_t = String;
  using graph_node_t = basic_node<String, Vector, Ptr>;
  using graph_edge_t = basic_edge<graph_node_t>;
  using node_t = graph_node_t const*;
  using edge_t = graph_edge_t const*;

  Ptr<graph_node_t> make_node(String name) {
    return nodes_
        .emplace_back(MakeUnique::template make_unique<graph_node_t>(
            graph_node_t{next_node_id_++, std::move(name)}))
        .get();
  }

  Ptr<graph_edge_t> make_edge(uint16_t const from_id, uint16_t const to_id,
                              uint16_t const weight) {
    auto const from = nodes_[from_id].get();
    auto const to = nodes_[to_id].get();
    auto const e =
        edges_
            .emplace_back(MakeUnique::template make_unique<graph_edge_t>(
                graph_edge_t{from, to, weight}))
            .get();
    from->out_edges_.emplace_back(e);
    to->in_edges_.emplace_back(e);
    return e;
  }

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
  node_t get_target(edge_t e) const { return e->to_; }
  node_t get_node(unsigned node_id) const { return nodes_[node_id].get(); }

  Vector<UniquePtr<graph_node_t>> nodes_;
  Vector<UniquePtr<graph_edge_t>> edges_;
  uint16_t next_node_id_{0};
};

}  // namespace csb