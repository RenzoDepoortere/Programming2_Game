#include "pch.h"
#include "Specknose.h"
#include "utils.h"

#include <math.h>
#include <iostream>

Specknose::Specknose(const std::string& texturePath, const Point2f& startPos, const Type& enemyType, float xSpeed, bool movesDiagonal)
	: Enemy{ texturePath,startPos,enemyType,xSpeed }
	, m_IsSpawning{ false }
	, m_SwitchedBack{ false }
	, m_MovesDiagonal{movesDiagonal}
	, m_StartUp{true}
	, m_StartPos{}
	, m_MoveArea{}
	, m_XFrames{}
	, m_YFrames{}
{
	m_NrFramesPerSec = 4;

	m_SheetTop = 55;
	m_SheetLeft = 240;
	m_SheetWidth = 42;
	m_SheetHeight = 44;

	m_Rows = 1;
	m_Cols = 1;

	SpriteConstructor();
}

void Specknose::Update(float elapsedSec, const Level& level)
{
	// Update spawner
	UpdateSpawner();
	
	// Update movement
	if (!m_IsSpawning && !m_IsDying)
	{
		UpdateMovement(elapsedSec);
	}

	// Update animation
	UpdateAnimation(elapsedSec);
}

void Specknose::SetDying(SoundManager& soundManager)
{
	m_SheetTop = 1311;
	m_SheetLeft = 1002;
	m_SheetWidth = 134;
	m_SheetHeight = 44;

	m_Rows = 1;
	m_Cols = 3;

	SetDyingReset(soundManager);
}

void Specknose::SetDead()
{
	m_SheetTop = 1311;
	m_SheetLeft = 1002;
	m_SheetWidth = 134;
	m_SheetHeight = 44;

	m_Rows = 1;
	m_Cols = 3;

	SetDeadReset();
}

void Specknose::SetMoveArea(const Rectf& moveArea)
{
	m_MoveArea = moveArea;
}

void Specknose::SetAlive(const Point2f& spawnPoint)
{
	m_StartPos.x = spawnPoint.x;
	m_StartPos.y = spawnPoint.y;

	m_NrFramesPerSec = 4;

	m_SheetTop = 55;
	m_SheetLeft = 240;
	m_SheetWidth = 42;
	m_SheetHeight = 44;

	m_Rows = 1;
	m_Cols = 1;

	m_IsSpawning = true;
	
	SetAliveReset(spawnPoint);
}

Rectf Specknose::GetShape() const
{
	if (m_IsDying)
	{
		return Rectf{ 0,0,0,0 };
	}

	return m_Shape;
}

void Specknose::UpdateSpawner()
{
	if (m_SwitchedBack)
	{
		return;
	}

	if (m_IsSpawning)
	{
		m_NrFramesPerSec = 6;

		m_SheetTop = 1311;
		m_SheetLeft = 1144;
		m_SheetWidth = 136;
		m_SheetHeight = 44;

		m_Rows = 1;
		m_Cols = 3;
	}
	else
	{
		m_NrFramesPerSec = 4;

		m_SheetTop = 55;
		m_SheetLeft = 240;
		m_SheetWidth = 42;
		m_SheetHeight = 44;

		m_Rows = 1;
		m_Cols = 1;

		m_SwitchedBack = true;
	}


	m_Shape.width = m_SheetWidth / m_Cols;
	m_Shape.height = m_SheetHeight / m_Rows;

	m_SrcRect.width = m_SheetWidth / m_Cols;
	m_SrcRect.height = m_SheetHeight / m_Rows;
}

void Specknose::UpdateMovement(float elapsedSec)
{
	const float xPos{ m_MoveArea.left + m_MoveArea.width / 2 };
	const float maxX{ m_MoveArea.width / 2 };
	const float yPos{ m_MoveArea.bottom + m_MoveArea.height / 2 };
	const float maxY{ m_MoveArea.height / 2 };

	// In start up, calculates where in cycle the Enemy is
	if (m_StartUp)
	{
		const float xValue{ (m_StartPos.x - (m_MoveArea.left + m_MoveArea.width / 2)) / m_MoveArea.width / 2 };
		const float xAngle{ asinf(xValue) };
		m_XFrames = int(xValue * m_XSpeed);

		const float yValue{ (m_StartPos.y - (m_MoveArea.bottom + m_MoveArea.height / 2)) / m_MoveArea.height / 2 };
		const float yAngle{ asinf(yValue) };
		m_YFrames = int(yValue * m_XSpeed);

		if (m_MovesDiagonal)
		{
			m_Shape.bottom = yPos + maxY * float(sin(2 * yAngle * float(M_PI)));
		}
		m_Shape.left = xPos + maxX * float(sin(2 * xAngle * float(M_PI)));

		m_StartUp = false;
		return;
	}

	// furthers the cycle
	++m_XFrames;
	const float xAngle{ float(m_XFrames) / m_XSpeed };

	++m_YFrames;
	const float yAngle{ float(m_YFrames) / m_XSpeed };

	if (m_MovesDiagonal)
	{
		m_Shape.bottom = yPos + maxY * float(sin(2 * yAngle * float(M_PI)));
	}
	m_Shape.left = xPos + maxX * float(sin(2 * xAngle * float(M_PI)));
}

void Specknose::UpdateAnimation(float elapsedSec)
{
	m_AccumSec += elapsedSec;
	const float frameTime{ 1.f / m_NrFramesPerSec };

	if (m_AccumSec >= frameTime)
	{
		if (m_IsDying || m_IsSpawning)
		{
			++m_CurrFrame %= (m_Cols * m_Rows);
		}
		else
		{
			m_IsLookingLeft = !m_IsLookingLeft;
		}
		m_AccumSec -= frameTime;
	}

	// Play Smoke-animation when dying or spawning
	if (m_IsDying || m_IsSpawning)
	{
		m_SrcRect.left = m_SheetLeft + m_CurrFrame % m_Cols * m_SrcRect.width;
		m_SrcRect.bottom = m_SheetTop + (m_CurrFrame / m_Cols + 1) * m_SrcRect.height;
	}
	else
	{
		m_SrcRect.left = m_SheetLeft;
		m_SrcRect.bottom = m_SheetTop + m_SrcRect.height;
	}

	// If animation done, spawn
	if (m_IsSpawning && m_CurrFrame == m_Cols - 1)
	{
		m_IsSpawning = false;
	}

	// If animation done, kill enemy
	if (m_IsDying && m_CurrFrame == m_Cols - 1)
	{
		m_IsDead = true;
		m_StartUp = true;
		m_SwitchedBack = false;
	}
}