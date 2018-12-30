#include <benchmark/benchmark.h>

#include "capnp/message.h"
#include "capnp/serialize.h"

#include "cereal/archives/binary.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

#include "utl/get_or_create.h"
#include "utl/to_map.h"
#include "utl/to_vec.h"

#include "csb/cista_offset_graph.h"
#include "csb/cista_raw_graph.h"
#include "csb/generate_graph.h"
#include "csb/traverse_graph.h"

#include "graph.capnp.h"
#include "graph_generated.h"

constexpr auto const GRAPH_SIZE = 2000u;
constexpr auto const CONNECTEDNESS = 0.9;
static auto const GRAPH =
    csb::generate_graph<csb::std_graph>(GRAPH_SIZE, CONNECTEDNESS);

struct cista_raw_make_unique {
  template <typename T, typename... Args>
  static cista::raw::unique_ptr<T> make_unique(Args... args) {
    return cista::raw::make_unique<T>(std::forward<Args>(args)...);
  }
};

using cista_raw_graph =
    csb::basic_graph<cista::raw::vector, cista::raw::unique_ptr, csb::raw_ptr,
                     cista::raw::string, cista_raw_make_unique>;

static auto const CISTA_RAW_GRAPH =
    csb::generate_graph<cista_raw_graph>(GRAPH_SIZE, CONNECTEDNESS);

struct cista_offset_make_unique {
  template <typename T, typename... Args>
  static cista::offset::unique_ptr<T> make_unique(Args... args) {
    return cista::offset::make_unique<T>(std::forward<Args>(args)...);
  }
};

using cista_offset_graph =
    csb::basic_graph<cista::offset::vector, cista::offset::unique_ptr,
                     cista::offset_ptr, cista::offset::string,
                     cista_offset_make_unique>;

static auto const CISTA_OFFSET_GRAPH =
    csb::generate_graph<cista_offset_graph>(GRAPH_SIZE, CONNECTEDNESS);

struct cereal_make_unique {
  template <typename T, typename... Args>
  static std::shared_ptr<T> make_unique(Args... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }
};

struct fbs_bench {
  struct wrapper {
    using node_t = graph::Node const*;
    using edge_t = graph::Edge const*;

    wrapper(uint8_t const* serialized) : graph_{graph::GetGraph(serialized)} {}

    template <typename Fn>
    static void for_each_out_edge(node_t n, Fn&& f) {
      for (auto const& e : *n->out_edges()) {
        f(e);
      }
    }

    template <typename Fn>
    static void for_each_in_edge(node_t n, Fn&& f) {
      for (auto const& e : *n->in_edges()) {
        f(e);
      }
    }

    node_t get_target(edge_t e) const {
      return graph_->nodes()->Get(e->to_node_id());
    }

    node_t get_node(unsigned node_id) const {
      return graph_->nodes()->Get(node_id);
    }

    graph::Graph const* graph_;
  };

  void serialize() {
    auto const edges = utl::to_map(GRAPH.edges_, [&](auto&& e) {
      return std::pair{e.get(), graph::CreateEdge(fbb_, e->from_->id_,
                                                  e->to_->id_, e->weight_)};
    });
    fbb_.Finish(graph::CreateGraph(
        fbb_, fbb_.CreateVector(utl::to_vec(GRAPH.nodes_, [&](auto&& n) {
          return graph::CreateNode(
              fbb_, n->id_, fbb_.CreateString(n->name_),
              fbb_.CreateVector(utl::to_vec(
                  n->out_edges_, [&](auto&& e) { return edges.at(e); })),
              fbb_.CreateVector(utl::to_vec(
                  n->in_edges_, [&](auto&& e) { return edges.at(e); })));
        }))));
  }

  void deserialize() {
    auto v = flatbuffers::Verifier{fbb_.GetBufferPointer(), fbb_.GetSize()};
    graph::VerifyGraphBuffer(v);
  }

  void deserialize_fast() {}

  void traverse() { csb::traverse_forward(wrapper(fbb_.GetBufferPointer())); }

  static constexpr bool skip_fast_deserialize() { return true; }

  size_t serialized_size() const { return fbb_.GetSize(); }

  flatbuffers::FlatBufferBuilder fbb_;
};

struct cista_raw_bench {
  void serialize() { buf_ = cista::serialize(CISTA_RAW_GRAPH); }
  void deserialize() { g_ = cista::raw::deserialize<cista_raw_graph>(buf_); }
  void deserialize_fast() {
    g_ = cista::raw::unchecked_deserialize<cista_raw_graph>(buf_);
  }
  void traverse() { csb::traverse_forward(*g_); }
  static constexpr bool skip_fast_deserialize() { return false; }
  size_t serialized_size() const { return buf_.size(); }

  std::vector<uint8_t> buf_;
  cista_raw_graph* g_;
};

struct cista_offset_bench {
  void serialize() { buf_ = cista::serialize(CISTA_OFFSET_GRAPH); }
  void deserialize() {
    g_ = cista::offset::deserialize<cista_offset_graph>(buf_);
  }
  void deserialize_fast() {
    g_ = cista::offset::unchecked_deserialize<cista_offset_graph>(buf_);
  }
  void traverse() { csb::traverse_forward(*g_); }
  static constexpr bool skip_fast_deserialize() { return true; }
  size_t serialized_size() const { return buf_.size(); }

  std::vector<uint8_t> buf_;
  cista_offset_graph* g_;
};

inline bool operator<(Graph::Node::Reader const& a,
                      Graph::Node::Reader const& b) {
  return a.getId() < b.getId();
}

struct capnp_bench {
  struct wrapper {
    using node_t = Graph::Node::Reader;
    using edge_t = Graph::Edge::Reader;

    wrapper(Graph::Reader g) : graph_{g} {}

    template <typename Fn>
    static void for_each_out_edge(node_t n, Fn&& f) {
      for (auto const& e : n.getOutEdges()) {
        f(e);
      }
    }

    template <typename Fn>
    static void for_each_in_edge(node_t n, Fn&& f) {
      for (auto const& e : n.getInEdges()) {
        f(e);
      }
    }

    node_t get_target(edge_t e) const { return graph_.getNodes()[e.getTo()]; }

    node_t get_node(unsigned node_id) const {
      return graph_.getNodes()[node_id];
    }

    Graph::Reader graph_;
  };

  void serialize() {
    capnp::MallocMessageBuilder message;
    auto graph = message.initRoot<Graph>();
    auto nodes = graph.initNodes(GRAPH.nodes_.size());

    auto node_idx = 0u;
    for (auto const& n : GRAPH.nodes_) {
      auto node = nodes[node_idx++];

      node.setId(n->id_);
      node.setName(n->name_);

      auto edge_idx = 0u;
      auto in_edges = node.initInEdges(n->in_edges_.size());
      for (auto const& e : n->in_edges_) {
        auto edge = in_edges[edge_idx++];
        edge.setFrom(e->from_->id_);
        edge.setTo(e->to_->id_);
        edge.setWeight(e->weight_);
      }

      edge_idx = 0;
      auto out_edges = node.initOutEdges(n->out_edges_.size());
      for (auto const& e : n->out_edges_) {
        auto edge = out_edges[edge_idx++];
        edge.setFrom(e->from_->id_);
        edge.setTo(e->to_->id_);
        edge.setWeight(e->weight_);
      }
    }

    serialized_ = capnp::messageToFlatArray(message);
  }

  void deserialize() {
    message_ = std::make_unique<capnp::FlatArrayMessageReader>(
        serialized_.asPtr(),
        capnp::ReaderOptions{std::numeric_limits<uint64_t>::max()});
    reader_ = message_->getRoot<Graph>();
  }

  void deserialize_fast() {
    // Not working.
    // reader_ = capnp::readMessageUnchecked<Graph>(serialized_.begin());

    deserialize();
  }

  void traverse() { csb::traverse_forward(wrapper{reader_}); }

  static constexpr bool skip_fast_deserialize() { return true; }

  size_t serialized_size() const { return serialized_.size(); }

  kj::Array<capnp::word> serialized_;
  std::unique_ptr<capnp::FlatArrayMessageReader> message_;
  Graph::Reader reader_;
};

struct cereal_bench {
  struct graph {
    struct edge {
      template <class A>
      void serialize(A& a) {
        a(from_, to_, weight_);
      }

      uint64_t from_, to_;
      uint32_t weight_;
    };
    struct node {
      template <class A>
      void serialize(A& a) {
        a(name_, id_, out_edges_, in_edges_);
      }

      std::string name_;
      uint32_t id_;
      std::vector<edge> out_edges_;
      std::vector<edge> in_edges_;
    };

    using node_t = node const*;
    using edge_t = edge;

    template <typename Fn>
    static void for_each_out_edge(node_t n, Fn&& f) {
      for (auto const& e : n->out_edges_) {
        f(e);
      }
    }

    template <typename Fn>
    static void for_each_in_edge(node_t n, Fn&& f) {
      for (auto const& e : n->in_edges_) {
        f(e);
      }
    }

    node_t get_target(edge_t e) const { return &nodes_[e.to_]; }
    node_t get_node(unsigned node_id) const { return &nodes_[node_id]; }

    template <class A>
    void serialize(A& a) {
      a(nodes_);
    }

    std::vector<node> nodes_;
  };

  void serialize() {
    cereal::BinaryOutputArchive oarchive(ss_);
    graph g;
    g.nodes_.resize(GRAPH.nodes_.size());

    auto node_id = 0u;
    for (auto const& node : GRAPH.nodes_) {
      auto& n = g.nodes_[node_id++];
      n.name_ = node->name_;
      n.id_ = node->id_;
      n.out_edges_ = utl::to_vec(node->out_edges_, [](auto&& e) {
        return graph::edge{e->from_->id_, e->to_->id_, e->weight_};
      });
      n.in_edges_ = utl::to_vec(node->in_edges_, [](auto&& e) {
        return graph::edge{e->from_->id_, e->to_->id_, e->weight_};
      });
    }

    oarchive(g);
  }

  void deserialize() {
    cereal::BinaryInputArchive iarchive(ss_);
    iarchive(deserialized_);
  }

  void deserialize_fast() { deserialize(); }

  void traverse() { csb::traverse_forward(deserialized_); }

  static constexpr bool skip_fast_deserialize() { return false; }

  size_t serialized_size() const { return ss_.str().size(); }

  std::stringstream ss_;
  graph deserialized_;
};

template <typename Bench>
void BM_SERIALIZE(benchmark::State& state) {
  Bench b;
  for (auto _ : state) {
    b.serialize();
  }
  state.counters["size"] = b.serialized_size();
}

template <typename Bench>
void BM_SAFE_DESERIALIZE(benchmark::State& state) {
  Bench b;
  for (auto _ : state) {
    state.PauseTiming();
    b.serialize();
    state.ResumeTiming();
    b.deserialize();
  }
}

template <typename Bench>
void BM_FAST_DESERIALIZE(benchmark::State& state) {
  if constexpr (Bench::skip_fast_deserialize()) {
    for (auto _ : state) {
    }
    return;
  }
  Bench b;
  for (auto _ : state) {
    state.PauseTiming();
    b.serialize();
    state.ResumeTiming();
    b.deserialize_fast();
  }
}

template <typename Bench>
void BM_TRAVERSE(benchmark::State& state) {
  Bench b;
  b.serialize();
  b.deserialize_fast();
  for (auto _ : state) {
    b.traverse();
  }
}

BENCHMARK_TEMPLATE(BM_SERIALIZE, capnp_bench)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SERIALIZE, cereal_bench)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SERIALIZE, fbs_bench)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SERIALIZE, cista_raw_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SERIALIZE, cista_offset_bench)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_TEMPLATE(BM_SAFE_DESERIALIZE, capnp_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SAFE_DESERIALIZE, fbs_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SAFE_DESERIALIZE, cista_offset_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SAFE_DESERIALIZE, cista_raw_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SAFE_DESERIALIZE, cereal_bench)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE, cista_offset_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE, fbs_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE, capnp_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE, cista_raw_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE, cereal_bench)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_TEMPLATE(BM_TRAVERSE, cereal_bench);
BENCHMARK_TEMPLATE(BM_TRAVERSE, cista_raw_bench);
BENCHMARK_TEMPLATE(BM_TRAVERSE, cista_offset_bench);
BENCHMARK_TEMPLATE(BM_TRAVERSE, capnp_bench);
BENCHMARK_TEMPLATE(BM_TRAVERSE, fbs_bench);

BENCHMARK_MAIN();