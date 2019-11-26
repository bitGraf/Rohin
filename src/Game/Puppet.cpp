#include "Puppet.hpp"

Puppet::Puppet() {
    floor = vec3(0, 1, 0);
}

void Puppet::create() {

}

void Puppet::update(double dt) {
    std::cout << "I am a puppet entity\n";
}

void Puppet::render() {

}

void Puppet::parseLevelData(std::istringstream &iss, ResourceManager* resource) {
    Entity::parseLevelData(iss, resource);
}

void Puppet::move(vec3 wishDir) {

}