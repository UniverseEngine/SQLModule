#pragma once

#include <ModuleAPI/ModuleAPI.hpp>

#include <SDK/ScriptAPI.hpp>

#include <memory>

#pragma once

using namespace Universe::ModuleAPI;

namespace AnnounceModule
{
    class ModuleHandler : public IModuleHandler {
    public:
        void OnModuleLoad(ModuleDetails&, IModuleInterface&) override;
        void OnModuleTick() override {};
    };
    static std::unique_ptr<ModuleHandler> m_moduleHandler = std::make_unique<ModuleHandler>();

    class ScriptingHandler /* : public IScriptingHandler */ {
    public:
        void OnRegister(Universe::Scripting::API::IVM*); // OnRegister(Universe::ScriptingAPI::VM) override;

    private: // Your functions here
        // clang-format off
        static void open    (Universe::Scripting::API::ICallbackInfo&); // (Universe::ScriptingAPI::CallbackInfo&);
        static void exec    (Universe::Scripting::API::ICallbackInfo&); // (Universe::ScriptingAPI::CallbackInfo&);
        static void queryOne(Universe::Scripting::API::ICallbackInfo&); // (Universe::ScriptingAPI::CallbackInfo&);
        static void query   (Universe::Scripting::API::ICallbackInfo&); // (Universe::ScriptingAPI::CallbackInfo&);
        static void close   (Universe::Scripting::API::ICallbackInfo&); // (Universe::ScriptingAPI::CallbackInfo&);
        static void escape  (Universe::Scripting::API::ICallbackInfo&); // (Universe::ScriptingAPI::CallbackInfo&);
        // clang-format on
    };

} // namespace AnnounceModule
