#include "pch.h"
#include "Enemy.h"
#include "Texture.h"
#include "SoundManager.h"

Texture* Enemy::m_pTexture{ nullptr };
int Enemy::m_InstanceCntr{ 0 };

Enemy::Enemy(const std::string& texturePath, const Point2f& startPos, const Type& enemyType, float xSpeed)
	: m_Shape{}
	, m_XSpeed{xSpeed}
	, m_Velocity{}
	, m_Acceleration{0,-981}
	, m_IsDead{ true }
	, m_IsLookingLeft{ true }
	, m_IsDying{ true }
	, m_SpawnedItem{ true }
	, m_GotRevived{ false }
	, m_SrcRect{}
	, m_AccumSec{}
	, m_CurrFrame{}
	, m_NrFramesPerSec{}
	, m_SheetTop{}
	, m_SheetLeft{}
	, m_SheetWidth{}
	, m_SheetHeight{}
	, m_Rows{}
	, m_Cols{}
	, m_EnemyType{ enemyType }
{
	if (m_InstanceCntr == 0)
	{
		m_pTexture = new Texture{ texturePath };
	}

	m_Shape.left = startPos.x;
	m_Shape.bottom = startPos.y;

	++m_InstanceCntr;
}

Enemy::~Enemy()
{
	if (m_InstanceCntr == 1)
	{
		delete m_pTexture;
		m_pTexture = nullptr;
	}
	
	--m_InstanceCntr;
}

void Enemy::Draw() const
{
	if (m_IsDead)
	{
		return;
	}

	glPushMatrix();

	// Draw Enemy
	if (!m_IsLookingLeft)
	{
		glTranslatef(m_Shape.left + m_Shape.width / 2, m_Shape.bottom, 0);
		glScalef(-1, 1, 1);
		glTranslatef(-m_Shape.left - m_Shape.width / 2, -m_Shape.bottom, 0);
	}

	m_pTexture->Draw(m_Shape, m_SrcRect);

	glPopMatrix();
}

bool Enemy::GetDying() const
{
	return m_IsDying;
}

bool Enemy::GetDead() const
{
	return m_IsDead;
}

void Enemy::SetRevived(bool setRevived)
{
	m_GotRevived = true;
}
bool Enemy::GetRevived() const
{
	return m_GotRevived;
}

void Enemy::SetSpawnedItems(bool boolean)
{
	m_SpawnedItem = boolean;
}
bool Enemy::GetSpawnedItems() const
{
	return m_SpawnedItem;
}

Rectf Enemy::GetShape() const
{
	if (m_IsDying)
	{
		return Rectf{ 0,0,0,0 };
	}

	return m_Shape;
}

Point2f Enemy::GetCords() const
{
	return Point2f{ m_Shape.left + m_Shape.width / 2, m_Shape.bottom + m_Shape.height / 2 };
}

float Enemy::GetHeight() const
{
	return m_Shape.height;
}

Enemy::Type Enemy::GetType() const
{
	return m_EnemyType;
}

void Enemy::UpdateAnimation(float elapsedSec)
{
	m_AccumSec += elapsedSec;
	const float frameTime{ 1.f / m_NrFramesPerSec };

	if (m_AccumSec >= frameTime)
	{
		++m_CurrFrame %= (m_Cols * m_Rows);
		m_AccumSec -= frameTime;
	}

	m_SrcRect.left = m_SheetLeft + m_CurrFrame % m_Cols * m_SrcRect.width;
	m_SrcRect.bottom = m_SheetTop + (m_CurrFrame / m_Cols + 1) * m_SrcRect.height;

	// If dying-animation done, kill enemy
	if (m_IsDying && m_CurrFrame == m_Cols - 1)
	{
		m_IsDead = true;
	}
}

void Enemy::SpriteConstructor()
{
	m_Shape.width = m_SheetWidth / m_Cols;
	m_Shape.height = m_SheetHeight / m_Rows;

	m_SrcRect.width = m_SheetWidth / m_Cols;
	m_SrcRect.height = m_SheetHeight / m_Rows;
}

void Enemy::SetDyingReset(SoundManager& soundManager)
{
	m_AccumSec = 0;
	m_CurrFrame = 0;
	m_NrFramesPerSec = 6;

	m_Shape.width = m_SheetWidth / m_Cols;
	m_Shape.height = m_SheetHeight / m_Rows;

	m_SrcRect.width = m_SheetWidth / m_Cols;
	m_SrcRect.height = m_SheetHeight / m_Rows;

	m_IsDying = true;

	soundManager.PlayEffect(SoundManager::Effect::EnemyHit);
}

void Enemy::SetDeadReset()
{
	m_AccumSec = 0;
	m_CurrFrame = 0;
	m_NrFramesPerSec = 6;

	m_Shape.width = m_SheetWidth / m_Cols;
	m_Shape.height = m_SheetHeight / m_Rows;

	m_SrcRect.width = m_SheetWidth / m_Cols;
	m_SrcRect.height = m_SheetHeight / m_Rows;

	m_IsDying = true;
	m_IsDead = true;
	m_SpawnedItem = true;
}

void Enemy::SetAliveReset(const Point2f& spawnPoint)
{
	m_Velocity.y = 0;

	m_Shape.left = spawnPoint.x - m_Shape.width / 2;
	m_Shape.bottom = spawnPoint.y - m_Shape.height;

	m_AccumSec = 0;
	m_CurrFrame = 0;

	m_Shape.width = m_SheetWidth / m_Cols;
	m_Shape.height = m_SheetHeight / m_Rows;

	m_SrcRect.width = m_SheetWidth / m_Cols;
	m_SrcRect.height = m_SheetHeight / m_Rows;

	m_IsDying = false;
	m_IsDead = false;
	m_SpawnedItem = false;
	m_GotRevived = true;
}