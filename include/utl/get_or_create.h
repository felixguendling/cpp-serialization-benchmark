#pragma once

namespace utl {

template <typename Map, typename K, typename CreateFun>
auto get_or_create(Map& m, K const& key, CreateFun&& f) ->
    typename Map::mapped_type& {
  auto const it = m.find(key);
  if (it == end(m)) {
    auto v = f();
    return m[key] = std::move(v);
  } else {
    return it->second;
  }
}

}  // namespace utl
