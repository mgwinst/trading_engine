#pragma once

#include <string>
#include <system_error>

enum class XdpError
{
   SocketCreationFailed,
   UmemRegistrationFailed,
   RingMappingFailed,
   BindFailed,
   InvalidInterface
};
