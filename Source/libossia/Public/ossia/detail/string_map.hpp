#pragma once
#include <ossia/detail/murmur3.hpp>
#include <ossia/detail/string_view.hpp>

#include <tsl/hopscotch_map.h>

#include <string>

/**
 * \file string_map.hpp
 *
 * Efficient hash maps for string and string_view
 */
namespace ossia
{
struct string_hash
{
  std::size_t operator()(const std::string& s) const noexcept
  {
    uint32_t seed = 0;
    ossia::murmur::murmur3_x86_32(s.data(), s.size(), seed, seed);
    return seed;
  }

  std::size_t operator()(std::string_view s) const noexcept
  {
    uint32_t seed = 0;
    ossia::murmur::murmur3_x86_32(s.data(), s.size(), seed, seed);
    return seed;
  }

  template <std::size_t N>
  std::size_t operator()(const char (&s)[N]) const noexcept
  {
    uint32_t seed = 0;
    ossia::murmur::murmur3_x86_32(s, N - 1, seed, seed);
    return seed;
  }
};

struct string_equal
{
  using is_transparent = std::true_type;
  bool operator()(const std::string& s, const std::string& s2) const noexcept
  {
    return s == s2;
  }
  bool operator()(ossia::string_view s, const std::string& s2) const noexcept
  {
    return s == s2;
  }
  bool operator()(const std::string& s, ossia::string_view s2) const noexcept
  {
    return s == s2;
  }
  bool operator()(ossia::string_view s, ossia::string_view s2) const noexcept
  {
    return s == s2;
  }

  template <std::size_t N>
  bool operator()(const std::string& s, const char (&s2)[N]) const noexcept
  {
    return operator()(s, make_string_view<N>(s2));
  }

  template <std::size_t N>
  bool operator()(ossia::string_view s, const char (&s2)[N]) const noexcept
  {
    return operator()(s, make_string_view<N>(s2));
  }

  template <std::size_t N>
  bool operator()(const char (&s)[N], const std::string& s2) const noexcept
  {
    return operator()(make_string_view<N>(s), s2);
  }

  template <std::size_t N>
  bool operator()(const char (&s)[N], ossia::string_view s2) const noexcept
  {
    return operator()(make_string_view<N>(s), s2);
  }
};

template <typename Value>
using string_map = tsl::hopscotch_map<
    std::string, Value, string_hash, string_equal,
    std::allocator<std::pair<std::string, Value>>, 4>;
template <typename Value>
using string_view_map = tsl::hopscotch_map<
    ossia::string_view, Value, string_hash, string_equal,
    std::allocator<std::pair<ossia::string_view, Value>>, 4>;
}
