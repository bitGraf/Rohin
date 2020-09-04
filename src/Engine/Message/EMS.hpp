#ifndef _EMS_HPP_
#define _EMS_HPP_

#include <iostream>
#include <string>
#include <vector>

#include "Console.hpp"
#include "DataTypes.hpp"


enum Events {
    eveCheckFileMod,
    eveFileUpdate,
    eveStopGame,
    eventTotal // total number of events
};

class MessageReceiver;

typedef void (MessageReceiver::*Callback)(void* data, u32 size);

class MessageReceiver {
public:
    MessageReceiver();
    ~MessageReceiver();

    void Register(Events eEvent, MessageReceiver* object, Callback function);
    void UnRegister(Events eEvent, MessageReceiver* object, Callback function);

    void Post(Events eEvent);
    void Post(Events eEvent, void* data, u32 size);
    void Post(Events eEvent, int data);
    void Post(Events eEvent, float data);
    void Post(Events eEvent, char* data);
};

struct Registration {
    MessageReceiver* object;
    Callback function;
};

struct EventType {
    Events eEvent;
    void* pData;
    u32 size;
    EventType* pNext;
};

struct EventLookupType {
    char* Name;
    Events key;
};

class EMS {
public:
    static EMS* GetInstance();
    bool Init();
    bool Destroy();
    bool Exec();

    void Post(Events eEvent, void* data = 0, u32 size = 0);

    void Register(Events eEvent, MessageReceiver* object, Callback func);
    void UnRegister(Events eEvent, MessageReceiver* object, Callback func);

protected:
    static EMS* _singleton;
    std::vector<Registration> registry[Events::eventTotal];
    EventType* pEventsHead;
    EventType* pEventsTail;

    EMS();
    EMS(EMS& obj);
    ~EMS();

    // Interface friendship
    friend MessageReceiver;
};

#endif