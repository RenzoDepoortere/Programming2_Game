#include "pch.h"
#include "Stage.h"

#include "Level.h"
#include "Avatar.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Door.h"
#include "SoundManager.h"
#include "HUDManager.h"
#include "SaveFile.h"
#include "PauseScreen.h"
#include "TitleScreen.h"

#include "SVGParser.h"

Stage::Stage(const Rectf& windowRect)
	: m_IsInitialized{false}
	, m_Window{windowRect}
	, m_IsDying{ false }
	, m_MaxHealth{7}
	, m_PlayerStartPos{120,97}
	, m_MainLevelBoundaries{ 0,0,862,1944 }
	, m_FirstCameraPos{0,0}
	, m_SecsBeforeFinalScreen{}
	, m_Started{true}
	, m_SwitchTheme{true}
	, m_Paused{false}
{
	// Init SaveFile and TitleScreen
	m_pSaveFile = new SaveFile{ "Resources/SaveFiles/SaveContent.txt" };
	m_pTitleScreen = new TitleScreen{ windowRect };

	// Level variables
	const Rectf levelBoundaries{ m_MainLevelBoundaries };
	
	// Player variables
	const Point2f playerPos{ m_PlayerStartPos };
	const int healthState{ m_MaxHealth };
	const int score{ 0 };

	// Camera variables
	const Point2f cameraPos{ m_FirstCameraPos };

	// Init Every Object
	InitLevel(levelBoundaries);
	InitAvatar(playerPos,healthState);	
	InitCamera(cameraPos);
	InitEnemyManager();
	InitDoor();
	InitSoundManager();
	InitHUDManager(healthState);

	// Play Title-Theme And Set Score
	m_pSoundManager->PlayTheme(SoundManager::Theme::Title, true);
	m_pAvatar->SetScore(score, *m_pHUDManager);
}

Stage::~Stage()
{
	delete m_pSaveFile;
	m_pSaveFile = nullptr;

	delete m_pTitleScreen;
	m_pTitleScreen = nullptr;

	delete m_pLevel;
	m_pLevel = nullptr;

	delete m_pAvatar;
	m_pAvatar = nullptr;

	delete m_pCamera;
	m_pCamera = nullptr;

	delete m_pEnemyManager;
	m_pEnemyManager = nullptr;

	delete m_pDoor;
	m_pDoor = nullptr;

	delete m_pSoundManager;
	m_pSoundManager = nullptr;

	delete m_pHUDManager;
	m_pHUDManager = nullptr;
}

void Stage::Draw() const
{
	glPushMatrix();

	m_pCamera->Transform(m_pAvatar->GetCollisionRect());

	m_pLevel->DrawBackground();

	m_pDoor->Draw();

	m_pAvatar->Draw();

	m_pLevel->DrawForeground();

	m_pEnemyManager->Draw();

	glPopMatrix();

	m_pHUDManager->Draw();

	m_pTitleScreen->Draw();
}

void Stage::Update(float elapsedSec)
{
	m_pTitleScreen->Update(*m_pSoundManager);

	// Update Level
	m_pLevel->SetCameraRect(m_pCamera->GetCameraRect());

	// Update Camera
	m_pCamera->Update(elapsedSec, m_pAvatar->GetCollisionRect(), m_Window);

	// Check when out of titleScreen and when to play Level-Theme
	m_Started = m_pTitleScreen->GetInTitleScreen();
	if (!m_Started && m_SwitchTheme)
	{
		m_pSoundManager->PlayTheme(SoundManager::Theme::Level, true);
		m_SwitchTheme = false;
	}

	// Check if Still Paused
	if (!m_Started)
	{
		m_pHUDManager->Update(*m_pSoundManager);
	}
	
	m_Paused = m_pHUDManager->GetPaused();
	m_pSoundManager->PauseTheme(m_Paused);

	// Update Sound
	m_pSoundManager->Update();

	// If not Paused or in TitleScreen, Update
	if (!m_Paused && !m_Started)
	{
		// Update Avatar
		m_pAvatar->Update(elapsedSec, *m_pLevel, *m_pSoundManager);

		// Update EnemyManager
		const Rectf cameraRect{ m_pCamera->GetCameraRect() };
		m_pEnemyManager->Update(elapsedSec, *m_pLevel, cameraRect, *m_pAvatar, *m_pSoundManager, *m_pHUDManager);

		// Set Player variables with Enemies
		UpdatePlayerInter();

		// Update Door
		m_pDoor->Update(*m_pAvatar, *m_pCamera, *m_pLevel, *m_pSoundManager);

		// GameOver
		CheckIfGameOver(elapsedSec);
	}

	// Save Game
	SaveGame();

	// When In TitleScreen, and objects not Init yet
	if (!m_Started && !m_IsInitialized)
	{
		InitObjects();
	}
}

void Stage::DevTeleport()
{
	// Teleports player to Dungeon
	const Rectf boundaries{ 881,13665,860,810 };
	m_pCamera->SetLevelBoundaries(boundaries);

	m_pLevel->SetBoundary(boundaries);

	const Point2f position{ 1513,13822 };
	m_pAvatar->SetPoint(position);
	m_pAvatar->SetLookToLeft(true);
}

// PRIVATE
void Stage::InitLevel(const Rectf& levelBoundaries)
{
	// SVG
	std::vector<std::vector <Point2f>> vertices{};
	SVGParser::GetVerticesFromSvgFile("Resources/SVG/BackgroundSVG.svg", vertices);
	
	// TexturePath
	const std::string backgroundPath{ "Resources/Textures/Background.png" };
	const std::string foregroundPath{ "Resources/Textures/Foreground.png" };

	m_pLevel = new Level{ vertices, backgroundPath,foregroundPath ,levelBoundaries };
}
void Stage::InitAvatar(const Point2f& playerPos, int healthState)
{
	// AvatarShape
	Rectf avatarRect{};
	avatarRect.width = 43;
	avatarRect.height = 65;

	// AvatarPos
	avatarRect.left = playerPos.x;
	avatarRect.bottom = playerPos.y;

	// AvatarSpeed
	const float xSpeed{ 180 }, ySpeed{ 400 };

	// TexturePath
	const std::string avatarPath{ "Resources/Textures/PitChar.png" };
	const std::string hurtPath{ "Resources/Textures/PitHurt.png" };

	m_pAvatar = new Avatar{healthState, avatarRect,xSpeed,ySpeed, avatarPath, hurtPath};
}
void Stage::InitCamera(const Point2f& cameraPos)
{
	const Rectf cameraRect{ cameraPos.x,cameraPos.y,m_Window.width,m_Window.height };
	m_pCamera = new Camera{ cameraRect, m_pLevel->GetBoundary()};
}
void Stage::InitEnemyManager()
{
	// TexturePath
	std::string texturePath{ "Resources/Textures/Enemies.png" };
	
	m_pEnemyManager = new EnemyManager{ texturePath ,m_Window };
}
void Stage::InitDoor()
{
	// TexturePath
	const std::string texturePath{ "Resources/Textures/Background.png" };

	// DoorPos
	std::vector<Point2f> doorsPos;
	doorsPos.push_back(Point2f{ 756,1782 });
	doorsPos.push_back(Point2f{ 1579,13821 });

	// spawnPos --> for player
	std::vector<Point2f> spawnPos;
	spawnPos.push_back(Point2f{ 1513,13822 });
	spawnPos.push_back(Point2f{ 695 ,1783 });

	m_pDoor = new Door{ texturePath,doorsPos, spawnPos, true, m_MainLevelBoundaries };
}
void Stage::InitSoundManager()
{
	m_pSoundManager = new SoundManager{ m_pSaveFile->GetSoundVolume() };
}
void Stage::InitHUDManager(int healthState)
{
	m_pHUDManager = new HUDManager{ healthState,m_MaxHealth, m_Window, m_pSaveFile->GetSoundVolume()};
}

void Stage::UpdatePlayerInter()
{
	EnemyManager::playerInteractions playerInter{ m_pEnemyManager->GetPlayerInteractions() };

	if (playerInter.gotHit)
	{
		m_pAvatar->SetHurt(*m_pSoundManager, *m_pHUDManager);
	}

	if (playerInter.hitEnemy)
	{
		m_pAvatar->DeactivateArrow();
	}

	const int newScore{ playerInter.newScore + m_pAvatar->GetScore() };
	m_pAvatar->SetScore(newScore, *m_pHUDManager);
}

void Stage::CheckIfGameOver(float elapsedSec)
{
	// If player not dead
	if (m_pAvatar->GetActionState() != Avatar::aState::dead)
	{
		return;
	}

	// If Death-Theme not playing
	if (!m_IsDying)
	{
		// Play Theme
		m_pSoundManager->PlayTheme(SoundManager::Theme::Death, false);
		m_IsDying = true;

		return;
	}
	else
	{
		// If still playing
		if (m_pSoundManager->IsMusicPlaying())
		{
			// Check when to stop playing
			m_SecsBeforeFinalScreen += elapsedSec;
			if (m_SecsBeforeFinalScreen > 1.5f)
			{
				m_pHUDManager->SetPlayerDead(true);
			}

			return;
		}

		ResetGame();
	}
}

void Stage::ResetGame()
{
	// Save Volume and Reset Score
	m_pSaveFile->SetSoundVolume(m_pSoundManager->GetVolume());
	m_pSaveFile->SetScore(0);

	// Delete Everything
	delete m_pLevel;
	delete m_pAvatar;
	delete m_pCamera;
	delete m_pEnemyManager;
	delete m_pDoor;
	delete m_pSoundManager;
	delete m_pHUDManager;

	// Reset Private Variables
	m_IsDying = false;
	m_SecsBeforeFinalScreen = 0;

	// Re-Initialize
	InitLevel(m_MainLevelBoundaries);
	InitAvatar(m_PlayerStartPos, m_MaxHealth);
	InitCamera(m_FirstCameraPos);
	InitEnemyManager();
	InitDoor();
	InitSoundManager();
	InitHUDManager(m_MaxHealth);

	// Play Level Theme and Re-Init LevelBoundaries
	m_pSoundManager->PlayTheme(SoundManager::Theme::Level, true);
	m_pCamera->SetLevelBoundaries(m_pLevel->GetBoundary());
}


void Stage::SaveGame()
{
	// Check if Save Needed
	if (!m_pHUDManager->GetShouldSave())
	{
		return;
	}

	// Sound
	m_pSaveFile->SetSoundVolume(m_pSoundManager->GetVolume());

	// Player
	m_pSaveFile->SetHealthState(m_pAvatar->GetHealth());
	m_pSaveFile->SetScore(m_pAvatar->GetScore());
	const Rectf playerRect{ m_pAvatar->GetShape() };
	const Point2f playerPos{ playerRect.left,playerRect.bottom };
	m_pSaveFile->SetPlayerPos(playerPos);

	// Camera
	const Rectf cameraRect{ m_pCamera->GetCameraRect() };
	const Point2f cameraPos{ cameraRect.left,cameraRect.bottom };
	m_pSaveFile->SetCameraPos(cameraPos);

	// Level
	m_pSaveFile->SetLevelBoundaries(m_pLevel->GetBoundary());

	// Save
	m_pSaveFile->SaveVariables();

	// Set no save needed
	m_pHUDManager->SetShouldSave(false);
}

void Stage::InitObjects()
{
	// Level
	Rectf levelBoundaries{ m_pSaveFile->GetLevelBoundaries() };

	// Player
	Point2f playerPos{ m_pSaveFile->GetPlayerPos() };
	int healthState{ m_pSaveFile->GetHealthState() };
	int score{ m_pSaveFile->GetScore() };

	// Camera
	Point2f cameraPos{ m_pSaveFile->GetCameraPos() };

	// Check if "Continue" pressed or not
	if (!m_pTitleScreen->GetLoadFile())
	{
		levelBoundaries = m_MainLevelBoundaries;

		playerPos = m_PlayerStartPos;
		healthState = m_MaxHealth;

		cameraPos = m_FirstCameraPos;

		score = 0;
	}

	// Delete objects
	delete m_pLevel;
	delete m_pAvatar;
	delete m_pCamera;
	delete m_pEnemyManager;
	delete m_pDoor;
	delete m_pSoundManager;
	delete m_pHUDManager;

	// Re-Init
	InitLevel(levelBoundaries);
	InitAvatar(playerPos, healthState);
	InitCamera(cameraPos);
	InitEnemyManager();
	InitDoor();
	InitSoundManager();
	InitHUDManager(healthState);

	// Play Level-Theme & set score
	m_pSoundManager->PlayTheme(SoundManager::Theme::Level, true);
	m_pAvatar->SetScore(score, *m_pHUDManager);

	m_IsInitialized = true;
}