#include "pch.h"
#include "Level.h"
#include "Texture.h"
#include "Vector2f.h"
#include "utils.h"
#include "SoundStream.h"

#include <iostream>

Level::Level(const std::vector<std::vector<Point2f>>& vertices, const std::string& backgroundPath,
	const std::string& foregroundPath, const Rectf& boundaryRect)
	: m_Vertices{vertices}
	, m_pBackgroundTexture{new Texture{backgroundPath}}
	, m_pForegroundTexture{new Texture{foregroundPath}}
	, m_Boundaries{boundaryRect}
	, m_CameraRect{}
{

}

Level::~Level()
{
	delete m_pBackgroundTexture;
	m_pBackgroundTexture = nullptr;

	delete m_pForegroundTexture;
	m_pForegroundTexture = nullptr;
}

void Level::DrawBackground() const
{
	m_pBackgroundTexture->Draw(Point2f{ 0,0 });
}

void Level::DrawForeground() const
{
	m_pForegroundTexture->Draw(Point2f{ 0,0 });
}

void Level::HandleCollision(Rectf& actorShape, Vector2f& actorVelocity, bool checkWall,
							bool useVelocity) const
{

	utils::HitInfo hitInfo{};

	// All Checking-Points
	const float gap{ 8 };

	const Point2f topLeftPos{ actorShape.left + gap,actorShape.bottom + actorShape.height };
	const Point2f botLeftPos{ actorShape.left + gap, actorShape.bottom - 1 };
	const Point2f topRightPos{ actorShape.left + actorShape.width - gap, actorShape.bottom + actorShape.height };
	const Point2f botRightPos{ actorShape.left + actorShape.width - gap, actorShape.bottom - 1 };

	const Point2f tLeftPos{ actorShape.left, actorShape.bottom + actorShape.height - gap };
	const Point2f bLeftPos{ actorShape.left, actorShape.bottom + gap };
	const Point2f tRightPos{ actorShape.left + actorShape.width, actorShape.bottom + actorShape.height - gap };
	const Point2f bRightPos{ actorShape.left + actorShape.width, actorShape.bottom + gap };

	const Point2f leftPos{ actorShape.left, actorShape.bottom + actorShape.height / 2 };
	const Point2f rightPos{ actorShape.left + actorShape.width, actorShape.bottom + actorShape.height / 2 };

	const Point2f topPos{ actorShape.left + actorShape.width / 2, actorShape.bottom + actorShape.height };
	const Point2f botPos{ actorShape.left + actorShape.width / 2, actorShape.bottom };

	const Point2f topLeftHalf{ leftPos.x ,(tLeftPos.y - leftPos.y) / 2 + leftPos.y };
	const Point2f topRightHalf{ rightPos.x ,(tRightPos.y - rightPos.y) / 2 + rightPos.y };

	const Point2f botLeftHalf{ leftPos.x,(leftPos.y - bLeftPos.y) / 2 + bLeftPos.y };
	const Point2f botRightHalf{ rightPos.x,(rightPos.y - bRightPos.y) / 2 + bRightPos.y };

	// Ground and ceiling collision
	for (size_t i{}; i < m_Vertices.size(); i++)
	{
		bool hitLeft{ utils::Raycast(m_Vertices[i], botLeftPos, topLeftPos, hitInfo) };
		bool hitRight{ utils::Raycast(m_Vertices[i], botRightPos, topRightPos, hitInfo) };
		bool hitMid{ utils::Raycast(m_Vertices[i], topPos, botPos, hitInfo) };

		// If Collision
		if (hitLeft || hitRight || hitMid)
		{
			// If falling or standing still, handle fall collision
			if (actorVelocity.y <= 0)
			{
				actorShape.bottom = hitInfo.intersectPoint.y;
				actorVelocity.y = 0;

				break;
			}
			// If hitting head, handle jump collision
			else
			{
				actorShape.bottom = hitInfo.intersectPoint.y - actorShape.height - 1;
				actorVelocity.y = 0;

				break;
			}
		}
	}

	if (!checkWall)
	{
		return;
	}

	// Wall collision
	for (size_t i{}; i < m_Vertices.size(); i++)
	{
		bool hitTop{ utils::Raycast(m_Vertices[i], tLeftPos, tRightPos, hitInfo) };
		bool hitBot{ utils::Raycast(m_Vertices[i], bLeftPos, bRightPos, hitInfo) };
		bool hitMid{ utils::Raycast(m_Vertices[i],leftPos,rightPos,hitInfo) };
		bool hitTopHalf{ utils::Raycast(m_Vertices[i],topLeftHalf,topRightHalf,hitInfo) };
		bool hitBotHalf{ utils::Raycast(m_Vertices[i],botLeftHalf,botRightHalf,hitInfo) };

		// If collision
		if (hitTop || hitBot || hitMid || hitTopHalf || hitBotHalf)
		{
			// If check walls with velocity
			if (useVelocity)
			{
				// If going Left, handle left collision
				if (actorVelocity.x < 0)
				{
					actorShape.left = hitInfo.intersectPoint.x;
					actorVelocity.x = 0;

					break;
				}
				// If going right, handle right collision
				else
				{
					actorShape.left = hitInfo.intersectPoint.x - actorShape.width;
					actorVelocity.x = 0;

					break;
				}
			}
			else
			{
				// Checks which is closer
				if (actorShape.left + actorShape.width / 2 < hitInfo.intersectPoint.x)
				{
					actorShape.left = hitInfo.intersectPoint.x - actorShape.width;
					actorVelocity.x = 0;

					break;
				}
				else
				{
					actorShape.left = hitInfo.intersectPoint.x;
					actorVelocity.x = 0;

					break;
				}
			}
		}
	}
}

bool Level::IsOnGround(const Rectf& actorShape) const
{
	utils::HitInfo hitInfo{};

	const float gap{ 8 };

	const Point2f topLeftPos{ actorShape.left + gap,actorShape.bottom + actorShape.height };
	const Point2f botLeftPos{ actorShape.left + gap, actorShape.bottom - 1 };
	const Point2f topRightPos{ actorShape.left + actorShape.width - gap, actorShape.bottom + actorShape.height };
	const Point2f botRightPos{ actorShape.left + actorShape.width - gap, actorShape.bottom - 1 };

	bool isOnGround{};

	for (size_t i{}; i < m_Vertices.size(); i++)
	{
		// Check if is on ground anywhere
		isOnGround = utils::Raycast(m_Vertices[i], botLeftPos, topLeftPos, hitInfo)
			|| utils::Raycast(m_Vertices[i], botRightPos, topRightPos, hitInfo);

		// If it is, stop loop and give bool
		if (isOnGround)
		{
			break;
		}
	}

	return isOnGround;
}
bool Level::IsAgainstLeftWall(const Rectf& actorShape) const
{
	utils::HitInfo hitInfo{};

	const float gap{ 8 };

	const Point2f tLeftPos{ actorShape.left - 1, actorShape.bottom + actorShape.height - gap };
	const Point2f bLeftPos{ actorShape.left - 1, actorShape.bottom + gap };
	
	const Point2f top{ actorShape.left + actorShape.width / 2 - gap, actorShape.bottom + actorShape.height - gap };
	const Point2f bot{ actorShape.left + actorShape.width / 2 - gap, actorShape.bottom + gap };

	for (size_t i{}; i < m_Vertices.size(); i++)
	{
		bool hitTop{ utils::Raycast(m_Vertices[i], tLeftPos, top, hitInfo) };
		bool hitBot{ utils::Raycast(m_Vertices[i], bLeftPos, bot, hitInfo) };

		if (hitTop || hitBot)
		{
			return true;
		}
	}

	return false;
}
bool Level::IsAgainstRightWall(const Rectf& actorShape) const
{
	utils::HitInfo hitInfo{};

	const float gap{ 8 };

	const Point2f tRightPos{ actorShape.left + actorShape.width + 1, actorShape.bottom + actorShape.height - gap };
	const Point2f bRightPos{ actorShape.left + actorShape.width + 1, actorShape.bottom + gap };

	const Point2f top{ actorShape.left + actorShape.width / 2 + gap, actorShape.bottom + actorShape.height - gap };
	const Point2f bot{ actorShape.left + actorShape.width / 2 + gap, actorShape.bottom + gap };

	for (size_t i{}; i < m_Vertices.size(); i++)
	{
		bool hitTop{ utils::Raycast(m_Vertices[i], tRightPos, top, hitInfo) };
		bool hitBot{ utils::Raycast(m_Vertices[i], bRightPos, bot, hitInfo) };

		if (hitTop || hitBot)
		{
			return true;
		}
	}

	return false;
}

void Level::HandleTeleport(Rectf& actorShape, Rectf& srcRect) const
{
	const float gap{ 5 };
	const float middleActorHeight{ actorShape.bottom + actorShape.height / 2 };

	// Trace left border on actor height
	const Point2f firstLeftPos{ m_Boundaries.left - gap, middleActorHeight };
	const Point2f firstRightPos{ m_Boundaries.left + gap,middleActorHeight };

	// Trace right border on actor height
	const Point2f secondLeftPos{ m_Boundaries.left + m_Boundaries.width - gap, middleActorHeight };
	const Point2f secondRightPos{ m_Boundaries.left + m_Boundaries.width + gap, middleActorHeight };

	utils::HitInfo hitInfo{};

	// If collision, stop character
	for (size_t i{}; i < m_Vertices.size(); i++)
	{
		const bool ifLeftHit{ utils::Raycast(m_Vertices[i], firstLeftPos, firstRightPos, hitInfo) };
		const bool ifRightHit{ utils::Raycast(m_Vertices[i], secondLeftPos, secondRightPos, hitInfo) };

		if (ifLeftHit || ifRightHit)
		{
			if (actorShape.left < m_Boundaries.left)
			{
				actorShape.left = m_Boundaries.left;
				return;
			}
			else if (actorShape.left + actorShape.width > m_Boundaries.left + m_Boundaries.width)
			{
				actorShape.left = m_Boundaries.left + m_Boundaries.width - actorShape.width;
				return;
			}
		}
	}


	// If no collision, teleport actor to other side
	if (actorShape.left + actorShape.width - 1 < m_Boundaries.left)
	{
		actorShape.left = m_Boundaries.left + m_Boundaries.width - actorShape.width;
	}
	else if (actorShape.left + 1 > m_Boundaries.left + m_Boundaries.width)
	{
		actorShape.left = m_Boundaries.left;
	}
}

bool Level::TopFree(float gap) const
{
	utils::HitInfo hitInfo{};

	// Check if place enough to spawn
	const Point2f firstPos{ m_CameraRect.left + m_CameraRect.width / 2, m_CameraRect.bottom + m_CameraRect.height + gap / 2 };
	const Point2f secPos{ m_CameraRect.left + m_CameraRect.width / 2, m_CameraRect.bottom + m_CameraRect.height - gap };

	// Check if can escape
	const float distance{ gap * 4 };
	const Point2f leftPos{ firstPos.x - distance, m_CameraRect.bottom + m_CameraRect.height };
	const Point2f rightPos{ firstPos.x + distance, m_CameraRect.bottom + m_CameraRect.height };
	
	// Check if ground beneath, so they don't fall on player
	const Point2f topPos{ m_CameraRect.left + m_CameraRect.width / 2, m_CameraRect.bottom + m_CameraRect.height };
	const Point2f botPos{ m_CameraRect.left + m_CameraRect.width / 2, m_CameraRect.bottom };

	bool isFree{ true };

	for (size_t i{}; i < m_Vertices.size(); i++)
	{
		if (utils::Raycast(m_Vertices[i], firstPos, secPos, hitInfo))
		{
			isFree = false;
			break;
		}
		else if(utils::Raycast(m_Vertices[i], leftPos, topPos, hitInfo) && utils::Raycast(m_Vertices[i], rightPos, topPos, hitInfo))
		{
			isFree = false;
			break;
		}
	/*	else if(!utils::Raycast(m_Vertices[i], topPos,botPos,hitInfo))
		{
			isFree = false;
			break;
		}*/
	}

	return isFree;
}

Rectf Level::GetBoundary() const
{
	return m_Boundaries;
}

void Level::SetBoundary(const Rectf& newBoundary)
{
	m_Boundaries = newBoundary;
}

void Level::SetCameraRect(const Rectf& cameraRect)
{
	m_CameraRect = cameraRect;
}

bool Level::IsOutsideScreen(const Rectf& actorShape, bool hasTeleported) const
{
	const float maxCameraY{ 1407 };

	if (hasTeleported)
	{
		// If bottom is outside screen, make char. fall --> death
		if (actorShape.bottom - 1 <= m_CameraRect.bottom)
		{
			return true;
		}

		return false;
	}

	// If Camera above max, use this check
	if (m_CameraRect.bottom >= maxCameraY)
	{
		if (actorShape.bottom - 1 <= maxCameraY)
		{
			return true;
		}

		return false;
	}

	// If bottom is outside screen, make char. fall --> death
	if (actorShape.bottom - 1 <= m_CameraRect.bottom)
	{
		return true;
	}

	return false;
}