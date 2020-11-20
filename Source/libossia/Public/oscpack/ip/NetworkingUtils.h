#pragma once

#if defined(_WIN32)
#include <oscpack/ip/win32/NetworkingUtils.h>
#else
#include <oscpack/ip/posix/NetworkingUtils.h>
#endif
