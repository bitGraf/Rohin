#include "EMS.hpp"

MessageReceiver::MessageReceiver() {
}

MessageReceiver::~MessageReceiver() {
}

void MessageReceiver::Register(Events eEvent, MessageReceiver* object, Callback function) {
    EMS::GetInstance()->Register(eEvent, object, function);
}

void MessageReceiver::UnRegister(Events eEvent, MessageReceiver* object, Callback function) {
    EMS::GetInstance()->UnRegister(eEvent, object, function);
}

void MessageReceiver::Post(Events eEvent) {
    EMS::GetInstance()->Post(eEvent);
}

void MessageReceiver::Post(Events eEvent, void* data, u32 size) {
    EMS::GetInstance()->Post(eEvent, data, size);
}

void MessageReceiver::Post(Events eEvent, int data) {
    EMS::GetInstance()->Post(eEvent, &data, (u32)sizeof(data));
}

void MessageReceiver::Post(Events eEvent, float data) {
    EMS::GetInstance()->Post(eEvent, &data, (u32)sizeof(data));
}

void MessageReceiver::Post(Events eEvent, char* data) {
    EMS::GetInstance()->Post(eEvent, data, (u32)strlen(data) + 1);
}




EMS* EMS::_singleton = 0;

EMS::EMS() {
    pEventsHead = 0;
    pEventsTail = 0;
}

EMS::EMS(EMS & obj) {
    pEventsHead = 0;
    pEventsTail = 0;
}

EMS::~EMS() {
}

EMS *EMS::GetInstance() {
    if (!_singleton) {
        _singleton = new EMS;
    }
    return _singleton;
}

bool EMS::Init() {
    return true;
}

bool EMS::Destroy() {
    BENCHMARK_FUNCTION();
    while (pEventsHead) {
        pEventsTail = pEventsHead;
        pEventsHead = pEventsHead->pNext;

        if (pEventsTail->pData) {
            delete[] pEventsTail->pData;
            pEventsTail->pData = 0;
        }

        if (pEventsTail) {
            delete pEventsTail;
            pEventsTail = 0;
        }
    }

    if (_singleton) {
        delete _singleton;
        _singleton = 0;
    }
    return true;
}

bool EMS::Exec() {
    if (!pEventsHead)
        return true;

    EventType* pEvent = pEventsHead;
    while (pEvent) {
        pEventsHead = pEvent->pNext;

        if (!pEventsHead)
            pEventsTail = 0;

        int i = 0, imax = registry[pEvent->eEvent].size();
        Registration* reg = registry[pEvent->eEvent].data();

        Console::logMessage("Processing Event: %d", pEvent->eEvent);

        for (;i < registry[pEvent->eEvent].size(); i++, reg++)
            (reg->object->*(reg->function))(pEvent->pData, pEvent->size);

        if (pEvent->pData) {
            delete[] pEvent->pData;
            pEvent->pData = 0;
        }

        if (pEvent) {
            delete pEvent;
            pEvent = 0;
        }

        pEvent = pEventsHead;
    }

    return true;
}

void EMS::Post(Events eEvent, void* data /* = 0 */, u32 size /* = 0 */) {
    EventType* pEvent = new EventType;
    pEvent->eEvent = eEvent;
    pEvent->pNext = 0;
    if (data && size) {
        pEvent->pData = new char[size];
        memset(pEvent->pData, 0, size);
        memcpy(pEvent->pData, data, size);
        pEvent->size = size;
    }
    else {
        pEvent->pData = 0;
    }

    if (!pEventsHead && !pEventsTail)
        pEventsHead = pEventsTail = pEvent;
    else {
        pEventsTail->pNext = pEvent;
        pEventsTail = pEvent;
    }
}

void EMS::Register(Events eEvent, MessageReceiver* object, Callback function) {
    u32 i = 0, imax = registry[eEvent].size();
    for (; i < imax; i++) {
        if (registry[eEvent].data()[i].function == function
            &&
            registry[eEvent].data()[i].object == object) {
            // Callback has already been registered
            return;
        }
    }

    // need to add the callback now
    Registration oCom = { object, function };
    registry[eEvent].push_back(oCom);
}

void EMS::UnRegister(Events eEvent, MessageReceiver* object, Callback function) {
    u32 i = 0, imax = registry[eEvent].size();
    for (; i < imax; i++) {
        if (registry[eEvent].data()[i].function == function
            &&
            registry[eEvent].data()[i].object == object) {
            //remove it from the list
            registry[eEvent].erase(registry[eEvent].begin() + i);
            return;
        }
    }
}