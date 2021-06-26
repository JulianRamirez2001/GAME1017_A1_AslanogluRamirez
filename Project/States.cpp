#include "States.h"
#include "StateManager.h"
#include "Game.h"
#include "Sprites.h"

#include "Utilities.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

void TitleState::Enter()
{
	std::cout << "Enter State" << std::endl;
}

void TitleState::Update(float deltaTime)
{
	if(Game::GetInstance().KeyDown(SDL_SCANCODE_N))
	{
		StateManager::ChangeState(new GameState());
	}
	if (Game::GetInstance().KeyDown(SDL_SCANCODE_ESCAPE))
	{
		Game::GetInstance().QuitGame();
	}
}

void TitleState::Render()
{
	SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 255, 0, 255, 255);
	SDL_RenderClear(Game::GetInstance().GetRenderer());
}

void TitleState::Exit()
{
	std::cout << "Exit State" << std::endl;
}

void GameState::CheckCollision()
{
	
	{
		// Player vs. Enemy.
		SDL_Rect playerCollisionRect = { (int)m_pPlayer->GetDestinationTransform()->x - Game::kPlayerHeight, (int)m_pPlayer->GetDestinationTransform()->y, Game::kPlayerHeight, Game::kPlayerWidth };
		for (size_t i = 0; i < m_enemies.size(); i++)
		{
			SDL_Rect enemyCollisionRect = { (int)m_enemies[i]->GetDestinationTransform()->x, (int)m_enemies[i]->GetDestinationTransform()->y - Game::kEnemyWidth, Game::kEnemyHeight, Game::kEnemyWidth };
			if (SDL_HasIntersection(&playerCollisionRect, &enemyCollisionRect))
			{
				// Game over!
				std::cout << "Player goes boom!" << std::endl;
				
				Mix_PlayChannel(-1, m_sounds[2], 0);
				delete m_enemies[i];
				m_enemies[i] = nullptr;
				m_shouldClearEnemies = true;
				StateManager::PushState(new LoseState());
			}
		}

		if (m_shouldClearEnemies)
		{
			CleanVector<Enemy*>(m_enemies, m_shouldClearEnemies);
		}

		// Player bullets vs. Enemies.
		for (size_t i = 0; i < m_playerBullets.size(); i++)
		{
			SDL_Rect bulletCollisionRect = { (int)m_playerBullets[i]->GetDestinationTransform()->x - Game::kPlayerBulletHeight, (int)m_playerBullets[i]->GetDestinationTransform()->y, Game::kPlayerBulletHeight, Game::kPlayerBulletWidth };
			for (size_t j = 0; j < m_enemies.size(); j++)
			{
				if (m_enemies[j] != nullptr)
				{
					SDL_Rect enemyCollisionRect = { (int)m_enemies[j]->GetDestinationTransform()->x, (int)m_enemies[j]->GetDestinationTransform()->y - Game::kEnemyWidth, Game::kEnemyHeight, Game::kEnemyWidth };
					if (SDL_HasIntersection(&bulletCollisionRect, &enemyCollisionRect))
					{
						Mix_PlayChannel(-1, m_sounds[2], 0);
						delete m_enemies[j];
						m_enemies[j] = nullptr;
						delete m_playerBullets[i];
						m_playerBullets[i] = nullptr;
						m_shouldClearEnemies = true;
						m_shouldClearPlayerBullets = true;
						break;
					}
				}
			}
		}

		if (m_shouldClearEnemies)
		{
			CleanVector<Enemy*>(m_enemies, m_shouldClearEnemies);
		}

		if (m_shouldClearPlayerBullets)
		{
			CleanVector<Bullet*>(m_playerBullets, m_shouldClearPlayerBullets);
		}

		// Enemy bullets vs. player.
		for (size_t i = 0; i < m_enemyBullets.size(); i++)
		{
			SDL_Rect enemyBulletCollisionRect = { (int)m_enemyBullets[i]->GetDestinationTransform()->x, (int)m_enemyBullets[i]->GetDestinationTransform()->y, (int)m_enemyBullets[i]->GetDestinationTransform()->w, (int)m_enemyBullets[i]->GetDestinationTransform()->h };
			if (SDL_HasIntersection(&playerCollisionRect, &enemyBulletCollisionRect))
			{
				// Game over!
				std::cout << "Player goes boom!" << std::endl;
				Mix_PlayChannel(-1, m_sounds[2], 0);
				delete m_enemyBullets[i];
				m_enemyBullets[i] = nullptr;
				m_shouldClearEnemyBullets = true;
				break;
			}
		}

		if (m_shouldClearEnemyBullets)
		{
			CleanVector<Bullet*>(m_enemyBullets, m_shouldClearEnemyBullets);
		}
	}
	
}

void GameState::Enter()
{
	m_pBGText = IMG_LoadTexture(Game::GetInstance().GetRenderer(), "Assets/Images/background.png");
	m_pSpriteTexture = IMG_LoadTexture(Game::GetInstance().GetRenderer(), "Assets/Images/sprites.png");
	m_pPlayer = new Player({Game::kPlayerSourceXPosition, Game::kPlayerSourceYPosition, Game::kPlayerWidth, Game::kPlayerHeight }, // Image Source
		{Game::kWidth / 4 , Game::kHeight / 2 - Game::kPlayerHeight / 2, Game::kPlayerWidth, Game::kPlayerHeight });


	
	m_pMusic = Mix_LoadMUS("Assets/Audio/game.mp3"); // Load the music track.
	if (m_pMusic == nullptr)
	{
		std::cout << "Failed to load music: " << Mix_GetError() << std::endl;
	}

	m_sounds.reserve(3);
	Mix_Chunk* sound = Mix_LoadWAV("Assets/Audio/enemy.wav");
	if (sound == nullptr)
	{
		std::cout << "Failed to load sound: " << Mix_GetError() << std::endl;
	}
	m_sounds.push_back(sound);

	sound = Mix_LoadWAV("Assets/Audio/laser.wav");
	if (sound == nullptr)
	{
		std::cout << "Failed to load sound: " << Mix_GetError() << std::endl;
	}
	m_sounds.push_back(sound);

	sound = Mix_LoadWAV("Assets/Audio/explode.wav");
	if (sound == nullptr)
	{
		std::cout << "Failed to load sound: " << Mix_GetError() << std::endl;
	}
	m_sounds.push_back(sound);

	// Window Destination

	Mix_PlayMusic(m_pMusic, -1); // Play. -1 = looping.
	Mix_VolumeMusic(16); // 0-MIX_MAX_VOLUME (128).
	
	m_pBGText = IMG_LoadTexture(Game::GetInstance().GetRenderer(), "Assets/Images/background.png");
	m_bgArray[0] = { {0, 0, Game::kWidth, Game::kHeight}, {0, 0, Game::kWidth, Game::kHeight} };
	m_bgArray[1] = { {0, 0, Game::kWidth, Game::kHeight}, {Game::kWidth, 0, Game::kWidth, Game::kHeight} };
	
}

void GameState::Update(float deltaTime)
{
	
	if (Game::GetInstance().KeyDown(SDL_SCANCODE_X))
	{
		StateManager::ChangeState(new TitleState());
	}
	else if (Game::GetInstance().KeyDown(SDL_SCANCODE_P))
	{
		StateManager::PushState(new PauseState());
	}
	else if (Game::GetInstance().KeyDown(SDL_SCANCODE_ESCAPE))
	{
		StateManager::PushState(new TitleState());
	}
	// Scroll the backgrounds. 
	for (int i = 0; i < Game::kNumberOfBackgrounds; i++)
	{
		m_bgArray[i].GetDestinationTransform()->x -= Game::kBackgroundScrollSpeed * deltaTime;
	}

	// Check if they need to snap back.
	if (m_bgArray[1].GetDestinationTransform()->x <= 0)
	{
		m_bgArray[0].GetDestinationTransform()->x = 0;
		m_bgArray[1].GetDestinationTransform()->x = Game::kWidth;
	}

	// Player animation/movement.
	m_pPlayer->Animate(deltaTime); // Oh! We're telling the player to animate itself. This is good! Hint hint.

	if (Game::GetInstance().KeyDown(SDL_SCANCODE_A) && m_pPlayer->GetDestinationTransform()->x > m_pPlayer->GetDestinationTransform()->h)
	{
		m_pPlayer->GetDestinationTransform()->x -= Game::kPlayerSpeed * deltaTime;
	}
	else if (Game::GetInstance().KeyDown(SDL_SCANCODE_D) && m_pPlayer->GetDestinationTransform()->x < Game::kWidth / 2)
	{
		m_pPlayer->GetDestinationTransform()->x += Game::kPlayerSpeed * deltaTime;
	}
	if (Game::GetInstance().KeyDown(SDL_SCANCODE_W) && m_pPlayer->GetDestinationTransform()->y > 0)
	{
		m_pPlayer->GetDestinationTransform()->y -= Game::kPlayerSpeed * deltaTime;
	}
	else if (Game::GetInstance().KeyDown(SDL_SCANCODE_S) && m_pPlayer->GetDestinationTransform()->y < Game::kHeight - m_pPlayer->GetDestinationTransform()->w)
	{
		m_pPlayer->GetDestinationTransform()->y += Game::kPlayerSpeed * deltaTime;
	}

	if (Game::GetInstance().KeyDown(SDL_SCANCODE_SPACE) && m_canShoot)
	{
		m_canShoot = false;
		m_playerBullets.push_back(
			new Bullet({Game::kPlayerBulletSourceXPosition, Game::kPlayerBulletSourceYPosition, Game::kPlayerBulletWidth, Game::kPlayerBulletHeight },
				{ m_pPlayer->GetDestinationTransform()->x + Game::kPlayerWidth - Game::kPlayerBulletWidth, m_pPlayer->GetDestinationTransform()->y + Game::kPlayerHeight / 2 - Game::kPlayerBulletWidth, Game::kPlayerBulletWidth, Game::kPlayerBulletHeight },
			           Game::kPlayerBulletSpeed));
		Mix_PlayChannel(-1, m_sounds[1], 0);
	}
	if(!Game::GetInstance().KeyDown(SDL_SCANCODE_SPACE))
	{
		m_canShoot = true;
	}
	

	// Enemy animation/movement.
	for (size_t i = 0; i < m_enemies.size(); i++)
	{
		m_enemies[i]->Update(deltaTime);
		if (m_enemies[i]->GetDestinationTransform()->x < -m_enemies[i]->GetDestinationTransform()->h)
		{
			delete m_enemies[i];
			m_enemies[i] = nullptr;
			m_shouldClearEnemies = true;
		}
	}
	if (m_shouldClearEnemies)
	{
		CleanVector<Enemy*>(m_enemies, m_shouldClearEnemies);
	}

	// Update enemy spawns.
	m_enemySpawnTime += deltaTime;
	if (m_enemySpawnTime > m_enemySpawnMaxTime)
	{
		// Randomizing enemy bullet spawn rate
		float bulletSpawnRate = 0.5f + (rand() % 3) / 2.0f;
		// Random starting y location
		float yEnemyLocation = (float)(Game::kEnemyHeight + rand() % (Game::kHeight - Game::kEnemyHeight));
		m_enemies.push_back(
			new Enemy({Game::kEnemySourceXPosition, Game::kEnemySourceYPosition, Game::kEnemyWidth, Game::kEnemyHeight },
				{Game::kWidth, yEnemyLocation, Game::kEnemyWidth, Game::kEnemyHeight },
				&m_enemyBullets,
				m_sounds[0],
				bulletSpawnRate));

		m_enemySpawnTime = 0;
	}

	// Update the bullets. Player's first.
	for (size_t i = 0; i < m_playerBullets.size(); i++)
	{
		m_playerBullets[i]->Update(deltaTime);
		if (m_playerBullets[i]->GetDestinationTransform()->x > Game::kWidth)
		{
			delete m_playerBullets[i];
			m_playerBullets[i] = nullptr;
			m_shouldClearPlayerBullets = true;
		}
	}

	if (m_shouldClearPlayerBullets)
	{
		CleanVector<Bullet*>(m_playerBullets, m_shouldClearPlayerBullets);
	}

	// Now enemy bullets.
	for (size_t i = 0; i < m_enemyBullets.size(); i++)
	{
		m_enemyBullets[i]->Update(deltaTime);
		if (m_enemyBullets[i]->GetDestinationTransform()->x < -m_enemyBullets[i]->GetDestinationTransform()->w)
		{
			delete m_enemyBullets[i];
			m_enemyBullets[i] = nullptr;
			m_shouldClearEnemyBullets = true;
		}
	}

	if (m_shouldClearEnemyBullets)
	{
		CleanVector<Bullet*>(m_enemyBullets, m_shouldClearEnemyBullets);
	}
	CheckCollision();
	//Game::GetInstance().CheckCollision();
}

void GameState::Render()
{
	SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(Game::GetInstance().GetRenderer()); // Clear the screen with the draw color.

	// Render stuff. Background first.
	for (int i = 0; i < Game::kNumberOfBackgrounds; i++)
	{
		SDL_RenderCopyF(Game::GetInstance().GetRenderer(), m_pBGText, m_bgArray[i].GetSourceTransform(), m_bgArray[i].GetDestinationTransform());
	}

	// Player.
	SDL_RenderCopyExF(Game::GetInstance().GetRenderer(),
		m_pSpriteTexture,
		m_pPlayer->GetSourceTransform(),
		m_pPlayer->GetDestinationTransform(),
		m_pPlayer->GetAngle(),
		&m_pivot,
		SDL_FLIP_NONE);

	// Player bullets.	
	for (size_t i = 0; i < m_playerBullets.size(); i++)
	{
		SDL_RenderCopyExF(Game::GetInstance().GetRenderer(), m_pSpriteTexture, m_playerBullets[i]->GetSourceTransform(), m_playerBullets[i]->GetDestinationTransform(), 90, &m_pivot, SDL_FLIP_NONE);
	}

	// Enemies.
	for (size_t i = 0; i < m_enemies.size(); i++)
	{
		SDL_RenderCopyExF(Game::GetInstance().GetRenderer(), m_pSpriteTexture, m_enemies[i]->GetSourceTransform(), m_enemies[i]->GetDestinationTransform(), -90, &m_pivot, SDL_FLIP_NONE);
	}

	// Enemy bullets.
	for (size_t i = 0; i < m_enemyBullets.size(); i++)
	{
		SDL_RenderCopyF(Game::GetInstance().GetRenderer(), m_pSpriteTexture, m_enemyBullets[i]->GetSourceTransform(), m_enemyBullets[i]->GetDestinationTransform());
	}
	CheckCollision();
}

void GameState::Exit()
{
	

	// Delete Player
	delete m_pPlayer;
	m_pPlayer = nullptr;

	// Destroy Enemies
	for (size_t i = 0; i < m_enemies.size(); i++)
	{
		delete m_enemies[i];
		m_enemies[i] = nullptr;
	}
	m_enemies.clear();

	// Destroy Player Bullets
	for (size_t i = 0; i < m_playerBullets.size(); i++)
	{
		delete m_playerBullets[i];
		m_playerBullets[i] = nullptr;
	}
	m_playerBullets.clear();

	// Destroy Enemy Bullets
	for (size_t i = 0; i < m_enemyBullets.size(); i++)
	{
		delete m_enemyBullets[i];
		m_enemyBullets[i] = nullptr;
	}
	m_enemyBullets.clear();

	// Clean sounds up
	for (size_t i = 0; i < m_sounds.size(); i++)
	{
		Mix_FreeChunk(m_sounds[i]);
	}
	m_sounds.clear();

	Mix_FreeMusic(m_pMusic);
	Mix_CloseAudio();
	Mix_Quit();
	StateManager::Quit();
	SDL_DestroyRenderer(Game::GetInstance().GetRenderer());
	SDL_DestroyWindow(Game::GetInstance().GetWindow());
	SDL_Quit();
}

void GameState::Pause()
{
	
}
void GameState::Resume()
{

}

void PauseState::Enter()
{
	
}

void PauseState::Update(float deltaTime)
{
	if (Game::GetInstance().KeyDown(SDL_SCANCODE_R))
	{
		StateManager::PopState();
	}
}

void PauseState::Render()
{
	StateManager::GetStates().front()->Render();
	//Pause state
	SDL_SetRenderDrawBlendMode(Game::GetInstance().GetRenderer(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 128, 0, 128, 128);
	
	SDL_Rect bg = { 256, 128, 512, 512 };
	SDL_RenderFillRect(Game::GetInstance().GetRenderer(), &bg);
	
	
}

void PauseState::Exit()
{
}

void LoseState::Enter()
{
	
}

void LoseState::Update(float deltaTime)
{
	StateManager::PopState();
}

void LoseState::Render()
{
	//StateManager::GetStates().front()->Render();
	//Lose state
	SDL_SetRenderDrawBlendMode(Game::GetInstance().GetRenderer(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 256, 0, 0, 256);
	SDL_Rect lose_bg = { 256, 128, 512, 512 };
	SDL_RenderFillRect(Game::GetInstance().GetRenderer(), &lose_bg);
}

void LoseState::Exit()
{
}

void WinState::Enter()
{
}

void WinState::Update(float deltaTime)
{
}

void WinState::Render()
{
}

void WinState::Exit()
{
}

