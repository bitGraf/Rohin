#include "Event.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"
#include "Engine/Memory/Memory_Arena.h"

struct registered_event {
    void* listener;
    on_event_func callback;
};

#define MAX_MESSAGE_CODES 512
#define MAX_LISTENERS 256

struct event_code_entry {
    uint16 num_listeners;
    registered_event* events;
};

struct event_system_state {
    memory_arena* engine_arena;
    //memory_arena event_queue;

    uint16 num_codes;
    event_code_entry* registered;
};

global_variable bool32 is_initialized;
global_variable event_system_state* global_event_state;

bool32 event_init(memory_arena* arena) {
    if (is_initialized) {
        return false;
    }

    global_event_state = PushStruct(arena, event_system_state);

    global_event_state->engine_arena = arena;
    global_event_state->registered = PushArray(arena, event_code_entry, MAX_MESSAGE_CODES);
    global_event_state->num_codes = MAX_EVENT_CODE;
    for (uint16 n = 0; n < MAX_MESSAGE_CODES; n++) {
        global_event_state->registered[n].events = PushArray(arena, registered_event, MAX_LISTENERS);
        global_event_state->registered[n].num_listeners = 0;
    }

    is_initialized = true;
    return true;
}
void event_shutdown() {
    // don't need to really do anything actually
    for (uint16 i = 0; i < global_event_state->num_codes; i++) {
        global_event_state->registered[i].num_listeners = 0;
        //if (global_event_state.registered[i].events) {
        //    // 
        //}
    }
}

bool32 event_register(uint16 code, void* listener, on_event_func on_event) {
    if (!is_initialized) {
        return false;
    }

    event_code_entry* event_entry = &global_event_state->registered[code];
    // search for a duplicate
    for (uint16 n = 0; n < event_entry->num_listeners; n++) {
        registered_event* event = &event_entry->events[n];
        if (event->listener == listener) {
            RH_WARN("Tried to register the same listener on event code %d twice!", code);
            return false;
        }
    }

    registered_event* new_event = &event_entry->events[event_entry->num_listeners++];
    new_event->listener = listener;
    new_event->callback = on_event;

    return true;
}
bool32 event_unregister(uint16 code, void* listener, on_event_func on_event) {
    if (!is_initialized) {
        return false;
    }

    event_code_entry* event_entry = &global_event_state->registered[code];
    if (event_entry->num_listeners == 0) {
        RH_WARN("No listeners registered on event code %d; cannot unregister.", code);
        return false;
    }

    // search for the listener
    for (uint16 n = 0; n < event_entry->num_listeners; n++) {
        registered_event* event = &event_entry->events[n];
        if (event->listener == listener && event->callback == on_event) {
            // remove this listener from the list
            // do this by shifting everything in the list afterwards left one
            for (uint16 j = n + 1; j < event_entry->num_listeners; j++) {
                event_entry->events[j-1] = event_entry->events[j];
            }
            event_entry->num_listeners--;

            return true;
        }
    }

    RH_WARN("Could not find this listener on event code %d!", code);
    return false;
}

bool32 event_fire(uint16 code, void* sender, event_context context) {
    if (!is_initialized) {
        return false;
    }

    event_code_entry* event_entry = &global_event_state->registered[code];
    if (event_entry->num_listeners == 0) {
        // no listeners on this event
        //RH_TRACE("Firing event code %d to no listeners", code);
        return false;
    }

    for (uint16 n = 0; n < event_entry->num_listeners; n++) {
        registered_event* event = &event_entry->events[n];
        AssertMsg(event->callback, "Event callback is NULL");
        //RH_TRACE("Firing event code %d to listener %d", code, n);
        if (event->callback(code, sender, event->listener, context)) {
            return true; // callback is handled, stop propogating this message
        }
    }

    //RH_WARN("Failed to fire event code %d for some reason!", code);
    return false;
}