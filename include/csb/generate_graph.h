#pragma once

#include <random>

#include "csb/graph.h"

namespace csb {

template <typename Graph>
inline Graph generate_graph(unsigned const size, double const connectedness) {
  auto rng = std::mt19937{};
  auto is_connected = [&connectedness, &rng,
                       dist = std::uniform_int_distribution<uint32_t>{
                           0, std::numeric_limits<uint32_t>::max()}]() mutable {
    return dist(rng) /
               static_cast<double>(std::numeric_limits<uint32_t>::max()) <=
           connectedness;
  };
  auto name = [&rng, char_dist = std::uniform_int_distribution<char>{'a', 'z'},
               length_dist =
                   std::uniform_int_distribution<uint16_t>{5, 20}]() mutable {
    typename Graph::string_t s;
    s.resize(length_dist(rng));
    for (auto& c : s) {
      c = char_dist(rng);
    }
    return s;
  };
  auto edge_weight = [&rng, dist = std::uniform_int_distribution<uint16_t>{
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