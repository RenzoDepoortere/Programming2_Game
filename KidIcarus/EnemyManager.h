#pragma once
#include <vector>
#include "Items.h"
#include "SoundManager.h"

class Enemy;
class Shemum;
class Nettler;
class Specknose;
class Level;
class Avatar;
class SoundStream;
class HUDManager;

class EnemyManager final
{
public:
	struct spawnInfo
	{
		int nrEnemies;
		int nrKilled;
		float spawnTime;
		float delay;
		float cooldown;
	};

	struct playerInteractions
	{
		bool gotHit;
		bool hitEnemy;
		int newScore;
	};

	explicit EnemyManager(const std::string& texturePath, const Rectf& windowRect);
	~EnemyManager();
	EnemyManager(const EnemyManager& other) = delete;
	EnemyManager& operator=(const EnemyManager& rhs) = delete;
	EnemyManager(EnemyManager&& other) = delete;
	EnemyManager& operator=(EnemyManager&& rhs) = delete;

	void Draw() const;
	void Update(float elapsedSec, const Level& level, const Rectf& cameraRect,
				Avatar& avatar, SoundManager& soundManager, HUDManager& hudManager);

	playerInteractions GetPlayerInteractions() const;

private:
	// Variables

	// Player
	playerInteractions m_PlayerInteractions;

	// Shemums
	std::vector<Shemum*> m_pShemums;
	spawnInfo m_ShemumInfo;

	// Nettlers
	std::vector<Nettler*> m_pNettlers;
	spawnInfo m_NettlerInfo;

	// Specknoses
	std::vector<Specknose*> m_pSpecknoses;
	const int m_NrSpecknoses;
	int m_SpecknosesKilled;
	float m_SpeckSec;
	float m_StartSec;
	bool m_EnteredOnce;

	// Vectors
	std::vector<Enemy*> m_pEnemies;
	std::vector<Items*> m_pItems;
	

	// Helper functions
	void UpdateEnemies(float elapsedSec, const Level& level, const Rectf& cameraRect,
						Avatar& avatar, SoundManager& soundManager, HUDManager& hudManager);
	
	void UpdateItems(float elapsedSec, Avatar& avatar, SoundManager& soundManager, HUDManager& hudManager);

	bool CheckOnScreen(Enemy* enemy, const Rectf& cameraRect, const Level& level);
	bool CheckIfHit(Enemy* enemy, Avatar& avatar, SoundManager& soundManager, bool dodgeBool = false);
	void CheckPlayerHit(Enemy* enemy, Avatar& avatar, SoundManager& soundManager, HUDManager& hudManager);

	void EnemySpawn(float elapsedSec, const Level& level, const Rectf& cameraRect,
		Avatar& avatar, spawnInfo& spawnInfo, int startIdx, const Rectf& spawnArea);
	void SpawnSpecknoses(float elapsedSec, const Rectf& spawnArea, const Avatar& avatar, int startIdx);

	void SpawnItems(Enemy* enemy, const Items::Size& itemSize);

	
};

