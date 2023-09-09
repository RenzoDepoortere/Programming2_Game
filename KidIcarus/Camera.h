#pragma once

class Camera final
{
public:
	explicit Camera(const Rectf& cameraRect, const Rectf& levelBoundaries);
	~Camera() = default;
	Camera(const Camera& other) = delete;
	Camera& operator= (const Camera& rhs) = delete;
	Camera(Camera&& other) = delete;
	Camera& operator= (Camera&& rhs) = delete;

	void SetLevelBoundaries(const Rectf& levelBoundaries);
	void Transform(const Rectf& target) const;
	void Update(float elapsedSec, const Rectf& target, const Rectf& windowRect);
	
	Rectf GetCameraRect() const;
	void SetCameraRect(const Rectf& newRect);

private:
	// Variables
	Rectf m_CameraRect;
	Rectf m_LevelBoundaries;

	// Helper functions
	Point2f Track(const Rectf& target) const;
	void Clamp(Point2f& bottomleftPos) const;
};

