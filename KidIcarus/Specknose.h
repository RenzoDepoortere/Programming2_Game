#pragma once
#include "Enemy.h"

class Level;

class Specknose final : public Enemy
{
public:
	explicit Specknose(const std::string& texturePath, const Point2f& startPos, const Type& enemyType, float xSpeed, bool movesDiagonal);
	~Specknose() = default;
	Specknose(const Specknose& other) = delete;
	Specknose& operator= (const Specknose& rhs) = delete;
	Specknose(Specknose&& other) = delete;
	Specknose& operator= (Specknose&& rhs) = delete;

	virtual void Update(float elapsedSec, const Level& level) override;

	virtual void SetDying(SoundManager& soundManager) override;
	virtual void SetDead() override;
	virtual void SetAlive(const Point2f& spawnPoint) override;

	void SetMoveArea(const Rectf& moveArea);

	virtual Rectf GetShape() const override;

private:
	bool m_IsSpawning;
	bool m_SwitchedBack;
	const bool m_MovesDiagonal;
	bool m_StartUp;

	Point2f m_StartPos;
	Rectf m_MoveArea;
	int m_XFrames;
	int m_YFrames;

	// Helper functions
	void UpdateSpawner();
	void UpdateMovement(float elapsedSec);
	virtual void UpdateAnimation(float elapsedSec) override;
};

