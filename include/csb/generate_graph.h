#pragma once

#include <random>

#include "csb/graph.h"

namespace csb {

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