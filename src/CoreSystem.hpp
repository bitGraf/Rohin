#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

class CoreSystem;

#include <stdio.h>
#include <stdlib.h>
#include <cstdarg>

#include "Configuration.hpp"
#include "HashTable.hpp"

#include "MessageBus.hpp"

/**
 * @brief Comon interface for core engine systems.
 * 
 * @class CoreSystem base class.
 * 
 * The standard interface for every engine CoreSystem. CoreSystem objects
 * do not communicate with each other directly, but rather put
 * Messages on a queue using the MessageBus. The MessageBus then
 * passes out the messages to each CoreSystem, and then they handle them 
 * with their own specific code.
 * 
 */
class CoreSystem {
public:
    CoreSystem(); ///< Empty Constructor
    ~CoreSystem(); ///< Empty Destructor

    /**
     * @brief Main startup function
     * 
     * This is where any initialization happens for the CoreSystem,
     * rather than in the constructor. This allows systems to be 
     * start and stopped in the correct order. Takes a ConfigurationManager
     * as an input to set any configuration parameters.
     * 
     * @param configMgr A pointer to the ConfigurationManager object.
     */
    void create(ConfigurationManager* configMgr);

    /**
     * @brief Set the Message Bus object.
     * 
     * Give the CoreSystem a reference to the engine's MessageBus.
     * 
     * @todo MessageBus should probably be a static class that can be accessed globally.
     * 
     * @param msgBus A pointer to the MessageBus object.
     */
    void setMessageBus(MessageBus* msgBus); // remove this, move into configMgr

    /**
     * @brief Push a Message to the MessageBus.
     * 
     * @param msg The Message to push.
     */
    void putMessage(Message msg);

    /**
     * @brief Push a Message to the MessageBus by type and data.
     * 
     * @param _type Type of Message to send.
     * @param data String of data to send.
     */
    void putMessage(Message::Type _type, std::string data);
    /**
     * @brief Send a Log Message with the specified text.
     * 
     * @param text The text
     */
    void logMessage(const char* text);
    /**
     * @brief Send a Log Message with the specified text, along with some ints.
     * 
     * @param text The text
     * @param count How many ints
     * @param ... The ints
     */
    void logMessage(const char* text, int count, ...); // ONLY ALLOWS INTS
    
    /**
     * @brief Update CoreSystem state.
     * 
     * Virtual function that gets called every update cycle.
     * Specific implementations should provide whatever
     * functionality is appropriate here.
     * 
     * @param dt Time passed since last update
     */
    virtual void update(double dt) = 0;
    /**
     * @brief Handle a message.
     * 
     * This is how the MessageBus sends a Message to a CoreSystem.
     * Overwrite this function to provide specific system implementation.
     * 
     * @param msg The Message sent to the CoreSystem.
     */
    virtual void handleMessage(Message msg) = 0;
    /**
     * @brief System specific shut-down.
     * 
     * System specific shut-down function.
     * 
     */
    virtual void destroy() = 0;
    /**
     * @brief System specific startup.
     * 
     * System specific start-up function.
     * Called by {@link create}.
     * 
     * @param configMgr Reference to ConfigurationManager.
     */
    virtual void sys_create(ConfigurationManager* configMgr) = 0;

protected:
    ConfigurationManager*   m_configMgr;
    MessageBus*             m_msgBus;
};

#endif