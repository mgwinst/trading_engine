#pragma once

#include <string>
#include <system_error>

struct ConfigurationError
{
   std::string message;
   std::error_code error;
};