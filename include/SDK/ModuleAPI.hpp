#pragma once

namespace Universe::ModuleAPI
{
    class IServerAPI {
    public:
        virtual String   GetName()                     = 0;
        virtual uint32_t GetPort()                     = 0;
        virtual uint8_t  GetPlayerCount()              = 0;
        virtual uint8_t  GetMaxPlayers()               = 0;
        virtual uint8_t  GetGame()                     = 0;
        virtual bool     IsPassworded()                = 0;
        virtual String   GetGamemode()                 = 0;
        virtual String   GetVersion()                  = 0;
        virtual String   GetClientName(uint32_t index) = 0;
    };

    class IModuleAPI {
    public:
        virtual const std::unordered_map<std::string, std::string>& GetConfig() const = 0;

        virtual IServerAPI* GetServerAPI() = 0;
    };
} // namespace Universe::ModuleAPI
