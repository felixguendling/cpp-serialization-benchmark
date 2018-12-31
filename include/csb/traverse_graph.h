#pragma once

#include <set>
#include <vector>

namespace csb {

template <typename Graph>
unsigned traverse_forward(Graph const& g) {
  std::set<typename Graph::node_t> visited;
  std::vector<typename Graph::node_t> todo;
  todo.emplace_back(g.get_node(0));

  while (!todo.empty()) {
    auto const next = todo.back();
    todo.resize(todo.size() - 1);

    auto edge_count = 0u;
    Graph::for_each_out_edge(next, [&](auto&& e) {
      ++edge_count;
      auto const target = g.get_target(e);
      if (visited.emplace(target).second) {
        todo.emplace_back(target);
      } else {
      }
    });
  }

  return visited.size();
}

}  // namespace csb