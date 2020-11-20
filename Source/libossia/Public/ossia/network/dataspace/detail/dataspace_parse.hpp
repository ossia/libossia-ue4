#pragma once
#include <ossia/detail/for_each.hpp>
#include <ossia/detail/string_map.hpp>
#include <ossia/detail/string_view.hpp>
#include <ossia/network/dataspace/dataspace.hpp>
#include <ossia/network/dataspace/dataspace_parse.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/dataspace/detail/dataspace_list.hpp>

#include <brigand/algorithms/wrap.hpp>
#include <brigand/sequences/list.hpp>
namespace ossia
{

namespace detail
{
using unit_map = string_view_map<ossia::unit_t>;

template <typename Arg, typename... Args>
struct unit_map_factory
{
  void operator()(unit_map& m)
  {
    for (ossia::string_view v : ossia::unit_traits<Arg>::text())
      m.emplace(v, ossia::unit_t{Arg{}});
    unit_map_factory<Args...>{}(m);
  }
};

template <typename Arg>
struct unit_map_factory<Arg>
{
  void operator()(unit_map& m)
  {
    for (ossia::string_view v : ossia::unit_traits<Arg>::text())
      m.emplace(v, ossia::unit_t{Arg{}});
  }
};

template <typename... Args>
struct make_unit_map
{
  unit_map operator()()
  {
    unit_map map;
    unit_map_factory<Args...>{}(map);
    return map;
  }
};

struct unit_factory_visitor
{
  ossia::string_view text;

  template <typename Dataspace_T>
  ossia::unit_t operator()(Dataspace_T arg)
  {
    static const auto units = brigand::wrap<
        typename matching_unit_u_list<Dataspace_T>::type, make_unit_map>{}();
    auto it = units.find(text);
    return it != units.end() ? it->second : ossia::unit_t{};
  }

  OSSIA_INLINE ossia::unit_t operator()()
  {
    return {};
  }
};

template <typename Unit>
using enable_if_multidimensional
    = std::enable_if_t<Unit::is_multidimensional::value>;

template <typename Dataspace, typename Unit, typename = void>
struct make_unit_symbols_sub_helper
{
  void operator()(unit_parse_symbols_t& map)
  {
    using unit_type = Unit;

    std::string res;
    res.reserve(20);

    for (auto ds : dataspace_traits<Dataspace>::text())
    {
      // For each unit :
      for (auto un : unit_traits<unit_type>::text())
      {
        res.clear();

        res.append(ds.data(), ds.size()); // color
        res += '.';                       // color.

        res.append(un.data(), un.size()); // color.rgb

        // Add the unit in long form
        map.add(res, {{}, unit_type{}});
      }
    }
  }
};

template <typename Dataspace, typename Unit>
struct make_unit_symbols_sub_helper<
    Dataspace, Unit, enable_if_multidimensional<Unit>>
{
  void operator()(unit_parse_symbols_t& map)
  {
    using unit_type = Unit;

    std::string res;
    res.reserve(20);

    for (auto ds : dataspace_traits<Dataspace>::text())
    {
      // For each unit :
      for (auto un : unit_traits<unit_type>::text())
      {
        res.clear();

        res.append(ds.data(), ds.size()); // color
        res += '.';                       // color.

        res.append(un.data(), un.size()); // color.rgb

        // Add the unit in long form
        map.add(res, {{}, unit_type{}});

        // Add all the accessors
        res += "._"; // color.rgb._

        const auto& params = unit_type::array_parameters();
        const auto n = params.size();
        for (std::size_t i = 0; i < n; i++)
        {
          // replace the last char with the one in the array parameter
          res[res.size() - 1] = params[i]; // color.rgb.r
          map.add(res, {{(uint8_t)i}, unit_type{}});
        }
      }
    }
  }
};

struct make_unit_symbols_helper
{
  unit_parse_symbols_t map;

  make_unit_symbols_helper()
  {
    ossia::for_each_tagged(dataspace_u_list{}, [&](auto t) {
      using dataspace_type = typename decltype(t)::type;
      ossia::for_each_tagged(dataspace_type{}, [&](auto u) {
        using unit_type = typename decltype(u)::type;
        make_unit_symbols_sub_helper<dataspace_type, unit_type>{}(map);
      });
    });
  }
};
}
}
