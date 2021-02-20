#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include "Engine/Scene/Scene.hpp"
#include "Engine/GameObject/Player.hpp"
#include "Engine/GameObject/Camera.hpp"

class PlayerManager {
public:
	static void updateActivePlayer(UID_t targetID);

private:
	static PlayerObject* currentActivePlayer;
};

#endif 