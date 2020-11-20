// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/network_logger.hpp>
namespace ossia
{
namespace net
{
protocol_base::~protocol_base() = default;

network_logger::network_logger() = default;
network_logger::~network_logger() = default;

void protocol_base::request(parameter_base&)
{
}

bool protocol_base::push(const parameter_base& p)
{
  return push(p, p.value());
}

bool protocol_base::push_bundle(
    const std::vector<const ossia::net::parameter_base*>& v)
{
  bool b = !v.empty();
  for (auto& addr : v)
  {
    b &= push(*addr);
  }
  return b;
}

bool protocol_base::push_raw_bundle(
    const std::vector<ossia::net::full_parameter_data>& v)
{
  bool b = !v.empty();
  for (auto& addr : v)
  {
    b &= push_raw(addr);
  }
  return b;
}

std::future<void> protocol_base::update_async(node_base& node_base)
{
  // Mock implementation for devices which haven't been ported to async yet
  std::promise<void> promise;
  promise.set_value();
  update(node_base);
  return promise.get_future();
}

void protocol_base::set_device(device_base& dev)
{
}

std::future<void> protocol_base::pull_async(parameter_base&)
{
  return {};
}
}
}
