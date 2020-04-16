#include "PlayerManager.hpp"

PlayerObject* PlayerManager::currentActivePlayer = NULL;

void PlayerManager::handleMessage(Message msg) {
	if (msg.isType("ActivePlayerSwitch")) {
		//std::cout << GetCurrentScene()->getObjectByID((UID_t)msg.data)->getID();
		PlayerManager::updateActivePlayer((UID_t)msg.data[0]);
	}
}

void PlayerManager::updateActivePlayer(UID_t targetID) {
	//std::vector<PlayerObject*>* playerList = &GetCurrentScene()->objectsByType.Players;
	//for (std::size_t i = 0; i < playerList->size(); ++i) {
		//playerList->at(i)->activeState = false;
	//}
	PlayerObject* playerPtr = (PlayerObject*)GetCurrentScene()->getObjectByID(targetID);
	PlayerManager::currentActivePlayer = playerPtr;
	static_cast<Camera*>(GetCurrentScene()->getObjectByID(GetCurrentScene()->getObjectIDByName("MainCamera")))->changeFollowTarget(targetID);
	playerPtr->activeState = true;
	return;
}