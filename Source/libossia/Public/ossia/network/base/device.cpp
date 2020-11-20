// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/protocol.hpp>

namespace ossia
{
namespace net
{
device_base::~device_base() = default;

device_base::device_base(std::unique_ptr<protocol_base> proto)
    : m_protocol{std::move(proto)}
{
}

protocol_base& device_base::get_protocol() const
{
  return *m_protocol;
}
}
}
