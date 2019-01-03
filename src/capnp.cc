#include <cstdio>

#include "capnp/message.h"
#include "capnp/serialize.h"

#include "graph.capnp.h"

int main(int argc, char**) {
  capnp::MallocMessageBuilder message;
  auto graph = message.initRoot<Graph>();
  auto nodes = graph.initNodes(argc);
  auto node = nodes[0u];
  node.setId(argc);

  auto const serialized = capnp::messageToFlatArray(message);
  capnp::FlatArrayMessageReader r{serialized.asPtr()};
  auto const deserialized = r.getRoot<Graph>();
  printf("%d\n", deserialized.getNodes()[0].getId());
}