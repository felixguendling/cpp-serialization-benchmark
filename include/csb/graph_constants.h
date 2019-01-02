#pragma once

namespace csb {

constexpr auto const GRAPH_SIZE = 1870u;
constexpr auto const CONNECTEDNESS = 0.9;

// Node IDs: 11 bits (2048 max)
// Edge weights: 10 bits (1024 max)
static_assert(GRAPH_SIZE < 2048);

}  // namespace csb