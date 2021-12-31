#include "zpp_bits/zpp_bits.h"

struct zpp_bits_graph {
  struct edge {
    uint16_t from_, to_;
    uint16_t weight_;
  };

  struct node {
    uint16_t id_;
    std::string name_;
    std::vector<edge> out_;
    std::vector<edge> in_;
  };

  constexpr static auto serialize(auto& archive, auto& self) {
    return archive(self.nodes_);
  }

  std::vector<node> nodes_;
  uint16_t next_node_id_{0};
};

int main(int argc, char**) {
  zpp_bits_graph g, deserialized;
  g.nodes_.emplace_back(zpp_bits_graph::node{static_cast<uint16_t>(argc)});
  auto [data, in, out] = zpp::bits::data_in_out();
  (void)out(g);
  (void)in(g);
  printf("%d\n", deserialized.nodes_[0].id_);
}
