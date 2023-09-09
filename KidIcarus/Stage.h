#pragma once

class Level;
class Avatar;
class Camera;
class EnemyManager;
class Door;
class SoundManager;
class HUDManager;
class SaveFile;
class TitleScreen;

class Stage final
{
public:
	explicit Stage(const Rectf& windowRect);
	~Stage();
	Stage(const Stage& other) = delete;
	Stage& operator= (const Stage& rhs) = delete;
	Stage(Stage&& other) = delete;
	Stage& operator= (Stage&& rhs) = delete;

	void Draw() const;
	void Update(float elapsedSec);

	void DevTeleport();

private:
	// DATA MEMBERS

	// Base Variables
	bool m_IsInitialized;

	const Rectf m_Window;
	bool m_IsDying;

	const int m_MaxHealth;
	const Point2f m_PlayerStartPos;
	const Rectf m_MainLevelBoundaries;
	const Point2f m_FirstCameraPos;

	float m_SecsBeforeFinalScreen;

	bool m_Started;
	bool m_SwitchTheme;

	bool m_Paused;

	// Pointers
	SaveFile* m_pSaveFile;
	TitleScreen* m_pTitleScreen;

	Level* m_pLevel;
	Avatar* m_pAvatar;
	Camera* m_pCamera;
	EnemyManager* m_pEnemyManager;
	Door* m_pDoor;
	SoundManager* m_pSoundManager;
	HUDManager* m_pHUDManager;

	// FUNCTIONS
	void InitLevel(const Rectf& levelBoundaries);
	void InitAvatar(const Point2f& playerPos, int healthState);
	void InitCamera(const Point2f& cameraPos);
	void InitEnemyManager();
	void InitDoor();
	void InitSoundManager();
	void InitHUDManager(int healthState); 

	void UpdatePlayerInter();

	void CheckIfGameOver(float elapsedSec);

	void ResetGame();

	void SaveGame();

	void InitObjects();
};

