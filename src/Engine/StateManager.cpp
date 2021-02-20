#include "StateManager.hpp"

void StateManager::Cleanup() {
	// Cleans up all states
	while (!states.empty()) {
		states.back()->Cleanup();
		states.pop_back();
	}
}

void StateManager::ChangeState(GameState* state) {
	// Cleans up current state
	if (!states.empty()) {
		states.back()->Cleanup();
		states.pop_back();
	}

	// Store and init new state
	states.push_back(state);
	states.back()->Init();
}

void StateManager::PushState(GameState* state) {
	// Pause current state
	if (!states.empty()) {
		states.back()->Pause();
	}

	// Store and init new state
	states.push_back(state);
	states.back()->Init();
}

void StateManager::PopState() {
	// Cleanup current state
	if (!states.empty()) {
		states.back()->Cleanup();
		states.pop_back();
	}

	// Resume previous state
	if (!states.empty()) {
		states.back()->Resume();
	}
}

void StateManager::globalHandle() {
	// let the state handle events
	states.back()->globalHandle();
}

void StateManager::Update() {
	// let the state update the game
	states.back()->Update();
}

void StateManager::Draw() {
	// let the state draw the screen
	states.back()->Draw();
}

DefaultState DefaultState::m_DefaultState;

void DefaultState::Init() {

}