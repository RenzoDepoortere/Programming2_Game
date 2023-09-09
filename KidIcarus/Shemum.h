#pragma once
#include "Enemy.h"

class Level;
class Avatar;

class Shemum final : public Enemy
{
public:
	explicit Shemum(const std::string& texturePath, const Point2f& startPos, const Type& enemyType, float xSpeed);
	~Shemum() = default;
	Shemum(const Shemum& other) = delete;
	Shemum& operator=(const Shemum& rhs) = delete;
	Shemum(Shemum&& other) = delete;
	Shemum& operator=(Shemum&& rhs) = delete;

	virtual void Update(float elapsedSec, const Level& level) override;
	
	virtual void SetDying(SoundManager& soundManager) override;
	virtual void SetDead() override;
	virtual void SetAlive(const Point2f& spawnPoint) override;

private:
	bool m_HasTurned;
	bool m_IsFalling;
	bool m_ChangeDirect;

	// Helper functions
	void UpdateMovement(float elapsedSec, const Level& level);
};

