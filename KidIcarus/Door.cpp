#include "pch.h"
#include "Door.h"
#include "Texture.h"
#include "Avatar.h"
#include "utils.h"
#include "Camera.h"
#include "Level.h"
#include "SoundManager.h"

Door::Door(const std::string& texturePath, const std::vector<Point2f>& doorsPos,
	const std::vector<Point2f>& spawnPos, bool flipChar, const Rectf& mainLevelBoundary)
	: m_pTexture{new Texture{texturePath}}
	, m_DoorsPos{doorsPos}
	, m_SpawnPos{spawnPos}
	, m_MainLevelBoundary{mainLevelBoundary}
	, m_CameraRect{}
	, m_SrcRect{880,3733,54,108}
	, m_PassedThrough{false}
	, m_FlipChar{ flipChar }
{
}

Door::~Door()
{
	delete m_pTexture;
	m_pTexture = nullptr;
}

void Door::Draw() const
{
	m_pTexture->Draw(m_DoorsPos[0], m_SrcRect);
}

void Door::Update(Avatar& avatar, Camera& camera, Level& level, SoundManager& soundManager)
{
	const Rectf firstDoor{ m_DoorsPos[0].x,m_DoorsPos[0].y,m_SrcRect.width,m_SrcRect.height };
	const Rectf secondDoor{ m_DoorsPos[1].x,m_DoorsPos[1].y,m_SrcRect.width,m_SrcRect.height };
	const Rectf avatarRect{ avatar.GetShape() };

	const bool throughFirst{ utils::IsOverlapping(avatarRect, firstDoor) };
	const bool throughSecond{ utils::IsOverlapping(avatarRect,secondDoor) };

	const bool isLookingLeft{ avatar.GetLookToLeft() };

	Rectf boundaries{};

	// If going through first, and hasn't done before
	if (throughFirst && !m_PassedThrough)
	{
		// Remember cameraRect;
		m_CameraRect = camera.GetCameraRect();

		m_PassedThrough = true;
		m_SrcRect.left = 938;

		if (m_FlipChar)
		{
			avatar.SetLookToLeft(!isLookingLeft);
		}

		// Teleport player
		avatar.SetPoint(m_SpawnPos[0]);
		avatar.SetPause();
		avatar.SetIsTeleported(true);

		// Set boundaries & camera
		boundaries = Rectf{ 881,13665,860,810 };
		camera.SetLevelBoundaries(boundaries);
		camera.SetCameraRect(boundaries);
		level.SetBoundary(boundaries);

		soundManager.PlayTheme(SoundManager::Theme::Dungeon, true);
		return;
	}

	// If going through second
	if (throughSecond)
	{
		if (m_FlipChar)
		{
			avatar.SetLookToLeft(!isLookingLeft);
		}

		// Teleport player
		avatar.SetPoint(m_SpawnPos[1]);
		avatar.SetPause();
		avatar.SetIsTeleported(false);

		// Set boundaries & camera
		camera.SetLevelBoundaries(m_MainLevelBoundary);
		camera.SetCameraRect(m_CameraRect);
		level.SetBoundary(m_MainLevelBoundary);

		soundManager.PlayTheme(SoundManager::Theme::Level, true);
	}
}

bool Door::GetPassed() const
{
	return m_PassedThrough;
}