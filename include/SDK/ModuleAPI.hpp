#pragma once

#include <string>

#include "ScriptAPI.hpp"

using namespace std;

class ServerAPI {
public:
    string (*GetName)();
    uint32_t (*GetPort)();
    uint8_t (*GetPlayerCount)();
    uint8_t (*GetMaxPlayers)();
    uint8_t (*GetGame)();
    string (*GetClientName)(uint32_t index);
};

class ModuleAPI {
public:
    ServerAPI m_server;
};