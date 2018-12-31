@0xed66c9fca5becdb8;

struct Graph {
  struct Edge {
    from @0 :UInt16;
    to @1 :UInt16;
    weight @2 :UInt16;
  }

  struct Node {
    id @0 :UInt16;
    name @1 :Text;
    outEdges @2 :List(Edge);
    inEdges @3 :List(Edge);
  }

  nodes @0 :List(Node);
}