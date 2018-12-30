#pragma once

#include <map>
#include <utility>

namespace utl {

template <typename C, typename Fn>
auto to_map(C const& c, Fn&& fn) {
  using input_t = typename C::value_type;
  using fn_return_t = decltype(fn(std::declval<input_t>()));
  using key_t = decltype(std::declval<fn_return_t>().first);
  using value_t = decltype(std::declval<fn_return_t>().second);
  std::map<key_t, value_t> m;
  for (auto const& el : c) {
    m.emplace(fn(el));
  }
  return m;
}

}  // namespace utl