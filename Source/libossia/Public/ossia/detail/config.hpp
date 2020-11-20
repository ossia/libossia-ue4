#pragma once
#if __has_include(<ossia-config.hpp>)
#include <ossia-config.hpp>
#endif

#if __has_include(<ossia-config.hpp>)
#include <ossia_export.h>
#else
#define OSSIA_EXPORT
#endif

#if defined(__APPLE__)
#include <MacTypes.h>
#if defined(nil)
#undef nil
#endif
#endif

#define OSSIA_EXTERN_EXPORT_HPP(EXPORT) EXPORT
#define OSSIA_EXTERN_EXPORT_CPP(EXPORT)

#if defined(__cplusplus) \
    && ((__cplusplus >= 201103L) || (defined(_MSC_VER) && _MSC_VER >= 1900))
#define OSSIA_CXX11 1
#endif
#if defined(__cplusplus) \
    && ((__cplusplus >= 201403L) || (defined(_MSC_VER) && _MSC_VER >= 1900))
#define OSSIA_CXX14 1
#endif
#if defined(__cplusplus) \
    && ((__cplusplus >= 201703L) || (defined(_MSC_VER) && _MSC_VER >= 1900))
#define OSSIA_CXX17 1
#endif

/// Constexpr support ///
#define constexpr_return(X)                 \
  do                                        \
  {                                         \
    constexpr auto constexpr_return_x_ = X; \
    return constexpr_return_x_;             \
  } while (0)

/// Inline support ///
#if defined(__GNUC__)
#define OSSIA_INLINE inline __attribute__((always_inline))
#elif defined(__clang__)
#define OSSIA_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define OSSIA_INLINE inline __forceinline
#else
#define OSSIA_INLINE inline
#endif

#define _WEBSOCKETPP_CPP11_STRICT_ 1

#define SPDLOG_NO_DATETIME
#define SPDLOG_NO_THREAD_ID
#define SPDLOG_NO_NAME

#define SPDLOG_DEBUG_ON
#define SPDLOG_TRACE_ON

#if !defined(SPDLOG_FMT_EXTERNAL)
#define SPDLOG_FMT_EXTERNAL 1
#endif

#if !defined(FMT_HEADER_ONLY)
#define FMT_HEADER_ONLY 1
#endif

#if !defined(RAPIDJSON_HAS_STDSTRING)
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#if defined(__SANITIZE_ADDRESS__)
#define OSSIA_ASAN 1
#elif defined(__has_feature)
#if __has_feature(address_sanitizer)
#define OSSIA_ASAN 1
#endif
#endif

#if !defined(OSSIA_ASAN)
#if defined(__AVX__)
#define RAPIDJSON_SSE42 1
#elif defined(__SSE2__)
#define RAPIDJSON_SSE2 1
#endif

#if defined(__ARM_NEON)
#define RAPIDJSON_NEON 1
#endif
#endif

// https://github.com/Tencent/rapidjson/issues/1015
#if !defined(RAPIDJSON_HAS_CXX11_RVALUE_REFS)
#define RAPIDJSON_HAS_CXX11_RVALUE_REFS 1
#endif

#define BOOST_MATH_DISABLE_FLOAT128
#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE 1

#define BOOST_ERROR_CODE_HEADER_ONLY 1
#define BOOST_SYSTEM_NO_DEPRECATED 1
#define BOOST_LEXICAL_CAST_ASSUME_C_LOCALE 1

#if !defined(ASIO_STANDALONE)
#define ASIO_STANDALONE 1
#endif

#if !defined(BOOST_REGEX_NO_LIB)
#define BOOST_REGEX_NO_LIB 1
#endif

#if !defined(BOOST_DATE_TIME_NO_LIB)
#define BOOST_DATE_TIME_NO_LIB 1
#endif

#if !defined(BOOST_SYSTEM_NO_LIB)
#define BOOST_SYSTEM_NO_LIB 1
#endif

#if !defined(QT_NO_KEYWORDS)
#define QT_NO_KEYWORDS 1
#endif
