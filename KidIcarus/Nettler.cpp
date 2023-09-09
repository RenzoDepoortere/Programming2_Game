#include "pch.h"
#include "Nettler.h"
#include "Level.h"

Nettler::Nettler(const std::string& texturePath, const Point2f& startPos, const Type& enemyType, float xSpeed)
	: Enemy{texturePath,startPos,enemyType,xSpeed}
	, m_IsFalling{ false }
	, m_HasTurned{ false }
	, m_DodgeSec{ 0.25f }
	, m_IsDodging{ false }
	, m_IsHalfDodging{ false }
{
	m_NrFramesPerSec = 4;

	m_SheetTop = 275;
	m_SheetLeft = 148;
	m_SheetWidth = 86;
	m_SheetHeight = 42;

	m_Rows = 1;
	m_Cols = 2;

	SpriteConstructor();
}

void Nettler::Update(float elapsedSec, const Level& level)
{
	if (m_IsDead)
	{
		return;
	}

	//Handles gravity & ground-collision
	m_Velocity.y += m_Acceleration.y * elapsedSec;
	level.HandleCollision(m_Shape, m_Velocity, !m_IsFalling, false);
	
	//Handles side-swap
	level.HandleTeleport(m_Shape,m_SrcRect);

	if (!m_IsDying)
	{
		// Handles movement
		UpdateMovement(elapsedSec, level);
	}

	// Handle Animation
	UpdateAnimation(elapsedSec);
}

void Nettler::SetDying(SoundManager& soundManager)
{
	m_SheetTop = 1311;
	m_SheetLeft = 714;
	m_SheetWidth = 136;
	m_SheetHeight = 44;

	m_Rows = 1;
	m_Cols = 3;

	SetDyingReset(soundManager);
}

void Nettler::SetDead()
{
	m_SheetTop = 1311;
	m_SheetLeft = 714;
	m_SheetWidth = 136;
	m_SheetHeight = 44;

	m_Rows = 1;
	m_Cols = 3;

	SetDeadReset();
}

void Nettler::SetAlive(const Point2f& spawnPoint)
{
	m_NrFramesPerSec = 4;

	m_SheetTop = 275;
	m_SheetLeft = 148;
	m_SheetWidth = 86;
	m_SheetHeight = 42;

	m_Rows = 1;
	m_Cols = 2;

	SetAliveReset(spawnPoint);
}

void Nettler::Dodge(float elapsedSec, const Avatar& avatar)
{
	if (m_IsDying)
	{
		return;
	}

	// Run timer, if is done reset state
	if (m_IsDodging || m_IsHalfDodging)
	{
		m_DodgeSec -= elapsedSec;
		if (m_DodgeSec <= 0)
		{
			m_DodgeSec = 0.25f;
			m_IsDodging = false;
			m_IsHalfDodging = false;


			m_AccumSec = 0;
			m_CurrFrame = 0;

			m_SheetTop = 275;
			m_SheetLeft = 148;
			m_SheetWidth = 86;
			m_SheetHeight = 42;

			m_Rows = 1;
			m_Cols = 2;


			m_Shape.width = m_SheetWidth / m_Cols;
			m_Shape.height = m_SheetHeight / m_Rows;

			m_SrcRect.width = m_SheetWidth / m_Cols;
			m_SrcRect.height = m_SheetHeight / m_Rows;
		}

		return;
	}

	const bool hasShot(avatar.GetShot());
	
	const Point2f midPosAvatar{ avatar.GetShape().left + avatar.GetShape().width / 2, avatar.GetShape().bottom + avatar.GetShape().height / 2 };
	const Point2f midPosEnemy{ m_Shape.left + m_Shape.width / 2, m_Shape.bottom + m_Shape.height / 2 };

	float offSet{ 25 };
	const bool isOnSameLevel{ midPosAvatar.y >= midPosEnemy.y - offSet && midPosAvatar.y <= midPosEnemy.y + offSet };
	
	offSet = 100;
	const bool isClose{ midPosAvatar.x >= midPosEnemy.x - offSet && midPosAvatar.x <= midPosEnemy.x + offSet };

	// Check if player is on same y-level and if he has shot
	if (isOnSameLevel && hasShot)
	{
		// If player shoots to close to enemy, he dodges
		if (isClose)
		{
			m_IsDodging = true;
		}
		else
		{
			m_IsHalfDodging = true;
		}		

		// Always show dodge-animation
		m_AccumSec = 0;
		m_CurrFrame = 0;

		m_SheetTop = 275;
		m_SheetLeft = 240;
		m_SheetWidth = 42;
		m_SheetHeight = 44;

		m_Rows = 1;
		m_Cols = 1;


		m_Shape.width = m_SheetWidth / m_Cols;
		m_Shape.height = m_SheetHeight / m_Rows;

		m_SrcRect.width = m_SheetWidth / m_Cols;
		m_SrcRect.height = m_SheetHeight / m_Rows;
	}
}

bool Nettler::GetDodge() const
{
	return m_IsDodging;
}

void Nettler::UpdateMovement(float elapsedSec, const Level& level)
{
	const bool isOnFloor{ level.IsOnGround(m_Shape) };
	const bool isAgainstLeftWall{ level.IsAgainstLeftWall(m_Shape) };
	const bool isAgainstRightWall{ level.IsAgainstRightWall(m_Shape) };

	// If not on floor, stop x-velocity
	if (!isOnFloor)
	{
		m_Velocity.x = 0;
		m_IsFalling = true;

		// Turn once, if falling
		if (!m_HasTurned)
		{
			m_IsLookingLeft = !m_IsLookingLeft;
			m_HasTurned = true;
		}

		m_Shape.left += m_Velocity.x * elapsedSec;
		m_Shape.bottom += m_Velocity.y * elapsedSec;
		return;
	}
	else
	{
		m_IsFalling = false;
		m_HasTurned = false;
	}

	// If against wall, turn
	if (isAgainstLeftWall)
	{
		m_IsLookingLeft = false;
	}
	if (isAgainstRightWall)
	{
		m_IsLookingLeft = true;
	}

	if (m_IsLookingLeft)
	{
		m_Velocity.x = -m_XSpeed;
	}
	else
	{
		m_Velocity.x = m_XSpeed;
	}

	m_Shape.left += m_Velocity.x * elapsedSec;
	m_Shape.bottom += m_Velocity.y * elapsedSec;
}