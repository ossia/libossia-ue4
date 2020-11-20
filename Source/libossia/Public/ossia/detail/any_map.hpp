#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/any.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/detail/string_map.hpp>

/**
 * \file any_map.hpp
 */
#if !defined(_MSC_VER)
extern template class OSSIA_EXTERN_EXPORT_HPP(OSSIA_EXPORT) tsl::hopscotch_map<
    std::string, ossia::any, ossia::string_hash, ossia::string_equal,
    std::allocator<std::pair<std::string, ossia::any>>, 4>;
#endif
namespace ossia
{
//! A container to store any kind of data indexed by a string
using any_map = string_map<ossia::any>;

using extended_attributes = any_map;
/**
 * @brief get_attribute Get an attribute of an any_map.
 *
 * Example :
 * \code
 * auto desc = get_attribute<description>(e, "description");
 * \endcode
 *
 * The value will be default-constructed if it is not found (e.g. empty string,
 * 0...)
 */
template <typename T>
auto get_attribute(const any_map& e, ossia::string_view name)
{
  auto it = e.find(name);
  if (it != e.cend())
  {
    auto val = ossia::any_cast<T>(&it.value());
    if (val)
      return *val;
  }

  return T{};
}

/**
 * @brief get_optional_attribute Maybe get an attribute of an any_map.
 *
 * Example :
 * \code
 * auto desc = get_optional_attribute<description>(e, "description");
 * if(desc)
 *   std::cerr << "The description is: " << *desc << "\n";
 * \endcode
 *
 * The value will be an empty \ref std::optional if it is not found.
 */
template <typename T>
std::optional<T> get_optional_attribute(const any_map& e, ossia::string_view name)
{
  auto it = e.find(name);
  if (it != e.cend())
  {
    auto val = ossia::any_cast<T>(&it.value());
    if (val)
      return *val;
  }

  return std::nullopt;
}

struct is_empty_value
{
  template <typename T>
  bool operator()(const T&) noexcept
  {
    return false;
  }
  bool operator()(const std::string& v) noexcept
  {
    return v.empty();
  }
  bool operator()(const ossia::string_view& v) noexcept
  {
    return v.empty();
  }
  template <typename T>
  bool operator()(const std::vector<T>& v) noexcept
  {
    return v.empty();
  }
};

//! Remove an attribute
inline void unset_attribute(any_map& e, ossia::string_view str)
{
  e.erase(str);
}

//! Sets an attribute in an any_map
template <typename T>
void set_attribute(any_map& e, ossia::string_view str, const T& val)
{
  if (!is_empty_value{}(val))
  {
    // TODO insert_or_assign
    auto it = e.find(str);
    if (it != e.end())
      it.value() = val;
    else
      e.insert(std::make_pair(std::string(str), ossia::any{val}));
  }
  else
  {
    unset_attribute(e, str);
  }
}

//! Checks if an attribute is present.
inline bool has_attribute(const any_map& e, ossia::string_view str) noexcept
{
  return e.find(str) != e.end();
}

//! Sets a bool-like attribute. It should be checked for with has_attribute.
inline void set_attribute(any_map& e, ossia::string_view str)
{
  auto it = e.find(str);
  if (it == e.end())
    e.insert(std::make_pair(std::string(str), ossia::any{}));
}

//! Sets an attribute in an any_map
template <typename T>
void set_attribute(any_map& e, ossia::string_view str, T&& val)
{
  if (!is_empty_value{}(val))
  {
    // TODO insert_or_assign
    auto it = e.find(str);
    if (it != e.end())
      it.value() = std::move(val);
    else
      e.insert(std::make_pair(std::string(str), ossia::any{std::move(val)}));
  }
  else
  {
    unset_attribute(e, str);
  }
}

//! Removes an attribute in an any_map
inline OSSIA_EXPORT void
set_attribute(any_map& e, ossia::string_view str, std::nullopt_t)
{
  unset_attribute(e, str);
}

//! Sets an attribute if opt has a value, else remove the attribute
template <typename T>
void set_optional_attribute(
    any_map& e, ossia::string_view str, const std::optional<T>& opt)
{
  if (opt && !is_empty_value{}(*opt))
    set_attribute(e, str, *opt);
  else
    set_attribute(e, str, std::nullopt);
}

template <typename T>
void set_optional_attribute(
    any_map& e, ossia::string_view str, std::optional<T>&& opt)
{
  if (opt && !is_empty_value{}(*opt))
    set_attribute(e, str, *std::move(opt));
  else
    set_attribute(e, str, std::nullopt);
}
}
