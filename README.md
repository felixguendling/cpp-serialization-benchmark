# C++ Serialization Benchmark

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

| Library                                                 | Serialize    | Deserialize   | Fast Deserialize | Traverse      | Deserialize & Traverse |    Size  |
| :---                                                    |         ---: |          ---: |             ---: |          ---: |                   ---: |       ---: |
| [Cap’n Proto](https://capnproto.org/capnp-tool.html)    |        50 ms |      0.002 ms |           0.0 ms |      193.0 ms |               193.0 ms |    26.1M |
| [Flatbuffers](https://google.github.io/flatbuffers/)    |      1166 ms |     15.200 ms |           0.0 ms |       79.8 ms |                80.3 ms |    32.5M |
| [cereal](https://uscilab.github.io/cereal/index.html)   |       191 ms |    102.000 ms |                - |       72.5 ms |               177.0 ms |    78.0M |
| [Cista++](https://cista.rocks/) `offset`                |       105 ms |      0.048 ms |           0.0 ms |       71.0 ms |                70.7 ms |    13.1M |
| [Cista++](https://cista.rocks/) `raw`                   |      1839 ms |     34.000 ms |           7.2 ms |       68.0 ms |                75.6 ms |    90.9M |

Cista++ `offset` describes the "slim" variant (where the edges use indices to reference source and target node instead of pointers).

Benchmarks were run on Mac OS X, compiled with Clang 7.

# Contribute

You have found a mistake/bug or want to contribute new benchmarks? Feel free to open an issue/pull request! :smiley:
