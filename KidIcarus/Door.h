#pragma once
#include <vector>

class Texture;
class Avatar;
class Camera;
class Level;
class SoundManager;

class Door final
{
public:
	explicit Door(const std::string& texturePath, const std::vector<Point2f>& doorsPos,
				const std::vector<Point2f>& spawnPos, bool flipChar, const Rectf& mainLevelBoundary);
	~Door();
	Door(const Door& other) = delete;
	Door& operator=(const Door& rhs) = delete;
	Door(Door&& other) = delete;
	Door& operator=(Door&& rhs) = delete;

	void Draw() const;
	void Update(Avatar& avatar, Camera& camera, Level& level, SoundManager& soundManager);

	bool GetPassed() const;

private:
	const Texture* m_pTexture;
	std::vector<Point2f> m_DoorsPos;
	std::vector<Point2f> m_SpawnPos;
	
	const Rectf m_MainLevelBoundary;
	Rectf m_CameraRect;

	Rectf m_SrcRect;
	bool m_PassedThrough;
	bool m_FlipChar;
};

