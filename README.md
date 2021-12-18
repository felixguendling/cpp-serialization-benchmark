# C++ Serialization Benchmark  [![Build Status](https://travis-ci.org/felixguendling/cpp-serialization-benchmark.svg?branch=master)](https://travis-ci.org/felixguendling/cpp-serialization-benchmark)

This benchmark suite accompanies the public release of the [Cista++](https://cista.rocks/) serialization library.

This repository contains benchmarks for C++ (binary & high performance) serialization libraries.
The goal was to create a benchmark based on a non-trivial data structure.
In this case, we serialize, deserialize and traverse a graph (nodes and edges).
Since the goal was to have a data structure containing pointers, we choose an "object oriented" representation of a graph instead of a simple adjacency matrix.
Some frameworks do no support cyclic data structures. Thus, instead of having node pointers in the edge object, we just reference start and destination node by their index.
Benchmarks are based on the [Google Benchmark](https://github.com/google/benchmark) framework.

This repository compares the following C++ binary serialization libraries:

  - [Cap’n Proto](https://capnproto.org/capnp-tool.html)
  - [cereal](https://uscilab.github.io/cereal/index.html)
  - [Flatbuffers](https://google.github.io/flatbuffers/)
  - [Cista++](https://cista.rocks/)


# Other Benchmarks

  - Benchmarks de/serialization (Thrift, Protobuf, Boost.Serialization, Msgpack, Cereal, Avro, Capnproto, Flatbuffers, YAS) of [two arrays (numbers and strings)](https://github.com/thekvs/cpp-serializers/blob/master/test.fbs): https://github.com/thekvs/cpp-serializers
  - Rust de/serialization benchmarks (Cap’n Proto vs. Protocol Buffers): https://github.com/ChrisMacNaughton/proto_benchmarks
  - FlatBuffers benchmarks: https://google.github.io/flatbuffers/flatbuffers_benchmarks.html


# Build & Execute

To run the benchmarks you need a C++17 compatible compiler and CMake. Tested on Mac OS X (but Linux should be fine, too).

    git clone --recursive github.com:felixguendling/cpp-serialization-benchmark.git
    cd cpp-serialization-benchmark
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
    ./cpp-serialization-benchmark


# Results

| Library                                               | Serialize      | Deserialize     | Fast Deserialize |   Traverse | Deserialize & Traverse |      Size  |
| :---                                                  |           ---: |            ---: |             ---: |       ---: |                   ---: |       ---: |
| [Cap’n Proto](https://capnproto.org/capnp-tool.html)  |        76 ms   |    **0.00 ms**  |       **0.0 ms** |   216 ms   |               221 ms   |    50.5M   |
| [cereal](https://uscilab.github.io/cereal/index.html) |       216 ms   |    111.00 ms    |                - |   67 ms    |               174 ms   |    37.8M   |
| [Cista++](https://cista.rocks/) `offset`              |      **4 ms**  |      0.16 ms    |       **0.0 ms** |   67 ms    |             **66 ms**  |  **25.3M** |
| [Cista++](https://cista.rocks/) `raw`                 |       650 ms   |     24.80 ms    |        24.8 ms   |   66 ms    |               91 ms    |   176.4M   |
| [Flatbuffers](https://google.github.io/flatbuffers/)  |      1409 ms   |     35.70 ms    |       **0.0 ms** |   75 ms    |               75 ms    |    63.0M   |
| [zpp_bits](https://github.com/eyalz800/zpp_bits)      |      **4 ms**  |      6.58 ms    |           6.6 ms | **65 ms**  |               72 ms    |    37.8M   |

Cista++ `offset` describes the "slim" variant (where the edges use indices to reference source and target node instead of pointers).

Exact results can be found [here](https://github.com/felixguendling/cpp-serialization-benchmark/blob/master/benchmark_result.txt).

Benchmarks were run on Mac OS X, compiled with Clang 7.


## Compilation Times

Compilation times are measured with code generation but without building the code generators or static libraries (Cap’n Proto, Flatbuffers).

| Library                                               | clang-7 on Mac OS X |
| :---                                                  |                ---: |
| [Cap’n Proto](https://capnproto.org/capnp-tool.html)  |              0.440s |
| [cereal](https://uscilab.github.io/cereal/index.html) |              1.827s |
| [Cista++](https://cista.rocks/) `raw`                 |              1.351s |
| [Flatbuffers](https://google.github.io/flatbuffers/)  |              0.857s |


# Contribute

You have found a mistake/bug or want to contribute new benchmarks? Feel free to open an issue/pull request! :smiley:
