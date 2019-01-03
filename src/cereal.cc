#include <sstream>

#include "cereal/archives/binary.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

struct cereal_graph {
  struct edge {
    template <class A>
    void serialize(A& a) {
      a(from_, to_, weight_);
    }

    uint16_t from_, to_;
    uint16_t weight_;
  };
  struct node {
    template <class A>
    void serialize(A& a) {
      a(id_, name_, out_, in_);
    }

    uint16_t id_;
    std::string name_;
    std::vector<edge> out_;
    std::vector<edge> in_;
  };

  template <class A>
  void serialize(A& a) {
    a(nodes_);
  }

  std::vector<node> nodes_;
  uint16_t next_node_id_{0};
};

int main(int argc, char**) {
  std::stringstream ss;

  cereal_graph g;
  g.nodes_.push_back(cereal_graph::node{static_cast<uint16_t>(argc)});
  cereal::BinaryOutputArchive oarchive(ss);
  oarchive(g);

  cereal_graph deserialized;
  cereal::BinaryInputArchive iarchive(ss);
  iarchive(deserialized);

  printf("%d\n", deserialized.nodes_[0].id_);
}