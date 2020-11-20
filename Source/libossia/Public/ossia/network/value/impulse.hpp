#pragma once
#include <ossia/detail/config.hpp>
namespace ossia
{
class value;

/**
 * @class Impulse impulse.hpp ossia/network/value/value.hpp
 *
 * Any value can be converted to an impulse.
 * An impulse generally just means that we want to send a message to the
 * address,
 * and a value is not needed.
 *
 * For instance :
 * \code
 * /audio/player/stop
 * \endcode
 *
 * \see expression_pulse
 */
struct OSSIA_EXPORT impulse
{
  using value_type = impulse;
  constexpr impulse() noexcept = default;
  constexpr impulse(const impulse&) noexcept = default;
  constexpr impulse(impulse&&) noexcept = default;
  constexpr impulse& operator=(const impulse&) noexcept = default;
  constexpr impulse& operator=(impulse&&) noexcept = default;

  constexpr bool operator==(const ossia::impulse&) const
  {
    return true;
  }

  constexpr bool operator!=(const ossia::impulse&) const
  {
    return false;
  }

  constexpr bool operator>(const ossia::impulse&) const
  {
    return false;
  }

  constexpr bool operator>=(const ossia::impulse&) const
  {
    return true;
  }

  constexpr bool operator<(const ossia::impulse&) const
  {
    return false;
  }

  constexpr bool operator<=(const ossia::impulse&) const
  {
    return true;
  }
};

template <typename T>
constexpr bool operator==(const T&, const ossia::impulse&)
{
  return true;
}
template <typename T>
constexpr bool operator!=(const T&, const ossia::impulse&)
{
  return false;
}
template <typename T>
constexpr bool operator>(const T&, const ossia::impulse&)
{
  return false;
}
template <typename T>
constexpr bool operator>=(const T&, const ossia::impulse&)
{
  return true;
}
template <typename T>
constexpr bool operator<(const T&, const ossia::impulse&)
{
  return false;
}
template <typename T>
constexpr bool operator<=(const T&, const ossia::impulse&)
{
  return true;
}
template <typename T>
constexpr bool operator==(const ossia::impulse&, const T&)
{
  return true;
}
template <typename T>
constexpr bool operator!=(const ossia::impulse&, const T&)
{
  return false;
}
template <typename T>
constexpr bool operator>(const ossia::impulse&, const T&)
{
  return false;
}
template <typename T>
constexpr bool operator>=(const ossia::impulse&, const T&)
{
  return true;
}
template <typename T>
constexpr bool operator<(const ossia::impulse&, const T&)
{
  return false;
}
template <typename T>
constexpr bool operator<=(const ossia::impulse&, const T&)
{
  return true;
}
}
