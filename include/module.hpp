#pragma once

#include "pch.hpp"

#include "SDK/ModuleAPI.hpp"

using namespace Universe;

namespace module
{
    ModuleAPI* m_api;

    DLLEXPORT void OnLoad(String* name, String* description, String* author, ModuleAPI* api);
    DLLEXPORT void RegisterFunctions(Scripting::API::IVM* vm);
    DLLEXPORT void OnPulse();
}