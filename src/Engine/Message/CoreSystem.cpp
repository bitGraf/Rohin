#include "CoreSystem.hpp"

CoreSystem::CoreSystem() {
}

CoreSystem::~CoreSystem() {
}

void CoreSystem::sendMessage(Message msg) {
    MessageBus::sendMessage(msg);
}