#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include "Sprites.h"

class Game
{
public:
	static const int kWidth = 1024;
	static const int kHeight = 768;
	static const int kPlayerSpeed = 400;

	static const int kPlayerSourceXPosition = 0;
	static const int kPlayerSourceYPosition = 0;
	static const int kPlayerWidth = 94;
	static const int kPlayerHeight = 100;

	static const int kPlayerBulletSpeed = 600;
	static const int kPlayerBulletSourceXPosition = 376;
	static const int kPlayerBulletSourceYPosition = 0;
	static const int kPlayerBulletWidth = 10;
	static const int kPlayerBulletHeight = 100;

	static const int kEnemySourceXPosition = 0;
	static const int kEnemySourceYPosition = 100;
	static const int kEnemyWidth = 40;
	static const int kEnemyHeight = 46;

	static const int kBackgroundScrollSpeed = 30;
	static const int kNumberOfBackgrounds = 2;
public:
	static Game& GetInstance();
	int Init(const char* title, int xPos, int yPos);
	bool IsRunning();
	void HandleEvents();
	bool KeyDown(SDL_Scancode key);
	void Update(float deltaTime);
	void Render();
	void Clean();
	void QuitGame() { m_running = false; }
	

	SDL_Window* GetWindow() { return m_pWindow; }
	SDL_Renderer* GetRenderer() { return m_pRenderer; }
private:
	bool m_canShoot;
	Game();
	bool m_running; 
	SDL_Window* m_pWindow; 
	SDL_Renderer* m_pRenderer; 

	const Uint8* m_keyStates; 

	 // These three flags check if we need to clear the respective vectors of nullptrs.
	
};
