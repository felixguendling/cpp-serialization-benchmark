#pragma once

#include <algorithm>
#include <iterator>
#include <vector>

namespace utl {

template <typename Container, typename UnaryOperation>
inline auto to_vec(Container const& c, UnaryOperation&& op)
    -> std::vector<decltype(op(*std::begin(c)))> {
  std::vector<decltype(op(*std::begin(c)))> v;
  v.reserve(std::distance(std::begin(c), std::end(c)));
  std::transform(std::begin(c), std::end(c), std::back_inserter(v), op);
  return v;
}

}  // namespace utl