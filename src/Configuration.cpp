#include "Configuration.hpp"

ConfigurationManager::ConfigurationManager() {
    int k = 0;
}

ConfigurationManager::~ConfigurationManager() {

}

void ConfigurationManager::create(int count, ...) {
    if (count > 0) {
        va_list args;
        va_start(args, count);

        for (int i = 0; i < count; i++) {
            CoreSystem* sys = va_arg(args, CoreSystem*);
            sys->create(this);
            m_coreSystems.push_back(sys);
        }
        va_end(args);
    }
    else {
        // No systems to register
    }
}

void ConfigurationManager::destroy() {

}

const char* ConfigurationManager::messageTypeToString(Message::Type type) {
    int k = 0;

    using mt = Message::Type;
    switch (type) {
    case mt::empty:             {return eMessageType_empty;             } break;
    case mt::log:               {return eMessageType_log;               } break;
    case mt::coreSystemCreate:  {return eMessageType_coreSystemCreate;  } break;
    case mt::inputEvent:        {return eMessageType_inputEvent;        } break;
    case mt::windowEvent:       {return eMessageType_windowEvent;       } break;
    case mt::gameEvent:         {return eMessageType_gameEvent;         } break;
    case mt::renderEvent:       {return eMessageType_renderEvent;       } break;
    default:                    {return eErrorString;                   } break;
    }
}