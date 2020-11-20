#pragma once

#ifndef Q_MOC_RUN
//#define BOOST_SPIRIT_DEBUG
// see https://svn.boost.org/trac/boost/ticket/11875
#if defined(_GLIBCXX_DEBUG)
#define BOOST_PHOENIX_USING_LIBCPP
#endif
#include <ossia/network/base/name_validation.hpp>
#include <ossia/network/value/value.hpp>

#include <boost/fusion/adapted/std_array.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/spirit/home/x3.hpp>
#endif
BOOST_FUSION_ADAPT_STRUCT(ossia::impulse)
namespace ossia
{
namespace detail
{

namespace parse
{
using namespace boost::fusion;
namespace x3 = boost::spirit::x3;

using x3::char_;
using x3::int_;
using x3::real_parser;
using x3::skip;

struct BoolParse_map : x3::symbols<bool>
{
  BoolParse_map()
  {
    add("bool: true",
        true)("bool: false", false)("bool: 1", true)("bool: 0", false);
  }
};

const x3::rule<class value_, ossia::value> value_ = "value";
const x3::rule<class o_impulse_, ossia::impulse> o_impulse_ = "impulse";
const x3::rule<class o_str_, std::string> o_str_ = "str";
const x3::rule<class o_vec2_, std::array<float, 2>> o_vec2_ = "vec2";
const x3::rule<class o_vec3_, std::array<float, 3>> o_vec3_ = "vec3";
const x3::rule<class o_vec4_, std::array<float, 4>> o_vec4_ = "vec4";
const x3::rule<class o_list_, std::vector<ossia::value>> o_list_ = "list";

struct EscapedChar : x3::symbols<const char>
{
  EscapedChar()
  {
    add("\\\"", '\"');
  }
};
using float_p = real_parser<float, x3::real_policies<float>>;

const auto o_impulse__def
    = x3::lit("impulse")[([](auto&) { return ossia::impulse{}; })];
const auto o_str__def
    = "string: \"" >> (x3::lexeme[*(EscapedChar() | (char_ - '"'))]) >> '"';
const auto o_vec2__def = "vec2f: [" >> float_p() >> "," >> float_p() >> "]";
const auto o_vec3__def
    = "vec3f: [" >> float_p() >> "," >> float_p() >> "," >> float_p() >> "]";
const auto o_vec4__def = "vec4f: [" >> float_p() >> "," >> float_p() >> ","
                         >> float_p() >> "," >> float_p() >> "]";
const auto o_list__def = "list: [" >> *(value_ % x3::lit(",")) >> "]";

BOOST_SPIRIT_DEFINE(o_impulse_)
BOOST_SPIRIT_DEFINE(o_str_)
BOOST_SPIRIT_DEFINE(o_vec2_)
BOOST_SPIRIT_DEFINE(o_vec3_)
BOOST_SPIRIT_DEFINE(o_vec4_)
BOOST_SPIRIT_DEFINE(o_list_)

const auto value__def = ("float: " >> float_p())
                        | ("char: '" >> (char_ - "'") >> "'")
                        | ("int: " >> int_) | BoolParse_map{} | o_impulse_
                        | o_str_ | o_vec2_ | o_vec3_ | o_vec4_ | o_list_;

BOOST_SPIRIT_DEFINE(value_)

const x3::rule<class address_, std::string> address_ = "address";
const auto address__def = x3::lexeme[(
    +("/" >> +x3::char_(std::string{ossia::net::name_characters()})) | "/")];
BOOST_SPIRIT_DEFINE(address_)

const x3::rule<class preset_pair_, std::pair<std::string, ossia::value>>
    preset_pair_ = "preset_pair";
const auto preset_pair__def
    = x3::lexeme
          [+x3::char_(std::string{ossia::net::path_characters()})
           >> x3::lit("\t")]
      >> value_;
BOOST_SPIRIT_DEFINE(preset_pair_)

const x3::rule<
    class preset_, std::vector<std::pair<std::string, ossia::value>>>
    preset_ = "preset";
const auto preset__def = *(preset_pair_ % x3::lexeme[x3::eol]);
BOOST_SPIRIT_DEFINE(preset_)
}
}
}
