#include "StaticEntity.hpp"

StaticEntity::StaticEntity() {

}

void StaticEntity::parseLevelData(std::istringstream &iss) {
    std::cout << "Static Entity parse" << std::endl;
}

#ifndef CUSTOM_ENTITIES

void StaticEntity::func() {
    std::cout << "Engine defined\n";
}

#else

void StaticEntity::func() {
    std::cout << "Game defined\n";
}

#endif