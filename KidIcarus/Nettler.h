#pragma once
#include "Enemy.h"

class Level;

class Nettler final : public Enemy
{
public:
	explicit Nettler(const std::string& texturePath, const Point2f& startPos, const Type& enemyType, float xSpeed);
	~Nettler() = default;
	Nettler(const Nettler& other) = delete;
	Nettler& operator= (const Nettler& rhs) = delete;
	Nettler(Nettler&& other) = delete;
	Nettler& operator= (Nettler&& rhs) = delete;

	virtual void Update(float elapsedSec, const Level& level) override;
	
	virtual void SetDying(SoundManager& soundManager) override;
	virtual void SetDead() override;
	virtual void SetAlive(const Point2f& spawnPoint) override;

	void Dodge(float elapsedSec, const Avatar& avatar);
	bool GetDodge() const;

private:
	bool m_IsFalling;
	bool m_HasTurned;
	float m_DodgeSec;
	bool m_IsDodging;
	bool m_IsHalfDodging;

	void UpdateMovement(float elapsedSec, const Level& level);
};

