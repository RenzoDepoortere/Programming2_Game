#include "pch.h"
#include "Camera.h"
#include "utils.h"

#include <iostream>

Camera::Camera(const Rectf& cameraRect, const Rectf& levelBoundaries)
	: m_CameraRect{cameraRect}
	, m_LevelBoundaries{levelBoundaries}
{

}

void Camera::SetLevelBoundaries(const Rectf& levelBoundaries)
{
	m_LevelBoundaries = levelBoundaries;
}

void Camera::Transform(const Rectf& target) const
{
	Point2f cameraPos{ Track(target) };

	Clamp(cameraPos);

	glTranslatef(-cameraPos.x, -cameraPos.y, 0);
}

Point2f Camera::Track(const Rectf& target) const
{
	Point2f cameraPos{};

	cameraPos.x = (target.left + target.width / 2) - m_CameraRect.width / 2;
	cameraPos.y = m_CameraRect.bottom;

	return cameraPos;
}

void Camera::Clamp(Point2f& bottomleftPos) const
{
	// Keeps camera inside x-field
	if ((bottomleftPos.x + m_CameraRect.width) - (m_LevelBoundaries.left + m_LevelBoundaries.width) >= 0)
	{
		bottomleftPos.x = (m_LevelBoundaries.left + m_LevelBoundaries.width) - m_CameraRect.width;
	}
	if (bottomleftPos.x - m_LevelBoundaries.left <= 0)
	{
		bottomleftPos.x = m_LevelBoundaries.left;
	}

	// Keep camera inside y-field
	if ((bottomleftPos.y + m_CameraRect.height) - (m_LevelBoundaries.bottom + m_LevelBoundaries.height) >= 0)
	{
		bottomleftPos.y = (m_LevelBoundaries.bottom + m_LevelBoundaries.height) - m_CameraRect.height;
	}
	if (bottomleftPos.y - m_LevelBoundaries.bottom <= 0)
	{
		bottomleftPos.y = m_LevelBoundaries.bottom;
	}
}

void Camera::Update(float elapsedSec, const Rectf& target, const Rectf& windowRect)
{
	const float speed{ 10 };

	// If target is higher than mid-screen
	if (target.bottom + target.height / 2 > windowRect.bottom + windowRect.height / 2)
	{
		// Elevate camera y-pos, until character at mid-screen again
		while (m_CameraRect.bottom + m_CameraRect.height / 2 < target.bottom + target.height / 2)
		{
			m_CameraRect.bottom += speed * elapsedSec;
		}
	}
	
	// Re-Adjust cameraPos
	Point2f cameraPos{ m_CameraRect.left,m_CameraRect.bottom };
	Clamp(cameraPos);
}

Rectf Camera::GetCameraRect() const
{
	return m_CameraRect;
}

void Camera::SetCameraRect(const Rectf& newRect)
{
	m_CameraRect = newRect;
}