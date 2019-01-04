#include "graph_generated.h"

int main(int argc, char**) {
  flatbuffers::FlatBufferBuilder fbb;
  std::vector<flatbuffers::Offset<graph::Node>> nodes;
  nodes.push_back(graph::CreateNode(fbb, argc));
  graph::FinishGraphBuffer(fbb,
                           graph::CreateGraph(fbb, fbb.CreateVector(nodes)));
  auto v = flatbuffers::Verifier{fbb.GetBufferPointer(), fbb.GetSize()};
  if (graph::VerifyGraphBuffer(v)) {
    printf("%d\n",
           graph::GetGraph(fbb.GetBufferPointer())->nodes()->Get(0)->id());
    return 0;
  } else {
    printf("error!\n");
    return 1;
  }
}