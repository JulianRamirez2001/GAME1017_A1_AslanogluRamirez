#pragma once
#include "Game.h"
#include "Sprites.h"
class State
{
public:
	virtual void Enter() { };
	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;
	virtual void Exit() { };

	virtual void Pause() {	};
	virtual void Resume() {	};
	
};

class TitleState : public State
{
public:
	virtual void Enter() override;
	virtual void Update(float deltaTime) override;
	virtual void Render() override;
	virtual void Exit() override;

};

class GameState : public State
{

	
	bool m_shouldClearEnemies = false;
	bool m_shouldClearPlayerBullets = false;
	bool m_shouldClearEnemyBullets = false;

	bool m_canShoot = true; // This restricts the player from firing again unless they release the Spacebar.
	float timeShoot = 0;
	SDL_Texture* m_pBGText; // For the bg.
	Sprite m_bgArray[Game::kNumberOfBackgrounds];

	SDL_Texture* m_pSpriteTexture; // For the sprites.
	Player* m_pPlayer;

	// The enemy spawn frame timer properties.
	float m_enemySpawnTime = 0;
	float m_enemySpawnMaxTime = 1.5f;

	SDL_FPoint m_pivot{ 0, 0 };

	Mix_Music* m_pMusic = nullptr;
	std::vector<Mix_Chunk*> m_sounds;

	std::vector<Enemy*> m_enemies;
	std::vector<Bullet*> m_playerBullets;
	std::vector<Bullet*> m_enemyBullets;
public:
	void CheckCollision();
	virtual void Enter() override;
	virtual void Update(float deltaTime) override;
	virtual void Render() override;
	virtual void Exit() override;

	virtual void Pause() override;
	virtual void Resume() override;

};

class PauseState : public State
{
public:
	virtual void Enter() override;
	virtual void Update(float deltaTime) override;
	virtual void Render() override;
	virtual void Exit() override;

};
class LoseState : public State
{
public:
	virtual void Enter() override;
	virtual void Update(float deltaTime) override;
	virtual void Render() override;
	virtual void Exit() override;

};
class WinState : public State
{
public:
	virtual void Enter() override;
	virtual void Update(float deltaTime) override;
	virtual void Render() override;
	virtual void Exit() override;

};