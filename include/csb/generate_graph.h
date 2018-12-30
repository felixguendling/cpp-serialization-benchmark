#pragma once

#include <memory>
#include <random>
#include <string>
#include <vector>

namespace csb {

template <typename Node>
struct basic_edge {
  using node_ptr = typename Node::template ptr_t<Node>;
  node_ptr from_{nullptr}, to_{nullptr};
  unsigned weight_{0u};
};

template <typename String,  //
          template <typename> typename Vector,  //
          template <typename> typename Ptr>
struct basic_node {
  using edge_t = basic_edge<basic_node>;

  template <typename T>
  using ptr_t = Ptr<T>;

  unsigned id_{0u};
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

  Ptr<graph_edge_t> make_edge(unsigned const from_id, unsigned const to_id,
                              unsigned const weight) {
    auto const from = nodes_[from_id].get();
    auto const to = nodes_[to_id].get();
    auto const e =
        edges_
            .emplace_back(MakeUnique::template make_unique<graph_edge_t>(
                graph_edge_t{from, to, weight}))
            .get();
    from->in_edges_.emplace_back(e);
    to->out_edges_.emplace_back(e);
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
  unsigned next_node_id_{0};
};  // namespace csb

template <typename T>
using raw_ptr = T*;

struct std_make_unique {
  template <typename T, typename... Args>
  static std::unique_ptr<T> make_unique(Args... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }
};

template <typename T>
using std_vector = std::vector<T>;

using std_string = std::string;

template <typename T>
using std_unique_ptr = std::unique_ptr<T>;

using std_graph = basic_graph<std_vector, std_unique_ptr, raw_ptr, std_string,
                              std_make_unique>;

template <typename Graph>
inline Graph generate_graph(unsigned const size, double const connectedness) {
  auto rng = std::mt19937{};
  auto is_connected =
      [&connectedness, &rng,
       dist = std::uniform_int_distribution<double>{0.0, 1.0}]() mutable {
        return dist(rng) <= connectedness;
      };
  auto name = [&rng, char_dist = std::uniform_int_distribution<char>{'a', 'z'},
               length_dist =
                   std::uniform_int_distribution<unsigned>{5, 20}]() mutable {
    typename Graph::string_t s;
    s.resize(length_dist(rng));
    for (auto& c : s) {
      c = char_dist(rng);
    }
    return s;
  };
  auto edge_weight = [&rng, dist = std::uniform_int_distribution<unsigned>{
                                0, 1000}]() mutable { return dist(rng); };

  Graph g;
  for (auto i = 0u; i < size; ++i) {
    g.make_node(name());
  }
  for (auto i = 0; i < size; ++i) {
    for (auto j = 0; j < size; ++j) {
      if (is_connected()) {
        g.make_edge(i, j, edge_weight());
      }
    }
  }
  return g;
}

}  // namespace csb