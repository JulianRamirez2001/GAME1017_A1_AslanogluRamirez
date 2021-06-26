#include "Game.h"
#include "Utilities.h"
#include "StateManager.h"
#include <iostream>

#include "States.h"

Game::Game() 
	: m_running(false)
	, m_pWindow(nullptr)
	, m_pRenderer(nullptr)
	, m_keyStates(nullptr)
	
	
{

}
Game& Game::GetInstance()
{
	static Game* instance = new Game();
	return *instance;
}


int Game::Init(const char* title, int xPos, int yPos)
{
	std::cout << "Init Game ..." << std::endl;
	srand((unsigned)time(nullptr));

	int errorCode = SDL_Init(SDL_INIT_EVERYTHING);
	if (errorCode == 0)
	{
		std::cout << "SDL_Init() succeeded!" << std::endl;
	}
	else
	{
		std::cout << "SDL_Init() failed. Error code: " << errorCode << ": " << SDL_GetError() << std::endl;
		system("pause");
		return errorCode;
	}

	m_pWindow = SDL_CreateWindow(title, xPos, yPos, kWidth, kHeight, 0);
	if (m_pWindow != nullptr)
	{
		std::cout << "SDL_CreateWindow() succeeded!" << std::endl;
	}
	else
	{
		std::cout << "SDL_CreateWindow() failed. Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		system("pause");
		return -1;
	}

	m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);
	if (m_pRenderer != nullptr)
	{
		std::cout << "SDL_CreateRenderer() succeeded!" << std::endl;
	}
	else
	{
		std::cout << "SDL_CreateRenderer() failed. Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(m_pWindow);
		SDL_Quit();
		system("pause");
		return -1;
	}

	

	std::cout << "Initialization successful!" << std::endl;

	

	StateManager::ChangeState(new TitleState);
	m_keyStates = SDL_GetKeyboardState(nullptr);
	m_running = true;
	return 0;
}

bool Game::IsRunning()
{
	return m_running;
}

void Game::HandleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT: // User pressed window's 'x' button.
			QuitGame();
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_SPACE)
				m_canShoot = true;
			break;
		}
	}
}

bool Game::KeyDown(SDL_Scancode key)
{
	if(m_keyStates)
	{
		return m_keyStates[key] == 1;
	}
	return false;
}



/* Update is SUPER way too long on purpose! Part of the Assignment 1, if you use
   this program as a start project is to chop up Update and figure out where each
   part of the code is supposed to go. A practice in OOP is to have objects handle
   their own behaviour and this is a big hint for you. */
void Game::Update(float deltaTime)
{
	StateManager::Update(deltaTime);
}

void Game::Render()
{
	StateManager::Render();
	SDL_RenderPresent(m_pRenderer);
}

void Game::Clean()
{
	StateManager::Quit();
}

