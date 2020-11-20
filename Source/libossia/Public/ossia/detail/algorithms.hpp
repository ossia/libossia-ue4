#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/string_view.hpp>

#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <tuple>
#include <utility>

#include <type_traits>

/**
 * \file algorithms.hpp
 *
 * This header contains various range-style functions that mimic std::algorithm
 * functions.
 * This won't be necessary anymore when ranges are introduced in C++20
 * (hopefully).
 */
namespace ossia
{
template <typename Vector>
using iterator_t = typename std::remove_reference<Vector>::type::iterator;

template <typename Vector, typename Value>
auto find(Vector&& v, const Value& val) noexcept
{
  return std::find(std::begin(v), std::end(v), val);
}

template <typename Vector, typename Fun>
auto find_if(Vector&& v, Fun fun)
{
  return std::find_if(std::begin(v), std::end(v), fun);
}

template <typename Vector, typename Value>
auto* ptr_find(Vector&& v, const Value& val) noexcept
{
  auto it = std::find(std::begin(v), std::end(v), val);
  return it != std::end(v) ? &*it : nullptr;
}

template <typename Vector, typename Fun>
auto* ptr_find_if(Vector&& v, Fun fun)
{
  auto it = std::find_if(std::begin(v), std::end(v), fun);
  return it != std::end(v) ? &*it : nullptr;
}

template <typename Vector, typename Value>
bool contains(Vector&& v, const Value& val) noexcept
{
  return find(v, val) != std::end(v);
}

template <typename Vector, typename Value>
void remove_one(Vector&& v, const Value& val)
{
  auto it = find(v, val);
  if (it != v.end())
  {
    v.erase(it);
  }
}

template <typename Vector, typename Function>
void remove_one_if(Vector& v, const Function& val)
{
  auto it = find_if(v, val);
  if (it != v.end())
  {
    v.erase(it);
  }
}

template <typename Vector, typename Value>
void remove_erase(Vector& v, const Value& val)
{
  v.erase(std::remove(v.begin(), v.end(), val), v.end());
}

template <typename Vector, typename Function>
void remove_erase_if(Vector& v, const Function& val)
{
  v.erase(std::remove_if(v.begin(), v.end(), val), v.end());
}

template <typename Vector, typename Fun>
void erase_if(Vector& r, Fun f)
{
  for (auto it = std::begin(r); it != std::end(r);)
  {
    it = f(*it) ? r.erase(it) : ++it;
  }
}

template <typename Vector, typename Fun>
bool any_of(Vector&& v, Fun fun) noexcept
{
  return std::any_of(std::begin(v), std::end(v), fun);
}

template <typename Vector, typename Fun>
auto all_of(Vector&& v, Fun fun) noexcept
{
  return std::all_of(std::begin(v), std::end(v), fun);
}

template <typename Vector, typename Fun>
bool none_of(Vector&& v, Fun fun) noexcept
{
  return std::none_of(std::begin(v), std::end(v), fun);
}

template <typename Vector, typename Fun>
auto remove_if(Vector&& v, Fun fun)
{
  return std::remove_if(std::begin(v), std::end(v), fun);
}

template <typename Vector, typename Fun>
auto count_if(Vector&& v, Fun fun)
{
  return std::count_if(std::begin(v), std::end(v), fun);
}

template <typename Vector, typename Fun>
auto max_element(Vector&& v, Fun fun)
{
  return std::max_element(std::begin(v), std::end(v), fun);
}

template <typename Vector>
auto sort(Vector&& v)
{
  return std::sort(std::begin(v), std::end(v));
}

template <typename Vector, typename T>
auto fill(Vector&& v, const T& val)
{
  return std::fill(std::begin(v), std::end(v), val);
}

template <typename Vector>
auto unique(Vector&& v)
{
  return std::unique(std::begin(v), std::end(v));
}

template <typename Vector, typename Fun>
auto sort(Vector&& v, Fun fun)
{
  return std::sort(std::begin(v), std::end(v), fun);
}

template <typename Vector, typename OutputIterator, typename Fun>
auto transform(Vector&& v, OutputIterator it, Fun f)
{
  return std::transform(v.begin(), v.end(), it, f);
}

template <typename Vector1, typename Vector2>
void copy(const Vector1& source, Vector2& destination)
{
  destination.reserve(destination.size() + source.size());
  std::copy(source.begin(), source.end(), std::back_inserter(destination));
}

template <typename Vector1, typename Vector2, typename Pred>
void copy_if(const Vector1& source, Vector2& destination, Pred predicate)
{
  std::copy_if(
      source.begin(), source.end(), std::back_inserter(destination),
      predicate);
}

template <typename T, typename K>
auto last_before(T&& container, const K& k)
{
  auto it = container.upper_bound(k);
  if (it != container.begin()) {
    std::advance(it, -1);
  }
  return it;
}

// http://stackoverflow.com/a/26902803/1495627
template <class F, class... Ts, std::size_t... Is>
void for_each_in_tuple(
    const std::tuple<Ts...>& tuple, F&& func, std::index_sequence<Is...>)
{
  (std::forward<F>(func)(std::get<Is>(tuple)), ...);
}

template <class F, class... Ts>
void for_each_in_tuple(const std::tuple<Ts...>& tuple, F&& func)
{
  for_each_in_tuple(
      tuple, std::forward<F>(func), std::make_index_sequence<sizeof...(Ts)>());
}

template <class F>
void for_each_in_tuple(const std::tuple<>& tuple, const F& func)
{
}


template <class F,
          template<class...> class T1, class... T1s, std::size_t... I1s,
          template<class...> class T2, class... T2s, std::size_t... I2s>
void for_each_in_tuples(
    T1<T1s...>&& t1,
    T2<T2s...>&& t2,
    F&& func,
    std::index_sequence<I1s...>,
    std::index_sequence<I2s...>
    )
{
  (std::forward<F>(func)
     (
        std::get<I1s>(std::forward<T1<T1s...>>(t1)),
        std::get<I2s>(std::forward<T2<T2s...>>(t2))
     ),
   ...);
}

template <class F,
          template<class...> class T1, class... T1s,
          template<class...> class T2, class... T2s>
void for_each_in_tuples(T1<T1s...>&& t1, T2<T2s...>&& t2, F&& func)
{
  for_each_in_tuples(
        std::forward<T1<T1s...>>(t1),
        std::forward<T2<T2s...>>(t2),
        std::forward<F>(func),
        std::make_index_sequence<sizeof...(T1s)>(),
        std::make_index_sequence<sizeof...(T2s)>()
        );
}

template <class F>
void for_each_in_tuples(const std::tuple<>& , const std::tuple<>& , const F& )
{
}

template <std::size_t N>
struct num
{
  static const constexpr auto value = N;
};

template <class F, std::size_t... Is>
void for_each_in_range(F&& func, std::index_sequence<Is...>)
{
  (std::forward<F>(func)(num<Is>{}), ...);
}

template <std::size_t N, typename F>
void for_each_in_range(F&& func)
{
  for_each_in_range(std::forward<F>(func), std::make_index_sequence<N>());
}

namespace detail
{
template <class T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N>
to_array_impl(T (&a)[N], std::index_sequence<I...>) noexcept
{
  return {{a[I]...}};
}
}

template <class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&a)[N]) noexcept
{
  return detail::to_array_impl(a, std::make_index_sequence<N>{});
}

template <typename... Args>
constexpr std::array<const char*, sizeof...(Args)>
make_array(Args&&... args) noexcept
{
  return {args...};
}

template<typename T>
void remove_duplicates(T& vec) {
  if(vec.size() <= 1)
    return;

  std::sort(vec.begin(), vec.end());
  vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}
}
