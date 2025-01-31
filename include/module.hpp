#pragma once

#include <ServerModuleAPI.hpp>

#include <SDK/ScriptAPI.hpp>

#include <memory>

#pragma once

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __attribute__((visibility("default")))
#endif

namespace SQLModule
{
    using namespace Universe;

    // Module API implementation for SQLModule
    class ModuleHandler : public IServerModuleHandler {
    public:
        void OnModuleLoad(ServerModuleDetails& details, IServerModuleInterface* moduleInterface) override;
        void OnModuleTick() override {};
    };
    static std::unique_ptr<ModuleHandler> gModuleHandler = std::make_unique<ModuleHandler>();
} // namespace SQLModule

// Scripting API entry point (Pending refactoring)
extern "C" DLLEXPORT void RegisterFunctions(Universe::Scripting::API::IVM* vm);

// Module API entry point
extern "C" DLLEXPORT Universe::IServerModuleHandler* GetServerModuleHandler()
{
    return SQLModule::gModuleHandler.get();
}
