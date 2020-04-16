#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include "Scene/Scene.hpp"
#include "GameObject/Player.hpp"
#include "GameObject/Camera.hpp"

class PlayerManager {
public:
	static void handleMessage(Message msg);
	static void updateActivePlayer(UID_t targetID);

private:
	static PlayerObject* currentActivePlayer;
};

#endif 