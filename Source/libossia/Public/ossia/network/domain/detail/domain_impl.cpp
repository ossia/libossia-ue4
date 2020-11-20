// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/logger.hpp>
#include <ossia/network/domain/detail/apply_domain.hpp>
#include <ossia/network/domain/domain_conversion.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/network/value/format_value.hpp>

namespace ossia
{
value domain::get_min() const
{
  return ossia::get_min(*this);
}
value domain::get_max() const
{
  return ossia::get_max(*this);
}

void domain::set_min(const ossia::value& val)
{
  return ossia::set_min(*this, val);
}
void domain::set_max(const ossia::value& val)
{
  return ossia::set_max(*this, val);
}

value domain::apply(bounding_mode b, const ossia::value& val) const
{
  return apply_domain(*this, b, val);
}
value domain::apply(bounding_mode b, ossia::value&& val) const
{
  return apply_domain(*this, b, std::move(val));
}

struct domain_prettyprint_visitor
{
  template <typename Domain>
  std::string operator()(const Domain& dom)
  {
    if(dom.min && dom.max)
      return fmt::format("min: {} ; max: {} ; values: {}", *dom.min, *dom.max, dom.values);
    else if(dom.min)
      return fmt::format("min: {} ; values: {}", *dom.min, dom.values);
    else if(dom.max)
      return fmt::format("max: {} ; values: {}", *dom.max, dom.values);
    else
      return fmt::format("values: {}", dom.values);
  }

  std::string operator()(const domain_base<bool>& )
  {
    using namespace std::literals;
    return "bool"s;
  }

  std::string operator()(const domain_base<impulse>& )
  {
    using namespace std::literals;
    return "impulse"s;
  }

  template <std::size_t N>
  std::string operator()(const vecf_domain<N>& dom)
  {
    return fmt::format("array: min: {} ; max: {} ; values : {}", dom.min, dom.max, dom.values);
  }

  std::string operator()(const domain_base<std::string>& dom)
  {
    return fmt::format("stro,g: values : {}", dom.values);
  }

  std::string operator()(const domain_base<ossia::value>& dom)
  {
    using namespace std::literals;
    // TODO
    return "generic"s;
  }

  std::string operator()(const vector_domain& dom)
  {
    return fmt::format("list: min: {} ; max: {} ; values : {}", dom.min, dom.max, dom.values);
  }
};

std::string domain::to_pretty_string() const
{
  if (bool(*this))
  {
    return ossia::apply_nonnull(domain_prettyprint_visitor{}, (const domain_base_variant&)*this);
  }
  else
  {
    return "none";
  }
}

template <typename Domain>
template <typename U>
ossia::value numeric_clamp<Domain>::operator()(bounding_mode b, U&& val) const
{
  using T = typename Domain::value_type;

  if (b == bounding_mode::FREE)
    return std::forward<U>(val);

  if (domain.values.empty())
  {
    const bool has_min = bool(domain.min);
    const bool has_max = bool(domain.max);
    if (has_min && has_max)
    {
      const auto min = *domain.min;
      const auto max = *domain.max;
      switch (b)
      {
        case bounding_mode::CLIP:
          return T(ossia::clamp(std::forward<U>(val), min, max));
        case bounding_mode::WRAP:
          return T(ossia::wrap(std::forward<U>(val), min, max));
        case bounding_mode::FOLD:
          return T(ossia::fold(std::forward<U>(val), min, max));
        case bounding_mode::LOW:
          return T(ossia::clamp_min(std::forward<U>(val), min));
        case bounding_mode::HIGH:
          return T(ossia::clamp_max(std::forward<U>(val), max));
        default:
          break;
      }
    }
    else if (has_min)
    {
      const auto min = *domain.min;
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
          return T(ossia::clamp_min(std::forward<U>(val), min));
        default:
          break;
      }
    }
    else if (has_max)
    {
      const auto max = *domain.max;
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
          return T(ossia::clamp_max(val, max));
        default:
          break;
      }
    }

    return std::forward<U>(val);
  }
  else
  {
    // Return a valid value only if it is in the given values
    auto it = ossia::find(domain.values, val);
    if (it != domain.values.end())
    {
      return T(*it);
    }
    else
    {
      return ossia::value{};
    }

    /* Alternative : return the closest element to value
    auto it = values.lower_bound(val.value);
    if(it != values.end())
    {
        if(it == values.begin())
        {
            // The closest is the first element
            return T(it);
        }
        else
        {
            // Find the closest element between this one and the previous.
            auto prev = it - 1;
            auto prev_diff = std::abs(val.value - *prev);
            auto cur_diff = std::abs(val.value - *it);
            return prev_diff > cur_diff ? *it : *prev;
        }
    }
    else if(it == values.end())
    {
        // Closest element is the latest
        return T(*values.rbegin());
    }
    */
  }
}

template <>
template <typename U>
ossia::value numeric_clamp<domain_base<bool>>::
operator()(bounding_mode b, U&& val) const
{
  switch (b)
  {
    case bounding_mode::CLIP:
      return bool(ossia::clamp(std::forward<U>(val), false, true));
    case bounding_mode::WRAP:
      return bool(ossia::wrap(std::forward<U>(val), false, true));
    case bounding_mode::FOLD:
      return bool(ossia::fold(std::forward<U>(val), false, true));
    case bounding_mode::LOW:
      return bool(ossia::clamp_min(std::forward<U>(val), false));
    case bounding_mode::HIGH:
      return bool(ossia::clamp_max(std::forward<U>(val), true));
    default:
      return std::forward<U>(val);
  }
}

template <typename T>
template <std::size_t N>
ossia::value numeric_clamp<T>::
operator()(bounding_mode b, std::array<float, N> val) const
{
  if (b == bounding_mode::FREE)
    return val;

  // We handle values by checking component by component
  const auto& values = domain.values;
  if (values.empty())
  {
    const bool has_min = bool(domain.min);
    const bool has_max = bool(domain.max);
    if (has_min && has_max)
    {
      const float min = *domain.min;
      const float max = *domain.max;
      switch (b)
      {
        case bounding_mode::CLIP:
          for (std::size_t i = 0; i < N; i++)
            val[i] = ossia::clamp(val[i], min, max);
          break;
        case bounding_mode::WRAP:
          for (std::size_t i = 0; i < N; i++)
            val[i] = ossia::wrap(val[i], min, max);
          break;
        case bounding_mode::FOLD:
          for (std::size_t i = 0; i < N; i++)
            val[i] = ossia::fold(val[i], min, max);
          break;
        case bounding_mode::LOW:
          for (std::size_t i = 0; i < N; i++)
            val[i] = ossia::clamp_min(val[i], min);
          break;
        case bounding_mode::HIGH:
          for (std::size_t i = 0; i < N; i++)
            val[i] = ossia::clamp_max(val[i], max);
          break;
        default:
          break;
      }
    }
    else if (has_min)
    {
      const float min = *domain.min;
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
          for (std::size_t i = 0; i < N; i++)
            val[i] = ossia::clamp_min(val[i], min);
        default:
          break;
      }
    }
    else if (has_max)
    {
      const float max = *domain.max;
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
          for (std::size_t i = 0; i < N; i++)
            val[i] = ossia::clamp_max(val[i], max);
        default:
          break;
      }
    }

    return val;
  }
  else
  {
    for (std::size_t i = 0; i < N; i++)
    {
      // Return a valid value only if it is in the given values
      auto it = ossia::find(values, val[i]);
      if (it == values.end())
      {
        return {};
      }
    }

    return val;
  }
}

value list_clamp::
operator()(bounding_mode b, const std::vector<ossia::value>& val) const
{
  std::vector<ossia::value> res;
  if (b == bounding_mode::FREE)
  {
    res = val;
    return res;
  }


  // We handle values by checking component by component
  const auto& values = domain.values;

  const auto N = val.size();
  res.resize(N);

  const auto& min = domain.min;
  const auto& max = domain.max;
  const auto& vals = domain.values;

  const auto min_N = min.size();
  const auto max_N = max.size();
  const auto vals_N = values.size();
  for (std::size_t i = 0; i < N; i++)
  {
    if (vals_N > i && !vals[i].empty())
    {
      auto it = vals[i].find(val[i]);
      if (it != vals[i].end())
        res[i] = val[i];
    }
    else if (min_N > i && max_N > i)
    {
      const bool valid_min = min[i].valid();
      const bool valid_max = max[i].valid();
      if (valid_min && valid_max)
      {
        switch (b)
        {
          case bounding_mode::CLIP:
            res[i] = ossia::clamp(val[i], min[i], max[i]);
            break;
          case bounding_mode::WRAP:
            res[i] = ossia::wrap(val[i], min[i], max[i]);
            break;
          case bounding_mode::FOLD:
            res[i] = ossia::fold(val[i], min[i], max[i]);
            break;
          case bounding_mode::LOW:
            res[i] = ossia::clamp_min(val[i], min[i]);
            break;
          case bounding_mode::HIGH:
            res[i] = ossia::clamp_max(val[i], max[i]);
            break;
          default:
            res[i] = val[i];
            break;
        }
      }
      else if (valid_min)
      {
        switch (b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::LOW:
            res[i] = ossia::clamp_min(val[i], min[i]);
            break;
          default:
            res[i] = val[i];
            break;
        }
      }
      else if (valid_max)
      {
        switch (b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            res[i] = ossia::clamp_max(val[i], max[i]);
            break;
          default:
            res[i] = val[i];
            break;
        }
      }
      else
      {
        res[i] = val[i];
      }
    }
    else if (min_N > i && min[i].valid())
    {
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
          res[i] = ossia::clamp_min(val[i], min[i]);
          break;
        default:
          res[i] = val[i];
          break;
      }
    }
    else if (max_N > i && max[i].valid())
    {
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
          res[i] = ossia::clamp_max(val[i], max[i]);
          break;
        default:
          res[i] = val[i];
          break;
      }
    }
    else
    {
      res[i] = val[i];
    }
  }
  return res;
}

template <>
template <std::size_t N>
ossia::value numeric_clamp<domain_base<bool>>::
operator()(bounding_mode b, std::array<float, N> val) const
{
  constexpr bool min = false, max = true;
  switch (b)
  {
    case bounding_mode::CLIP:
      for (std::size_t i = 0; i < N; i++)
        val[i] = ossia::clamp(bool(val[i]), min, max);
      break;
    case bounding_mode::WRAP:
      for (std::size_t i = 0; i < N; i++)
        val[i] = ossia::wrap(bool(val[i]), min, max);
      break;
    case bounding_mode::FOLD:
      for (std::size_t i = 0; i < N; i++)
        val[i] = ossia::fold(bool(val[i]), min, max);
      break;
    case bounding_mode::LOW:
      for (std::size_t i = 0; i < N; i++)
        val[i] = ossia::clamp_min(bool(val[i]), min);
      break;
    case bounding_mode::HIGH:
      for (std::size_t i = 0; i < N; i++)
        val[i] = ossia::clamp_max(bool(val[i]), max);
      break;
    default:
      break;
  }
  return val;
}

value list_clamp::
operator()(bounding_mode b, std::vector<ossia::value>&& val) const
{
  std::vector<ossia::value> res;
  if (b == bounding_mode::FREE)
  {
    res = val;
    return res;
  }

  // We handle values by checking component by component

  const auto N = val.size();
  res.resize(N);

  const auto& min = domain.min;
  const auto& max = domain.max;
  const auto& vals = domain.values;

  const auto min_N = min.size();
  const auto max_N = max.size();
  const auto vals_N = vals.size();
  for (std::size_t i = 0; i < N; i++)
  {
    if (vals_N > i && !vals[i].empty())
    {
      auto it = vals[i].find(val[i]);
      if (it != vals[i].end())
        res[i] = std::move(val[i]);
    }
    else if (min_N > i && max_N > i)
    {
      const bool valid_min = min[i].valid();
      const bool valid_max = max[i].valid();
      if (valid_min && valid_max)
      {
        switch (b)
        {
          case bounding_mode::CLIP:
            res[i] = ossia::clamp(std::move(val[i]), min[i], max[i]);
            break;
          case bounding_mode::WRAP:
            res[i] = ossia::wrap(std::move(val[i]), min[i], max[i]);
            break;
          case bounding_mode::FOLD:
            res[i] = ossia::fold(std::move(val[i]), min[i], max[i]);
            break;
          case bounding_mode::LOW:
            res[i] = ossia::clamp_min(std::move(val[i]), min[i]);
            break;
          case bounding_mode::HIGH:
            res[i] = ossia::clamp_max(std::move(val[i]), max[i]);
            break;
          default:
            res[i] = std::move(val[i]);
            break;
        }
      }
      else if (valid_min)
      {
        switch (b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::LOW:
            res[i] = ossia::clamp_min(std::move(val[i]), min[i]);
            break;
          default:
            res[i] = std::move(val[i]);
            break;
        }
      }
      else if (valid_max)
      {
        switch (b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            res[i] = ossia::clamp_max(std::move(val[i]), max[i]);
            break;
          default:
            res[i] = std::move(val[i]);
            break;
        }
      }
      else
      {
        res[i] = std::move(val[i]);
      }
    }
    else if (min_N > i && min[i].valid())
    {
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
          res[i] = ossia::clamp_min(std::move(val[i]), min[i]);
          break;
        default:
          res[i] = std::move(val[i]);
          break;
      }
    }
    else if (max_N > i && max[i].valid())
    {
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
          res[i] = ossia::clamp_max(std::move(val[i]), max[i]);
          break;
        default:
          res[i] = std::move(val[i]);
          break;
      }
    }
    else
    {
      res[i] = std::move(val[i]);
    }
  }
  return res;
}

template <std::size_t N>
value vec_clamp<N>::operator()(bounding_mode b, std::array<float, N> val) const
{
  std::array<float, N> res{};
  if (b == bounding_mode::FREE)
  {
    res = val;
    return res;
  }

  // We handle values by checking component by component
  const auto& min = domain.min;
  const auto& max = domain.max;
  const auto& vals = domain.values;

  for (std::size_t i = 0; i < N; i++)
  {
    if (!vals[i].empty())
    {
      auto it = vals[i].find(val[i]);
      if (it != vals[i].end())
        res[i] = val[i];
    }
    else
    {
      const bool valid_min = bool(min[i]);
      const bool valid_max = bool(max[i]);
      if (valid_min && valid_max)
      {
        switch (b)
        {
          case bounding_mode::CLIP:
            res[i] = ossia::clamp(val[i], *min[i], *max[i]);
            break;
          case bounding_mode::WRAP:
            res[i] = ossia::wrap(val[i], *min[i], *max[i]);
            break;
          case bounding_mode::FOLD:
            res[i] = ossia::fold(val[i], *min[i], *max[i]);
            break;
          case bounding_mode::LOW:
            res[i] = ossia::clamp_min(val[i], *min[i]);
            break;
          case bounding_mode::HIGH:
            res[i] = ossia::clamp_max(val[i], *max[i]);
            break;
          default:
            res[i] = val[i];
            break;
        }
      }
      else if (valid_min)
      {
        switch (b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::LOW:
            res[i] = ossia::clamp_min(val[i], *min[i]);
            break;
          default:
            res[i] = val[i];
            break;
        }
      }
      else if (valid_max)
      {
        switch (b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            res[i] = ossia::clamp_max(val[i], *max[i]);
            break;
          default:
            res[i] = val[i];
            break;
        }
      }
      else
      {
        res[i] = val[i];
      }
    }
  }
  return res;
}

value generic_clamp::operator()(bounding_mode b, const value& v) const
{
  if (b == bounding_mode::FREE)
    return v;

  const auto& values = domain.values;
  if (values.empty())
  {
    const bool has_min = bool(domain.min);
    const bool has_max = bool(domain.max);
    if (has_min && has_max)
    {
      const auto& min = *domain.min;
      const auto& max = *domain.max;
      switch (b)
      {
        case bounding_mode::CLIP:
          return ossia::clamp(v, min, max);
        case bounding_mode::WRAP:
          return ossia::wrap(v, min, max);
        case bounding_mode::FOLD:
          return ossia::fold(v, min, max);
        case bounding_mode::LOW:
          return ossia::clamp_min(v, min);
        case bounding_mode::HIGH:
          return ossia::clamp_max(v, max);
        default:
          break;
      }
    }
    else if (has_min)
    {
      const auto& min = *domain.min;
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
          return ossia::clamp_min(v, min);
        default:
          break;
      }
    }
    else if (has_max)
    {
      const auto& max = *domain.max;
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
          return ossia::clamp_max(v, max);
        default:
          break;
      }
    }

    return v;
  }
  else
  {
    // Return a valid value only if it is in the given values
    auto it = ossia::find(values, v);
    return (it != values.end()) ? v : ossia::value{};
  }
}

value generic_clamp::operator()(bounding_mode b, value&& v) const
{
  if (b == bounding_mode::FREE)
    return std::move(v);

  const auto& values = domain.values;
  if (values.empty())
  {
    const bool has_min = bool(domain.min);
    const bool has_max = bool(domain.max);
    if (has_min && has_max)
    {
      const auto& min = *domain.min;
      const auto& max = *domain.max;
      switch (b)
      {
        case bounding_mode::CLIP:
          return ossia::clamp(std::move(v), min, max);
        case bounding_mode::WRAP:
          return ossia::wrap(std::move(v), min, max);
        case bounding_mode::FOLD:
          return ossia::fold(std::move(v), min, max);
        case bounding_mode::LOW:
          return ossia::clamp_min(std::move(v), min);
        case bounding_mode::HIGH:
          return ossia::clamp_max(std::move(v), max);
        default:
          break;
      }
    }
    else if (has_min)
    {
      const auto& min = *domain.min;
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
          return ossia::clamp_min(std::move(v), min);
        default:
          break;
      }
    }
    else if (has_max)
    {
      const auto& max = *domain.max;
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
          return ossia::clamp_max(std::move(v), max);
        default:
          break;
      }
    }

    return std::move(v);
  }
  else
  {
    // Return a valid value only if it is in the given values
    auto it = ossia::find(values, v);
    return (it != values.end()) ? std::move(v) : ossia::value{};
  }
}

value generic_clamp::
operator()(bounding_mode b, const std::vector<ossia::value>& val) const
{
  if (b == bounding_mode::FREE)
    return val;

  const auto& values = domain.values;
  if (values.empty())
  {
    const bool has_min = bool(domain.min);
    const bool has_max = bool(domain.max);
    if (has_min && has_max)
    {
      std::vector<ossia::value> res;
      res.reserve(val.size());
      const auto& min = *domain.min;
      const auto& max = *domain.max;

      switch (b)
      {
        case bounding_mode::CLIP:
          for (auto& v : val)
            res.push_back(ossia::clamp(v, min, max));
          break;
        case bounding_mode::WRAP:
          for (auto& v : val)
            res.push_back(ossia::wrap(v, min, max));
          break;
        case bounding_mode::FOLD:
          for (auto& v : val)
            res.push_back(ossia::fold(v, min, max));
          break;
        case bounding_mode::LOW:
          for (auto& v : val)
            res.push_back(ossia::clamp_min(v, min));
          break;
        case bounding_mode::HIGH:
          for (auto& v : val)
            res.push_back(ossia::clamp_max(v, max));
          break;
        default:
          return val;
      }

      return res;
    }
    else if (has_min)
    {
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
        {
          const auto& min = *domain.min;
          std::vector<ossia::value> res;
          res.reserve(val.size());
          for (auto& v : val)
          {
            res.push_back(ossia::clamp_min(v, min));
          }
          return res;
        }
        default:
          return val;
      }
    }
    else if (has_max)
    {
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
        {
          const auto& max = *domain.max;
          std::vector<ossia::value> res;
          res.reserve(val.size());
          for (auto& v : val)
          {
            res.push_back(ossia::clamp_max(v, max));
          }
          return res;
        }
        default:
          return val;
      }
    }
    else
    {
      return val;
    }
  }
  else
  {
    for (auto& v : val)
    {
      // Return a valid value only if all the subvalues are in the given values
      auto it = ossia::find(values, v);
      if (it == values.end())
      {
        return {};
      }
    }
    return val;
  }
}

value generic_clamp::
operator()(bounding_mode b, std::vector<ossia::value>&& val) const
{
  if (b == bounding_mode::FREE)
    return std::move(val);

  const auto& values = domain.values;
  if (values.empty())
  {
    const bool has_min = bool(domain.min);
    const bool has_max = bool(domain.max);
    if (has_min && has_max)
    {
      const auto& min = *domain.min;
      const auto& max = *domain.max;
      switch (b)
      {
        case bounding_mode::CLIP:
          for (auto& v : val)
            v = ossia::clamp(v, min, max);
          break;
        case bounding_mode::WRAP:
          for (auto& v : val)
            v = ossia::wrap(v, min, max);
          break;
        case bounding_mode::FOLD:
          for (auto& v : val)
            v = ossia::fold(v, min, max);
          break;
        case bounding_mode::LOW:
          for (auto& v : val)
            v = ossia::clamp_min(v, min);
          break;
        case bounding_mode::HIGH:
          for (auto& v : val)
            v = ossia::clamp_max(v, max);
          break;
        default:
          return std::move(val);
      }

      return std::move(val);
    }
    else if (has_min)
    {
      const auto& min = *domain.min;
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
        {
          for (auto& v : val)
            v = ossia::clamp_min(v, min);
          return std::move(val);
        }
        default:
          return std::move(val);
      }
    }
    else if (has_max)
    {
      const auto& max = *domain.max;
      switch (b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
        {
          for (auto& v : val)
            v = ossia::clamp_max(v, max);
          return std::move(val);
        }
        default:
          return std::move(val);
      }
    }
    else
    {
      return std::move(val);
    }
  }
  else
  {
    for (auto& v : val)
    {
      // Return a valid value only if it is in the given values
      auto it = ossia::find(values, v);
      if (it == values.end())
      {
        // TODO should we remove the "bad" value instead ?
        // Or maybeclamping to the closest value
        // would make most sense.
        return {};
      }
    }
    return std::move(val);
  }
}

value apply_domain_visitor::
operator()(int32_t value, const domain_base<int32_t>& domain) const
{
  return numeric_clamp<domain_base<int32_t>>{domain}(b, value);
}

value apply_domain_visitor::
operator()(float value, const domain_base<float>& domain) const
{
  return numeric_clamp<domain_base<float>>{domain}(b, value);
}

value apply_domain_visitor::
operator()(char value, const domain_base<char>& domain) const
{
  return numeric_clamp<domain_base<char>>{domain}(b, value);
}

value apply_domain_visitor::
operator()(bool value, const domain_base<bool>& domain) const
{
  return numeric_clamp<domain_base<bool>>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::vector<ossia::value>& value,
    const domain_base<ossia::value>& domain) const
{
  std::vector<ossia::value> res = value;
  for (auto& val : res)
  {
    val = generic_clamp{domain}(b, val);
  }
  return res;
}

ossia::value apply_domain_visitor::operator()(
    std::vector<ossia::value>&& value,
    const domain_base<ossia::value>& domain) const
{
  for (auto& val : value)
  {
    val = generic_clamp{domain}(b, std::move(val));
  }
  // TODO currently other values (strings, etc...) are ignored; what should we
  // do here ?
  return std::move(value);
}

// Second case : we filter a whole list.
ossia::value apply_domain_visitor::operator()(
    const std::vector<ossia::value>& value, const vector_domain& domain) const
{
  return list_clamp{domain}(b, value);
}
ossia::value apply_domain_visitor::operator()(
    std::vector<ossia::value>&& value, const vector_domain& domain) const
{
  return list_clamp{domain}(b, std::move(value));
}

// Vec : we can either filter each value, or filter the whole shebang
ossia::value apply_domain_visitor::operator()(
    const std::array<float, 2>& value, const domain_base<float>& domain) const
{
  return numeric_clamp<domain_base<float>>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 2>& value,
    const domain_base<int32_t>& domain) const
{
  return numeric_clamp<domain_base<int32_t>>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 2>& value, const domain_base<bool>& domain) const
{
  return numeric_clamp<domain_base<bool>>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 2>& value, const domain_base<char>& domain) const
{
  return numeric_clamp<domain_base<char>>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 2>& value, const vecf_domain<2>& domain) const
{
  return vec_clamp<2>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 2>& value, const vector_domain& domain) const
{
  return vec_clamp<2>{
      ossia::domain_conversion<vecf_domain<2>>{}.list_func(domain)}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 3>& value, const domain_base<float>& domain) const
{
  return numeric_clamp<domain_base<float>>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 3>& value,
    const domain_base<int32_t>& domain) const
{
  return numeric_clamp<domain_base<int32_t>>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 3>& value, const domain_base<bool>& domain) const
{
  return numeric_clamp<domain_base<bool>>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 3>& value, const domain_base<char>& domain) const
{
  return numeric_clamp<domain_base<char>>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 3>& value, const vecf_domain<3>& domain) const
{
  return vec_clamp<3>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 3>& value, const vector_domain& domain) const
{
  return vec_clamp<3>{
      ossia::domain_conversion<vecf_domain<3>>{}.list_func(domain)}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 4>& value, const domain_base<float>& domain) const
{
  return numeric_clamp<domain_base<float>>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 4>& value,
    const domain_base<int32_t>& domain) const
{
  return numeric_clamp<domain_base<int32_t>>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 4>& value, const domain_base<bool>& domain) const
{
  return numeric_clamp<domain_base<bool>>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 4>& value, const domain_base<char>& domain) const
{
  return numeric_clamp<domain_base<char>>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 4>& value, const vecf_domain<4>& domain) const
{
  return vec_clamp<4>{domain}(b, value);
}

ossia::value apply_domain_visitor::operator()(
    const std::array<float, 4>& value, const vector_domain& domain) const
{
  return vec_clamp<4>{
      ossia::domain_conversion<vecf_domain<4>>{}.list_func(domain)}(b, value);
}
}
std::ostream& operator<<(std::ostream& s, const ossia::domain& d)
{
  // OPTIMIZEME
  s << d.to_pretty_string();
  return s;
}
