#pragma once
#include <ossia/detail/config.hpp>

#if defined(_MSC_VER)
#define OSSIA_USE_BOOST_ANY 1
#elif defined(__APPLE__)
// APPLE && (__cplusplus < 201703L) <- "|| "error: call to unavailable function 'any_cast': introduced in macOS 10.14"
#define OSSIA_USE_BOOST_ANY 1

#else

#ifdef __has_include
#if __has_include(<any>) && (__cplusplus > 201402L)
#define OSSIA_USE_STD_ANY 1
#else
#define OSSIA_USE_BOOST_ANY 1
#endif
#else
#define OSSIA_USE_BOOST_ANY 1
#endif
#endif

#if defined(OSSIA_USE_BOOST_ANY)
#include <boost/any.hpp>
namespace ossia
{
using any = boost::any;
using boost::any_cast;
}

#elif defined(OSSIA_USE_STD_ANY)
#include <any>
namespace ossia
{
using any = std::any;
using std::any_cast;
}

#elif defined(OSSIA_USE_STD_EXPERIMENTAL_ANY)
#include <experimental/any>
namespace ossia
{
using any = std::experimental::any;
using std::experimental::any_cast;
}
#endif
