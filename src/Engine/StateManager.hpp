#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

#include "Engine.hpp"

// Default GameState constructor
class GameState {
public:
	virtual void Init() = 0;
	virtual void Cleanup() = 0;

	virtual void Pause() = 0;
	virtual void Resume() = 0;

	virtual void globalHandle() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0; //will combine prerender and render

	virtual void End() = 0;
};

class DefaultState : public GameState {
public:
	void Init();
	void Cleanup();

	void Pause();
	void Resume();

	void globalHandle();
	void Update();
	void Draw();

	void End();

	static DefaultState* Instance() {
		return &m_DefaultState;
	}

protected:
	DefaultState();

private:
	static DefaultState m_DefaultState;
};


class StateManager {
public:
	void Cleanup();
	void ChangeState(GameState* state);
	void PushState(GameState* state);
	void PopState();

	void globalHandle();
	void Update();
	void Draw();

private:
	std::vector<GameState*> states;
};

#endif