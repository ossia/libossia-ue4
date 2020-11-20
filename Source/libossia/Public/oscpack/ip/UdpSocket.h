#pragma once
#include "AbstractUdpSocket.h"
#include <oscpack/ip/NetworkingUtils.h>

#if defined(_WIN32)
#include "win32/UdpSocket.h"
#else
#include "posix/UdpSocket.h"
#endif

namespace oscpack
{
namespace detail
{
#if defined(_WIN32)
using Implementation = oscpack::win32::Implementation;
#else
using Implementation = oscpack::posix::Implementation;
#endif
}

using UdpTransmitSocket = detail::UdpTransmitSocket<detail::Implementation>;
using UdpReceiveSocket = detail::UdpReceiveSocket<detail::Implementation>;
using UdpListeningReceiveSocket = detail::UdpListeningReceiveSocket<detail::Implementation>;
}
