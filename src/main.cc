#include <benchmark/benchmark.h>

#include "csb/benches/capnp.h"
#include "csb/benches/cereal.h"
#include "csb/benches/cista_offset.h"
#include "csb/benches/cista_offset_slim.h"
#include "csb/benches/cista_raw.h"
#include "csb/benches/fbs.h"

using namespace csb;

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
  b.serialize();
  b.backup();
  for (auto _ : state) {
    state.PauseTiming();
    b.restore();
    state.ResumeTiming();

    b.deserialize();
  }
}

template <typename Bench>
void BM_FAST_DESERIALIZE(benchmark::State& state) {
  Bench b;
  b.serialize();
  b.backup();
  for (auto _ : state) {
    state.PauseTiming();
    b.restore();
    state.ResumeTiming();

    b.deserialize_fast();
  }
}

template <typename Bench>
void BM_FAST_DESERIALIZE_AND_TRAVERSE(benchmark::State& state) {
  Bench b;
  b.serialize();
  b.backup();
  auto node_count = 0u;
  for (auto _ : state) {
    state.PauseTiming();
    b.restore();
    state.ResumeTiming();

    b.deserialize_fast();
    node_count = b.traverse();
  }
  state.counters["nodeCount"] = node_count;
}

template <typename Bench>
void BM_TRAVERSE(benchmark::State& state) {
  Bench b;
  b.serialize();
  b.deserialize_fast();
  unsigned node_count;
  for (auto _ : state) {
    node_count = b.traverse();
  }
  state.counters["nodeCount"] = node_count;
}

BENCHMARK_TEMPLATE(BM_SERIALIZE, capnp_bench)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SERIALIZE, cista_offset_slim_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SERIALIZE, cereal_bench)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SERIALIZE, fbs_bench)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SERIALIZE, cista_raw_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SERIALIZE, cista_offset_bench)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_TEMPLATE(BM_SAFE_DESERIALIZE, capnp_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SAFE_DESERIALIZE, cista_offset_slim_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SAFE_DESERIALIZE, fbs_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SAFE_DESERIALIZE, cista_offset_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SAFE_DESERIALIZE, cista_raw_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_SAFE_DESERIALIZE, cereal_bench)
    ->Unit(benchmark::kMillisecond);

/* Nothing to benchmark here: it's essentially a pointer cast.
BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE, cista_offset_slim_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE, cista_offset_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE, fbs_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE, capnp_bench)
    ->Unit(benchmark::kMillisecond);
*/
BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE, cista_raw_bench)
    ->Unit(benchmark::kMillisecond);
// There is no "unsafe / fast" deserialize for cereal.
// BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE, cereal_bench)
//     ->Unit(benchmark::kMillisecond);

BENCHMARK_TEMPLATE(BM_TRAVERSE, cista_raw_bench)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_TRAVERSE, cista_offset_slim_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_TRAVERSE, cista_offset_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_TRAVERSE, cereal_bench)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_TRAVERSE, fbs_bench)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_TRAVERSE, capnp_bench)->Unit(benchmark::kMillisecond);

BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE_AND_TRAVERSE, cista_offset_slim_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE_AND_TRAVERSE, cista_offset_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE_AND_TRAVERSE, cista_raw_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE_AND_TRAVERSE, fbs_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE_AND_TRAVERSE, cereal_bench)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_FAST_DESERIALIZE_AND_TRAVERSE, capnp_bench)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();