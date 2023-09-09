#pragma once
#include "Vector2f.h"

class Level;
class Texture;
class Arrow;
class SoundManager;
class HUDManager;

class Avatar final
{
public:
	enum class mState
	{
		waiting, jumping, moving, dying
	};

	enum class aState
	{
		idle, ducking, lookingUp, dead
	};

	explicit Avatar(int maxHealth, const Rectf& shapeRect, float xSpeed, float ySpeed,
					const std::string& texturePath, const std::string& hurtPath);
	~Avatar();
	Avatar(const Avatar& other) = delete;
	Avatar& operator=(const Avatar& rhs) = delete;
	Avatar(Avatar&& other) = delete;
	Avatar& operator=(Avatar&& rhs) = delete;

	void Draw() const;
	void Update(float elapsedSec, const Level& level, SoundManager& soundManager);
	
	Rectf GetShape() const;
	Rectf GetCollisionRect() const;
	
	void SetHurt(SoundManager& soundManager, HUDManager& hudManager);
	
	Rectf GetArrow() const;
	void DeactivateArrow();
	bool GetShot() const;
	
	void SetPoint(const Point2f& position);
	
	void SetLookToLeft(bool lookLeft = true);
	bool GetLookToLeft() const;
	
	void SetPause();
	void SetIsTeleported(bool isTeleported);
	
	aState GetActionState() const;
	
	int GetHealth() const;
	void SetHealth(int newHealth);

	int GetScore() const;
	void SetScore(int newScore, HUDManager& hudManager);

private:
	// General variables
	Rectf m_Shape;
	Rectf m_CollisionRect;
	const float m_XSpeed;
	const float m_YSpeed;
	Vector2f m_Velocity;
	mState m_MoveState;
	aState m_ActionState;
	const Vector2f m_Acceleration;
	bool m_IsLookingLeft;
	
	// HUD variables
	int m_Health;
	int m_Score;

	// Jump Variables
	const float m_MaxJumpHeight;
	float m_JumpTime;
	bool m_DidOnce;
	bool m_StillJumping;

	// Teleport Variables
	bool m_TeleportPausing;
	float m_TeleportPauseSec;
	bool m_IsTeleported;

	// Hurt variables
	const Texture* m_pHurtText;
	bool m_IsHurt;
	const float m_InvincibilityTime;
	float m_HurtSec;
	float m_HurtAccumSec;
	int m_HurtFrame;
	const int m_HurtpSec;
	float m_DyingSec;

	// Sprite variables
	const Texture* m_pTexture;
	Rectf m_SrcRect;
	const int m_NrFramesPerSec;
	float m_AccumSec;
	int m_CurrFrame;
	float m_SheetTop;
	float m_SheetLeft;
	float m_SheetWidth;
	float m_SheetHeight;
	int m_Rows;
	int m_Cols;
	

	// Arrow variables
	Arrow* m_pArrow;
	float m_ArrowSec;
	float m_ShootCooldownSec;
	bool m_ShootCooldown;
	bool m_IsShot;
	Point2f m_StartPos;

	// Helper functions
	void UpdateMovement(float elapsedSec, bool isOnFloor, bool isAgainstLeftWall,
						bool isAgainstRightWall, SoundManager& soundManager);
	void UpdateAction(float elapsedSec, SoundManager& soundManager);

	void MovementSwitch(float elapsedSec);
	void ActionSwitch(bool isOnFloor);

	void UpdateAnimation(float elapsedSec);
	void SpriteSelect();

	void UpdateHurt(float elapsedSec);

	void ShootArrow();
	void UpdateArrow(float elapsedSec, const Level& level);
};

