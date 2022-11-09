#pragma once

#include "pch.hpp"

#include <SDK/SDK.hpp>

using namespace Universe;

namespace module
{
    ModuleAPI::IModuleAPI* m_api;

    DLLEXPORT void OnLoad(String* name, String* description, String* author, ModuleAPI::IModuleAPI* api);
    DLLEXPORT void RegisterFunctions(Scripting::API::IVM* vm);
    DLLEXPORT void OnPulse();
}