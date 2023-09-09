#include "pch.h"
#include "EnemyManager.h"
#include "Texture.h"
#include "Shemum.h"
#include "Nettler.h"
#include "Specknose.h"
#include "Level.h"
#include "utils.h"
#include "HUDManager.h"

#include <iostream>

EnemyManager::EnemyManager(const std::string& texturePath, const Rectf& windowRect)
	: m_PlayerInteractions{}
	, m_NrSpecknoses{10}
	, m_SpecknosesKilled{}
	, m_SpeckSec{}
	, m_StartSec{1}
	, m_EnteredOnce{false}
	, m_pItems{}
{
	m_ShemumInfo.nrEnemies = 4;
	m_ShemumInfo.nrKilled = m_ShemumInfo.nrEnemies;
	m_ShemumInfo.spawnTime = 2;
	m_ShemumInfo.delay = 0;
	m_ShemumInfo.cooldown = 2;

	m_NettlerInfo.nrEnemies = 1;
	m_NettlerInfo.nrKilled = 1;
	m_NettlerInfo.delay = 0;
	m_NettlerInfo.cooldown = 5;
	m_NettlerInfo.spawnTime = m_NettlerInfo.cooldown;

	m_SpecknosesKilled = m_NrSpecknoses;

	const Point2f startPos{ windowRect.width / 2, windowRect.bottom / 2 };
	float xSpeed{ 100 };

	// Shemums
	for (int idx{}; idx < m_ShemumInfo.nrEnemies; idx++)
	{
		m_pEnemies.push_back(new Shemum{ texturePath,startPos,Enemy::Type::Shemum ,xSpeed });
	}

	// Nettlers
	for (int idx{}; idx < m_NettlerInfo.nrEnemies; idx++)
	{
		m_pEnemies.push_back(new Nettler{ texturePath,startPos,Enemy::Type::Nettler, xSpeed });
	}

	// Specknoses
	xSpeed = 500;
	for (int idx{}; idx < m_NrSpecknoses; idx++)
	{
		const int randNr{ rand() % 2 };
		
		const bool randBool{ randNr == 0 };
		const float randSpeed{ float(pow(-1,randNr)) * xSpeed };

		m_pEnemies.push_back(new Specknose{ texturePath,startPos,Enemy::Type::Specknose,xSpeed, randBool });
	}
}

EnemyManager::~EnemyManager()
{
	for (size_t idx{}; idx < m_pEnemies.size(); idx++)
	{
		delete m_pEnemies[idx];
		m_pEnemies[idx] = nullptr;
	}

	for (size_t i{}; i < m_pItems.size(); i++)
	{
		if (m_pItems[i])
		{
			delete m_pItems[i];
			m_pItems[i] = nullptr;
		}
	}
}

void EnemyManager::Draw() const
{
	// Draw Enemies
	for (size_t idx{}; idx < m_pEnemies.size(); idx++)
	{
		m_pEnemies[idx]->Draw();
	}

	// Draw Items
	for (size_t i{}; i < m_pItems.size(); i++)
	{
		if (m_pItems[i])
		{
			m_pItems[i]->Draw();
		}
	}
}

void EnemyManager::Update(float elapsedSec, const Level& level, const Rectf& cameraRect,
					Avatar& avatar, SoundManager& soundManager, HUDManager& hudManager)
{
	// Reset playerInter
	m_PlayerInteractions.gotHit = false;
	m_PlayerInteractions.hitEnemy = false;
	m_PlayerInteractions.newScore = 0;

	UpdateEnemies(elapsedSec, level, cameraRect, avatar, soundManager, hudManager);

	UpdateItems(elapsedSec, avatar, soundManager, hudManager);
}

EnemyManager::playerInteractions EnemyManager::GetPlayerInteractions() const
{
	return m_PlayerInteractions;
}

// HELPER

void EnemyManager::UpdateEnemies(float elapsedSec, const Level& level, const Rectf& cameraRect,
	Avatar& avatar, SoundManager& soundManager, HUDManager& hudManager)
{
	Rectf spawnArea{ 0,0,864,1728 };
	int startIdx{};

	// Spawn Shemums
	EnemySpawn(elapsedSec, level, cameraRect, avatar, m_ShemumInfo, startIdx,spawnArea);

	// Spawn Nettlers
	spawnArea = Rectf{ 0,865,864,860 };
	startIdx += m_ShemumInfo.nrEnemies;
	EnemySpawn(elapsedSec, level, cameraRect, avatar, m_NettlerInfo, startIdx,spawnArea);
	
	// Spawn Specknoses
	spawnArea = Rectf{ 1001, 13690, 617, 490 };
	startIdx += m_NettlerInfo.nrEnemies;
	SpawnSpecknoses(elapsedSec, spawnArea, avatar,startIdx);

	// Update enemies
	for (size_t idx{}; idx < m_pEnemies.size(); idx++)
	{
		// Nettlers: Check if need to dodge
		bool isDodging{};
		if (m_pEnemies[idx]->GetType() == Enemy::Type::Nettler)
		{
			Nettler* currNett{ dynamic_cast<Nettler*>(m_pEnemies[idx]) };
			currNett->Dodge(elapsedSec, avatar);
			isDodging = currNett->GetDodge();
		}
	
		if (m_pEnemies[idx]->GetType() != Enemy::Type::Specknose)
		{
			// Check if still on-screen
			if (!CheckOnScreen(m_pEnemies[idx], cameraRect, level))
			{
				switch (m_pEnemies[idx]->GetType())
				{
				case Enemy::Type::Shemum:
					++m_ShemumInfo.nrKilled;
					break;

				case Enemy::Type::Nettler:
					++m_NettlerInfo.nrKilled;
					break;
				}
			}
		}		
		
	
		// Check if hit
		if (CheckIfHit(m_pEnemies[idx], avatar, soundManager,isDodging))
		{
			switch (m_pEnemies[idx]->GetType())
			{
			case Enemy::Type::Shemum:
				++m_ShemumInfo.nrKilled;
				break;

			case Enemy::Type::Nettler:
				++m_NettlerInfo.nrKilled;
				break;

			case Enemy::Type::Specknose:
				++m_SpecknosesKilled;
				break;
			}
		}

		// Check if hit Player
		CheckPlayerHit(m_pEnemies[idx], avatar, soundManager, hudManager);

		// Update enemy-class
		m_pEnemies[idx]->Update(elapsedSec, level);
	}
}

void EnemyManager::UpdateItems(float elapsedSec, Avatar& avatar, SoundManager& soundManager, HUDManager& hudManager)
{
	const Rectf avatarShape{ avatar.GetShape() };

	for (size_t idx{}; idx < m_pEnemies.size(); idx++)
	{
		Items::Size itemSize{};

		// Assign itemSize to each enemy
		switch (m_pEnemies[idx]->GetType())
		{
		case Enemy::Type::Shemum:
			itemSize = Items::Size::small;
			break;

		case Enemy::Type::Nettler:
			itemSize = Items::Size::half;
			break;

		case Enemy::Type::Specknose:
			itemSize = Items::Size::big;
			break;
		}

		SpawnItems(m_pEnemies[idx], itemSize);
	}

	// Update Items
	for (int i{ int(m_pItems.size()) - 1 }; i >= 0; i--)
	{
		m_pItems[i]->UpdateTime(elapsedSec);

		// Delete when picked up
		const float timeLimit{ 3 };
		const bool isOverTime{ m_pItems[i]->GetTime() >= timeLimit };
		const bool isPickedUp{ utils::IsOverlapping(m_pItems[i]->GetShape(),avatarShape) };
		if (isPickedUp || isOverTime)
		{
			if (isPickedUp)
			{
				// Assign score to items
				int score{};
				switch (m_pItems[i]->GetSize())
				{
				case Items::Size::small:
					score = 1;

					break;
				case Items::Size::half:
					score = 5;

					break;
				case Items::Size::big:
					score = 10;

					break;
				}


				m_PlayerInteractions.newScore = score;
				soundManager.PlayEffect(SoundManager::Effect::PickUp);
			}

			// Switch Items to delete
			delete m_pItems[i];
			m_pItems[i] = m_pItems.back();
			m_pItems.pop_back();
		}
	}
}

bool EnemyManager::CheckOnScreen(Enemy* enemy, const Rectf& cameraRect, const Level& level)
{
	// Set as dead, when offscreen
	const bool isDying{ enemy->GetDying() };
	
	Rectf customShape{ enemy->GetShape() };
	customShape.bottom += 1;

	const bool isOnScreen{ !level.IsOutsideScreen(customShape)};


	if (!isOnScreen && !isDying)
	{
		enemy->SetDead();
		return false;
	}
	
	return true;
}
bool EnemyManager::CheckIfHit(Enemy* enemy, Avatar& avatar, SoundManager& soundManager, bool dodgeBool)
{
	const bool isHit{ utils::IsOverlapping(enemy->GetShape(), avatar.GetArrow()) };
	const bool isDying{ enemy->GetDying() };
	const bool isDead{ enemy->GetDead() };

	if (isHit && !isDying && !isDead && !dodgeBool)
	{
		m_PlayerInteractions.hitEnemy = true;
		enemy->SetDying(soundManager);

		return true;
	}

	return false;
}
void EnemyManager::CheckPlayerHit(Enemy* enemy, Avatar& avatar, SoundManager& soundManager, HUDManager& hudManager)
{
	const bool hitPlayer{ utils::IsOverlapping(enemy->GetShape(),avatar.GetShape()) };
	const bool isDying{ enemy->GetDying() };
	const bool isDead{ enemy->GetDead() };

	if (hitPlayer && (!isDying || !isDead))
	{
		m_PlayerInteractions.gotHit = true;
	}
}

void EnemyManager::EnemySpawn(float elapsedSec, const Level& level, const Rectf& cameraRect,
	Avatar& avatar, spawnInfo& spawnInfo, int startIdx, const Rectf& spawnArea)
{
	const bool allDead{ spawnInfo.nrKilled >= spawnInfo.nrEnemies };
	const Rectf avatarShape{ avatar.GetShape() };
	const bool isInArea{ utils::IsOverlapping(avatarShape,spawnArea) };

	if (!(allDead && isInArea))
	{
		return;
	}

	// Time before spawning enemies
	spawnInfo.spawnTime += elapsedSec;

	const bool isTime{ spawnInfo.spawnTime >= spawnInfo.cooldown };
	const float height{ m_pEnemies[startIdx]->GetHeight() };
	const bool topFree{ level.TopFree(height) };

	// If place enough to spawn, do so
	if (!(isTime && topFree))
	{
		return;
	}

	// Time between each Enemy
	spawnInfo.delay -= elapsedSec;
	if (spawnInfo.delay > 0)
	{
		return;
	}

	const Point2f spawnPoint{ cameraRect.left + cameraRect.width / 2, cameraRect.bottom + cameraRect.height };
	for (int idx{}; idx < spawnInfo.nrEnemies; idx++)
	{
		// If dead
		if (m_pEnemies[startIdx + idx]->GetDead())
		{
			// Set alive
			m_pEnemies[startIdx + idx]->SetAlive(spawnPoint);

			// Set spawn-time for next Enemy
			const int max{ 100 }, min{ 15 };
			const float divider{ 100 };
			spawnInfo.delay = float(rand() % (max - min + 1) + min) / divider;

			m_pEnemies[startIdx + idx]->SetRevived(true);
			return;
		}
	}

	// Reset variables
	spawnInfo.nrKilled = 0;
	spawnInfo.spawnTime = 0;
	for (int idx{}; idx < spawnInfo.nrEnemies; idx++)
	{
		m_pEnemies[startIdx + idx]->SetRevived(false);
	}
}
void EnemyManager::SpawnSpecknoses(float elapsedSec, const Rectf& spawnArea, const Avatar& avatar, int startIdx)
{
	// If all dead
	const bool allDead{ m_SpecknosesKilled >= m_NrSpecknoses };
	const Rectf avatarShape{ avatar.GetShape() };
	const bool isInArea{ utils::IsOverlapping(avatarShape,spawnArea) };

	if (!(allDead && isInArea))
	{
		return;
	}

	// Countdown before spawning all Specknoses
	m_StartSec -= elapsedSec;
	if (m_StartSec > 0)
	{
		return;
	}

	// Time between each Specknoses
	m_SpeckSec -= elapsedSec;
	if (m_SpeckSec > 0)
	{
		return;
	}

	const float buffer{ 50 };
	Point2f position{};
	for (size_t idx{}; idx < m_NrSpecknoses; idx++)
	{
		Specknose* currSpeck{ dynamic_cast<Specknose*>(m_pEnemies[startIdx + idx]) };

		// If dead and not revived
		if (currSpeck->GetDead() && !currSpeck->GetRevived())
		{
			// Set random position
			position.x = rand() % int(spawnArea.width + 1 - (2 * buffer)) + spawnArea.left + buffer;
			position.y = rand() % int(spawnArea.height + 1 - (2 * buffer)) + spawnArea.bottom + buffer;

			// Spawn Specknose
			currSpeck->SetAlive(position);
			currSpeck->SetMoveArea(spawnArea);
			currSpeck->SetRevived(true);

			// Set random time between each Specknoses
			const int max{ 50 }, min{ 15 };
			const float divider{ 100 };
			m_SpeckSec = float(rand() % (max - min + 1) + min) / divider;

			return;
		}
	}

	// Set all Specknoses as revived
	m_SpecknosesKilled = 0;
	m_SpeckSec = 0;
	m_StartSec = 1;

	for (size_t idx{}; idx < m_NrSpecknoses; idx++)
	{
		m_pEnemies[startIdx + idx]->SetRevived(false);
	}
}

void EnemyManager::SpawnItems(Enemy* enemy, const Items::Size& itemSize)
{
	const bool isDead{ enemy->GetDead() };
	const bool hasSpawned{ enemy->GetSpawnedItems() };

	if (isDead && !hasSpawned)
	{
		// Create Item when Enemy died
		const std::string itemPath{ "Resources/Textures/Items.png" };
		const Point2f enemyPos{ enemy->GetCords() };

		m_pItems.push_back(new Items{ itemPath,itemSize, enemyPos });
		enemy->SetSpawnedItems();
	}
}