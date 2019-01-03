#include "graph_generated.h"

int main(int argc, char**) {
  flatbuffers::FlatBufferBuilder fbb;
  std::vector<flatbuffers::Offset<graph::Node>> nodes;
  nodes.push_back(graph::CreateNode(fbb, argc));
  fbb.Finish(graph::CreateGraph(fbb, fbb.CreateVector(nodes)));

  auto const g = reinterpret_cast<graph::Graph const*>(fbb.GetBufferPointer());
  printf("%d\n", g->nodes()->Get(0)->id());
}