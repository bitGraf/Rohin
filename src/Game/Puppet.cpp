#include "Puppet.hpp"

Puppet::Puppet() {
    floor = vec3(0, 1, 0);
    controller = nullptr;
}

void Puppet::create() {

}

void Puppet::update(double dt) {
    if (controller) {
        // Get any movement commands from this puppet's controller

        move(controller->getCommand());
    }
}

void Puppet::render() {

}

void Puppet::parseLevelData(std::istringstream &iss, ResourceManager* resource) {
    Entity::parseLevelData(iss, resource);
}

void Puppet::move(vec3 wishDir) {

}