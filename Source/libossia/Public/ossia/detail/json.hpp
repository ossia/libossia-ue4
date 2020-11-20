#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/string_view.hpp>
#if defined(_WIN32)
#include <windows.h>
#ifdef GetObject
#undef GetObject
#endif
#endif
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#include <cstring>

inline std::string get_string(const rapidjson::Value& val)
{
  return std::string{val.GetString(), val.GetStringLength()};
}
inline ossia::string_view get_string_view(const rapidjson::Value& val)
{
  return ossia::string_view{val.GetString(), val.GetStringLength()};
}

inline void write_json_key(
    rapidjson::Writer<rapidjson::StringBuffer>& writer, ossia::string_view k)
{
  writer.Key(k.data(), k.size());
}

inline void write_json(
    rapidjson::Writer<rapidjson::StringBuffer>& writer, ossia::string_view k)
{
  writer.String(k.data(), k.size());
}

inline void
write_json(rapidjson::Writer<rapidjson::StringBuffer>& writer, char c)
{
  writer.String(&c, 1);
}

inline std::string json_to_str(const rapidjson::StringBuffer& other)
{
  return std::string(other.GetString(), other.GetSize());
}

using allocator = rapidjson::MemoryPoolAllocator<>;
