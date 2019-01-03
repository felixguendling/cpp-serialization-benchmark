#include "cista.h"

struct slim_graph {
  using string_t = cista::offset::string;

  struct edge {
    uint32_t from_ : 11;
    uint32_t to_ : 11;
    uint32_t weight_ : 10;
  };

  template <typename Ctx>
  friend void serialize(Ctx&, edge const*, cista::offset_t const) {}
  friend void deserialize(cista::deserialization_context const&, edge*) {}
  friend void unchecked_deserialize(cista::deserialization_context const&,
                                    edge*) {}

  struct node {
    uint16_t id_;
    uint16_t __fill_0_{0};
    uint32_t __fill_1_{0};
    cista::offset::string name_;
    cista::offset::vector<edge> out_;
    cista::offset::vector<edge> in_;
  };

  using node_t = node const*;
  using edge_t = edge;

  cista::offset::vector<node> nodes_;
  uint16_t next_node_id_{0};
};

int main(int argc, char**) {
  slim_graph g;
  g.nodes_.emplace_back(slim_graph::node{static_cast<uint16_t>(argc)});
  auto buf = cista::serialize(g);
  auto const deserialized = cista::offset::deserialize<slim_graph>(buf);
  printf("%d\n", deserialized->nodes_[0].id_);
}