#pragma once
#include "Vector2f.h"

class Texture;
class Level;
class Avatar;
class SoundManager;

class Enemy
{
public:
	enum class Type
	{
		Shemum, Nettler, Specknose
	};

	explicit Enemy(const std::string& texturePath, const Point2f& startPos, const Type& enemyType, float xSpeed);
	virtual ~Enemy();
	Enemy(const Enemy& other) = delete;
	Enemy& operator=(const Enemy& rhs) = delete;
	Enemy(Enemy&& other) = delete;
	Enemy& operator=(Enemy&& other) = delete;

	virtual void Draw() const;
	virtual void Update(float elapsedSec, const Level& level) = 0;

	virtual void SetDying(SoundManager& soundManager) = 0;
	bool GetDying() const;
	virtual void SetDead() = 0;
	bool GetDead() const;

	void SetRevived(bool setRevived);
	bool GetRevived() const;

	virtual void SetAlive(const Point2f& spawnPoint) = 0;
	void SetSpawnedItems(bool boolean = true);
	bool GetSpawnedItems() const;

	virtual Rectf GetShape() const;
	Point2f GetCords() const;
	float GetHeight() const;

	Type GetType() const;

protected:
	static Texture* m_pTexture;

	// General
	Rectf m_Shape;
	const float m_XSpeed;
	Vector2f m_Velocity;
	const Vector2f m_Acceleration;

	bool m_IsDead;
	bool m_IsLookingLeft;

	bool m_IsDying;
	bool m_SpawnedItem;

	bool m_GotRevived;

	// Sprite
	Rectf m_SrcRect;
	float m_AccumSec;
	int m_CurrFrame;
	int m_NrFramesPerSec;
	
	float m_SheetTop;
	float m_SheetLeft;
	float m_SheetWidth;
	float m_SheetHeight;

	int m_Rows;
	int m_Cols;

	// Helper Functions
	virtual void UpdateAnimation(float elapsedSec);
	void SpriteConstructor();

	void SetDyingReset(SoundManager& soundManager);
	void SetDeadReset();
	void SetAliveReset(const Point2f& spawnPoint);

private:
	Type m_EnemyType;
	static int m_InstanceCntr;

};

