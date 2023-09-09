#pragma once
#include "Avatar.h"
#include <vector>

class Texture;
struct Vector2f;

class Level final
{
public:
	explicit Level(const std::vector<std::vector<Point2f>>& vertices, const std::string& backgroundPath,
		const std::string& foregroundPath, const Rectf& boundaryRect);
	~Level();
	Level(const Level& other) = delete;
	Level& operator= (const Level& rhs) = delete;
	Level(Level&& other) = delete;
	Level& operator= (Level&& rhs) = delete;

	void DrawBackground() const;
	void DrawForeground() const;

	void HandleCollision(Rectf& actorShape, Vector2f& actorVelocity, bool checkWall = true,
						bool useVelocity = true) const;
	bool IsOnGround(const Rectf& actorShape) const;
	
	bool IsAgainstLeftWall(const Rectf& actorShape) const;
	bool IsAgainstRightWall(const Rectf& actorShape) const;
	
	void HandleTeleport(Rectf& actorShape, Rectf& srcRect) const;

	bool TopFree(float gap) const;

	Rectf GetBoundary() const;
	void SetBoundary(const Rectf& newBoundary);

	void SetCameraRect(const Rectf& cameraRect);

	bool IsOutsideScreen(const Rectf& actorShape, bool hasTeleported = false) const;

private:
	std::vector<std::vector<Point2f>> m_Vertices;

	const Texture* m_pBackgroundTexture;
	const Texture* m_pForegroundTexture;

	Rectf m_Boundaries;
	Rectf m_CameraRect;
};

