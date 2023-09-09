#include "pch.h"
#include "Shemum.h"
#include "Texture.h"
#include "Level.h"

#include <iostream>

Shemum::Shemum(const std::string& texturePath, const Point2f& startPos, const Type& enemyType, float xSpeed)
	: Enemy{texturePath, startPos, enemyType, xSpeed}
	, m_HasTurned{false}
	, m_IsFalling{false}
	, m_ChangeDirect{false}
{
	m_NrFramesPerSec = 12;

	m_SheetTop = 55;
	m_SheetLeft = 3;
	m_SheetWidth = 102;
	m_SheetHeight = 44;

	m_Rows = 1;
	m_Cols = 4;

	SpriteConstructor();
}

void Shemum::Update(float elapsedSec, const Level& level)
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

	// Handles animation
	UpdateAnimation(elapsedSec);
}

void Shemum::SetDying(SoundManager& soundManager)
{
	m_SheetTop = 1395;
	m_SheetLeft = 714;
	m_SheetWidth = 72;
	m_SheetHeight = 44;

	m_Rows = 1;
	m_Cols = 3;

	SetDyingReset(soundManager);
}

void Shemum::SetDead()
{
	m_SheetTop = 1395;
	m_SheetLeft = 714;
	m_SheetWidth = 72;
	m_SheetHeight = 44;

	m_Rows = 1;
	m_Cols = 3;

	SetDeadReset();
}

void Shemum::SetAlive(const Point2f& spawnPoint)
{
	m_NrFramesPerSec = 12;

	m_SheetTop = 55;
	m_SheetLeft = 3;
	m_SheetWidth = 102;
	m_SheetHeight = 44;

	m_Rows = 1;
	m_Cols = 4;

	SetAliveReset(spawnPoint);
}

// HELPER FUNCTIONS

void Shemum::UpdateMovement(float elapsedSec, const Level& level)
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