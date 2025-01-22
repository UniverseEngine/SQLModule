#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>
#include <functional>
#include <exception>
#include <stdexcept>

#ifndef WIN32
#define fprintf_s fprintf
#define printf_s printf
#endif

namespace Universe
{
    using Boolean = bool;
    using Integer = int32_t;
    using Float   = float;
    using Double  = double;
    using String  = std::string;
    using Path = std::filesystem::path;

    template <typename T>
    using Vector  = std::vector<T>;

    template <typename T>
    using Ref = std::reference_wrapper<T>;

    template <typename T>
    using Function = std::function<T>;

    using Exception    = std::exception;
    using RuntimeError = std::runtime_error;

} // namespace Universe
