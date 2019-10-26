#ifndef CONFIGURATION_H
#define CONFIGURATION_H

class ConfigurationManager;

#include <vector>

#include "Message.hpp"
#include "CoreSystem.hpp"

/*enum class eCoreSystems {
    sys_err,
    sys_Window,
    sys_Console,
    sys_FileSystem,
    sys_ResourceManager
};*/

class ConfigurationManager {//  : public CoreSystem {
public:
    ConfigurationManager();
    ~ConfigurationManager();

    void create(int count, ...);
    void destroy();

    /* Enum decodes */
    const char* messageTypeToString(Message::Type type);

    std::vector<CoreSystem*> m_coreSystems;
private:
    const char* eErrorString = "ERROR: ";

    /* Message Types */
    const char* eMessageType_empty = "Empty Message: ";
    const char* eMessageType_log = "LOG: ";
    const char* eMessageType_coreSystemCreate = "SysCreate: ";
    const char* eMessageType_inputEvent = "INPUT: ";
    const char* eMessageType_windowEvent = "WINDOW: ";
    const char* eMessageType_gameEvent = "GAME: ";
    const char* eMessageType_renderEvent = "RENDER: ";

    /*
    empty = 0,
    log,
    coreSystemCreate,
    inputEvent,
    windowEvent,
    gameEvent
    */
};

#endif
