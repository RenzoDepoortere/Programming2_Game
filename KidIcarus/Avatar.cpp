#include "pch.h"
#include "Avatar.h"
#include "utils.h"
#include "Level.h"
#include "Texture.h"
#include "Arrow.h"
#include "SoundManager.h"
#include "HUDManager.h"
#include <iostream>

Avatar::Avatar(int maxHealth, const Rectf& shapeRect, float xSpeed, float ySpeed,
	const std::string& texturePath, const std::string& hurtPath)
	// General Variables
	: m_Shape{shapeRect}
	, m_CollisionRect{shapeRect}
	, m_XSpeed{xSpeed}
	, m_YSpeed{ySpeed}
	, m_Velocity{0,0}
	, m_MoveState{ mState::moving}
	, m_ActionState{aState::idle}
	, m_Acceleration{0,-981}
	, m_IsLookingLeft{false}
	// HUD Variables
	, m_Health{maxHealth}
	, m_Score{}
	// Jump Variables
	, m_MaxJumpHeight{0.25f}
	, m_JumpTime{}
	, m_DidOnce{false}
	, m_StillJumping{false}
	// Teleport Variables
	, m_TeleportPausing{false}
	, m_TeleportPauseSec{}
	, m_IsTeleported{false}
	// Hurt Variables
	, m_pHurtText{ new Texture{hurtPath} }
	, m_IsHurt{false}
	, m_InvincibilityTime{1.5f}
	, m_HurtSec{}
	, m_HurtAccumSec{}
	, m_HurtFrame{}
	, m_HurtpSec{ 8 }
	, m_DyingSec{}
	// Sprite Variables
	, m_pTexture{new Texture{texturePath}}
	, m_SrcRect{}
	, m_NrFramesPerSec{12}
	, m_AccumSec{}
	, m_CurrFrame{}
	, m_SheetLeft{}
	, m_SheetTop{}
	, m_SheetWidth{}
	, m_SheetHeight{}
	, m_Rows{}
	, m_Cols{}
	// Arrow Variables
	, m_pArrow{ new Arrow{texturePath,500 } }
	, m_ArrowSec{}
	, m_ShootCooldownSec{0}
	, m_ShootCooldown{false}
	, m_IsShot{false}
	, m_StartPos{}
{
}

Avatar::~Avatar()
{
	delete m_pHurtText;
	m_pHurtText = nullptr;

	delete m_pTexture;
	m_pTexture = nullptr;

	delete m_pArrow;
	m_pArrow = nullptr;
}

void Avatar::Draw() const
{	
	glPushMatrix();

	// Flip Sprite when looking left
	if (m_IsLookingLeft)
	{
		glTranslatef(m_Shape.left + m_Shape.width / 2, m_Shape.bottom, 0);
		glScalef(-1, 1, 1);
		glTranslatef(-m_Shape.left - m_Shape.width / 2, -m_Shape.bottom, 0);
	}

	// Switch texture when hurt
	if (m_IsHurt)
	{
		m_pHurtText->Draw(m_Shape, m_SrcRect);
	}
	else
	{
		m_pTexture->Draw(m_Shape, m_SrcRect);
	}
	
	glPopMatrix();

	// Draws bullet;
	m_pArrow->Draw();
}

void Avatar::Update(float elapsedSec, const Level& level, SoundManager& soundManager)
{
	// equilize Shape and collisionRect
	m_CollisionRect.left = m_Shape.left;
	m_CollisionRect.bottom = m_Shape.bottom;

	// Handles side-swap
	level.HandleTeleport(m_CollisionRect,m_SrcRect);

	// Handles gravity
	m_Velocity.y += m_Acceleration.y * elapsedSec;

	// Check if fallen out of screen
	if (level.IsOutsideScreen(m_CollisionRect, m_IsTeleported))
	{
		m_ActionState = aState::dead;
		m_IsHurt = false;
	}
	
	// If not dead, or dying --> handle Collision
	if (!(m_ActionState == aState::dead || m_MoveState == mState::dying))
	{
		level.HandleCollision(m_CollisionRect, m_Velocity, true, false);
	}

	// equilize Shape and collisionRect
	m_Shape.left = m_CollisionRect.left;
	m_Shape.bottom = m_CollisionRect.bottom;

	// Check if colliding with something
	bool isOnFloor{ level.IsOnGround(m_CollisionRect) };
	bool isAgainstLeftWall{level.IsAgainstLeftWall(m_CollisionRect)};
	bool isAgainstRightWall{level.IsAgainstRightWall(m_CollisionRect)};

	// Doesn't let player move for a moment after using doors
	if (m_TeleportPausing)
	{
		m_TeleportPauseSec -= elapsedSec;

		if (m_TeleportPauseSec <= 0)
		{
			m_TeleportPausing = false;
		}
	}

	if (!m_TeleportPausing)
	{
		// Reset jump-variables when on floor
		if (isOnFloor)
		{
			m_DidOnce = false;
			m_JumpTime = 0;
		}

		// If Health less than 0 --> die
		if (m_Health <= 0)
		{
			m_DyingSec += elapsedSec;
			m_MoveState = mState::dying;
		}

		// If not dead or dying --> update
		if (!(m_ActionState == aState::dead || m_MoveState == mState::dying))
		{
			// Handles movement input
			UpdateMovement(elapsedSec, isOnFloor, isAgainstLeftWall, isAgainstRightWall, soundManager);

			// Handles action input
			UpdateAction(elapsedSec, soundManager);
		}
		

		// Movement state update
		MovementSwitch(elapsedSec);
	}

	// Action state update
	ActionSwitch(isOnFloor);

	// Sprite selector
	SpriteSelect();

	// Handles animation
	UpdateAnimation(elapsedSec);

	// Handles hurt-state
	UpdateHurt(elapsedSec);

	// Handles Arrow
	UpdateArrow(elapsedSec, level);
}

Rectf Avatar::GetShape() const
{
	return m_Shape;
}
Rectf Avatar::GetCollisionRect() const
{
	return m_CollisionRect;
}

void Avatar::SetHurt(SoundManager& soundManager, HUDManager& hudManager)
{
	// If hurt, but still invinc or if dying or dead, return
	if ((m_HurtSec < m_InvincibilityTime && m_IsHurt) || m_MoveState == mState::dying || m_ActionState == aState::dead)
	{
		return;
	}

	m_IsHurt = true;
	m_HurtSec = 0;
	m_Health -= 1;

	soundManager.PlayEffect(SoundManager::Effect::Hurt);
	hudManager.SetPlayerHealth(m_Health);
}

Rectf Avatar::GetArrow() const
{
	return m_pArrow->GetShape();
}
void Avatar::DeactivateArrow()
{
	m_IsShot = false;
	m_ArrowSec = 0;

	m_pArrow->SetArrowOff();
}
bool Avatar::GetShot() const
{
	return m_IsShot;
}

void Avatar::SetPoint(const Point2f& position)
{
	m_Shape.left = position.x;
	m_Shape.bottom = position.y;
}

void Avatar::SetLookToLeft(bool lookLeft)
{
	m_IsLookingLeft = lookLeft;
}
bool Avatar::GetLookToLeft() const
{
	return m_IsLookingLeft;
}

void Avatar::SetPause()
{
	const float waitSec{ 0.5f };

	m_TeleportPausing = true;
	m_TeleportPauseSec = waitSec;

	m_MoveState = mState::waiting;
	m_ActionState = aState::idle;
}
void Avatar::SetIsTeleported(bool isTeleported)
{
	m_IsTeleported = isTeleported;
}

Avatar::aState Avatar::GetActionState() const
{
	return m_ActionState;
}

int Avatar::GetHealth() const
{
	return m_Health;
}
void Avatar::SetHealth(int newHealth)
{
	m_Health = newHealth;
}

int Avatar::GetScore() const
{
	return m_Score;
}
void Avatar::SetScore(int newScore, HUDManager& hudManager)
{
	m_Score = newScore;
	hudManager.SetPlayerScore(newScore);
}

// HELPER FUNCTIONS

void Avatar::UpdateMovement(float elapsedSec, bool isOnFloor, bool isAgainstLeftWall,
	bool isAgainstRightWall, SoundManager& soundManager)
{
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);

	// If falling, put in Jump state
	if (m_MoveState == mState::moving && !isOnFloor)
	{
		m_MoveState = mState::jumping;
	}

	// Can move and jump while on ground
	if (isOnFloor)
	{
		// When not pressing buttons, idle
		if (!pStates[SDL_SCANCODE_A] || !pStates[SDL_SCANCODE_D] || !pStates[SDL_SCANCODE_SPACE])
		{
			m_MoveState = mState::waiting;
		}

		// If not ducking or looking up --> moveable
		if (m_ActionState != aState::ducking && m_ActionState != aState::lookingUp)
		{
			if (pStates[SDL_SCANCODE_A] && !isAgainstLeftWall)
			{
				m_Velocity.x = -m_XSpeed;
				m_IsLookingLeft = true;
				m_MoveState = mState::moving;
			}

			if (pStates[SDL_SCANCODE_D] && !isAgainstRightWall)
			{
				m_Velocity.x = m_XSpeed;
				m_IsLookingLeft = false;
				m_MoveState = mState::moving;
			}
		}

		
	}

	// If space pressed and jumped already or still jumping
	if (pStates[SDL_SCANCODE_SPACE] && (!m_DidOnce || (m_MoveState == mState::jumping && m_StillJumping )))
	{
		m_StillJumping = true;
	}
	else 
	{
		m_StillJumping = false;
	}

	// If not ducking --> can jump
	if (m_ActionState != aState::ducking)
	{
		// If max height not reached yet, can still jump higher
		bool jumpable{ m_JumpTime < m_MaxJumpHeight };
		if (pStates[SDL_SCANCODE_SPACE] && jumpable && m_StillJumping)
		{
			m_Velocity.y = m_YSpeed;

			// Start jump
			if (!m_DidOnce)
			{
				m_DidOnce = true;
				soundManager.PlayEffect(SoundManager::Effect::Jump);
			}

			m_JumpTime += elapsedSec;
			m_MoveState = mState::jumping;
		}
	}

	// If in air, still controllable
	if (m_MoveState == mState::jumping)
	{
		if (!pStates[SDL_SCANCODE_A] || !pStates[SDL_SCANCODE_D])
		{
			m_Velocity.x = 0;
		}

		if (pStates[SDL_SCANCODE_A] && !isAgainstLeftWall)
		{
			m_Velocity.x = -m_XSpeed;
			m_IsLookingLeft = true;
		}
		if (pStates[SDL_SCANCODE_D] && !isAgainstRightWall)
		{
			m_Velocity.x = m_XSpeed;
			m_IsLookingLeft = false;
		}
	}
}
void Avatar::UpdateAction(float elapsedSec, SoundManager& soundManager)
{
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);

	// If on shootCooldown, let timer run
	if (m_ShootCooldown)
	{
		m_ShootCooldownSec += elapsedSec;
		if (m_ShootCooldownSec >= 0.5f)
		{
			m_ShootCooldown = false;
			m_ShootCooldownSec = 0;
		}
	}

	// If not ducking or on cooldown, shoot
	bool isDucking{ m_ActionState == aState::ducking };
	if (!(isDucking || m_IsShot) && pStates[SDL_SCANCODE_RETURN] && !m_ShootCooldown)
	{
		soundManager.PlayEffect(SoundManager::Effect::Shoot);
		m_IsShot = true;
		m_ShootCooldown = true;
	}

	// Ducking
	if (pStates[SDL_SCANCODE_S])
	{
		m_ActionState = aState::ducking;
	}
	// Looking Up
	else if (pStates[SDL_SCANCODE_W])
	{
		m_ActionState = aState::lookingUp;
	}
	else
	{
		m_ActionState = aState::idle;
	}
}

void Avatar::MovementSwitch(float elapsedSec)
{
	switch (m_MoveState)
	{
	case Avatar::mState::waiting:
		m_Velocity.x = 0;

		break;
	case Avatar::mState::jumping:
	case Avatar::mState::moving:
		m_Shape.left += m_Velocity.x * elapsedSec;

		break;
	case Avatar::mState::dying:
		
		// Time before falling
		float maxTime{ 1 };
		if (m_DyingSec < maxTime)
		{
			m_Velocity.y = 0;

			return;
		}

		break;
	}

	m_Shape.bottom += m_Velocity.y * elapsedSec;
}
void Avatar::ActionSwitch(bool isOnFloor)
{
	switch (m_ActionState)
	{
	case Avatar::aState::idle:
		break;

	case Avatar::aState::ducking:
	case Avatar::aState::lookingUp:
		// While on floor, can't move
		if (isOnFloor)
		{
			m_Velocity.x = 0;
		}

		break;
	}

	// If Shot pressed and arrow not on screen, shoot
	if (m_IsShot)
	{
		if (m_ArrowSec <= 0)
		{
			ShootArrow();
		}
	}
}

void Avatar::UpdateAnimation(float elapsedSec)
{
	m_AccumSec += elapsedSec;
	const float frameTime{ 1.f / m_NrFramesPerSec };

	if (m_AccumSec >= frameTime)
	{
		++m_CurrFrame %= (m_Cols * m_Rows);
		m_AccumSec -= frameTime;

	}

	m_SrcRect.left = m_SheetLeft + m_CurrFrame % m_Cols * m_SrcRect.width;
	m_SrcRect.bottom = m_SheetTop + (m_CurrFrame / m_Cols + 1) * m_SrcRect.height;
}

void Avatar::SpriteSelect()
{
	const float mainWidth{ 43 }, mainHeight{ 65 };

	// Sprites when dying
	if (m_MoveState == mState::dying || m_ActionState == aState::dead)
	{
		m_SheetLeft = 287;
		m_SheetTop = 68;
		m_SheetWidth = 40;
		m_SheetHeight = 66;

		m_Rows = 1;
		m_Cols = 1;

		m_Shape.width = m_SheetWidth / m_Cols;
		m_Shape.height = m_SheetHeight / m_Rows;

		m_SrcRect.width = m_SheetWidth / m_Cols;
		m_SrcRect.height = m_SheetHeight / m_Rows;

		return;
	}

	// Sprites when shooting
	if (m_IsShot)
	{
		switch (m_ActionState)
		{
		case Avatar::aState::idle:
			switch (m_MoveState)
			{
			case Avatar::mState::waiting:
				m_SheetLeft = 239;
				m_SheetTop = 67;
				m_SheetWidth = 44;
				m_SheetHeight = mainHeight;

				m_Rows = 1;
				m_Cols = 1;

				break;
			case Avatar::mState::jumping:
				if (m_Velocity.y >= 0)
				{
					m_SheetLeft = 369;
					m_SheetTop = 135;
					m_SheetWidth = mainWidth;
					m_SheetHeight = mainHeight;
				}
				else
				{
					m_SheetLeft = 418;
					m_SheetTop = 135;
					m_SheetWidth = mainWidth;
					m_SheetHeight = mainHeight;
				}

				m_Rows = 1;
				m_Cols = 1;

				break;
			case Avatar::mState::moving:
				m_SheetLeft = 103;
				m_SheetTop = 67;
				m_SheetWidth = 131;
				m_SheetHeight = mainHeight;

				m_Rows = 1;
				m_Cols = 3;
				break;
			}

			break;
		case Avatar::aState::ducking:
			m_SheetLeft = 424;
			m_SheetTop = 67;
			m_SheetWidth = 41;
			m_SheetHeight = 43;

			m_Rows = 1;
			m_Cols = 1;

			break;
		case Avatar::aState::lookingUp:
			if (m_MoveState == mState::jumping)
			{
				m_SheetLeft = 271;
				m_SheetTop = 135;
				m_SheetWidth = 44;
				m_SheetHeight = mainHeight;
			}
			else
			{
				m_SheetLeft = 320;
				m_SheetTop = 135;
				m_SheetWidth = mainWidth;
				m_SheetHeight = mainHeight;
			}

			m_Rows = 1;
			m_Cols = 1;

			break;

			m_Shape.width = m_SheetWidth / m_Cols;
			m_Shape.height = m_SheetHeight / m_Rows;

			m_SrcRect.width = m_SheetWidth / m_Cols;
			m_SrcRect.height = m_SheetHeight / m_Rows;
		}

	}
	else
	{
		// Sprites if not shooting
		switch (m_ActionState)
		{
		case Avatar::aState::idle:
			switch (m_MoveState)
			{
			case Avatar::mState::waiting:
				m_SheetLeft = 423;
				m_SheetTop = 0;
				m_SheetWidth = mainWidth;
				m_SheetHeight = mainHeight;

				m_Rows = 1;
				m_Cols = 1;

				break;
			case Avatar::mState::jumping:
				if (m_Velocity.y >= 0)
				{
					m_SheetLeft = 185;
					m_SheetTop = 1;
					m_SheetWidth = mainWidth;
					m_SheetHeight = mainHeight;
				}
				else
				{
					m_SheetLeft = 234;
					m_SheetTop = 1;
					m_SheetWidth = mainWidth;
					m_SheetHeight = mainHeight;
				}

				m_Rows = 1;
				m_Cols = 1;

				break;
			case Avatar::mState::moving:
				m_SheetLeft = 287;
				m_SheetTop = 0;
				m_SheetWidth = 132;
				m_SheetHeight = mainHeight;

				m_Rows = 1;
				m_Cols = 3;
				break;
			}

			break;
		case Avatar::aState::ducking:
			m_SheetLeft = 424;
			m_SheetTop = 67;
			m_SheetWidth = 41;
			m_SheetHeight = 43;

			m_Rows = 1;
			m_Cols = 1;

			break;
		case Avatar::aState::lookingUp:
			if (m_MoveState == mState::jumping)
			{
				m_SheetLeft = 89;
				m_SheetTop = 2;
				m_SheetWidth = mainWidth;
				m_SheetHeight = mainHeight;
			}
			else
			{
				m_SheetLeft = 138;
				m_SheetTop = 2;
				m_SheetWidth = mainWidth;
				m_SheetHeight = mainHeight;
			}

			m_Rows = 1;
			m_Cols = 1;

			break;
		}

		m_Shape.width = m_SheetWidth / m_Cols;
		m_Shape.height = m_SheetHeight / m_Rows;

		m_SrcRect.width = m_SheetWidth / m_Cols;
		m_SrcRect.height = m_SheetHeight / m_Rows;
	}

	// Cycles through palletes
	if (m_IsHurt)
	{
		float sheetTopAdd{};

		switch (m_HurtFrame)
		{
		case 0:
			m_SheetTop += sheetTopAdd;

			break;

		case 1:
			sheetTopAdd = 206;
			m_SheetTop += sheetTopAdd;

			break;
		case 2:
			sheetTopAdd = 412;
			m_SheetTop += sheetTopAdd;

			break;
		}
	}
}

void Avatar::UpdateHurt(float elapsedSec)
{
	if (!m_IsHurt)
	{
		return;
	}

	const int nrColors{ 3 };
	const float frameTime{ 1.f / m_HurtpSec };

	// Cycles through colors
	m_HurtAccumSec += elapsedSec;
	if (m_HurtSec >= frameTime)
	{
		++m_HurtFrame %= nrColors;
		m_HurtAccumSec -= frameTime;
	}

	// Check if done	
	m_HurtSec += elapsedSec;
	if (m_HurtSec >= m_InvincibilityTime)
	{
		m_HurtSec = 0;
		m_IsHurt = false;
	}
}

void Avatar::ShootArrow()
{
	Arrow::Direction direction{};
	Point2f startPos{};

	// If looking up, shoot up
	if (m_ActionState == aState::lookingUp)
	{
		direction = Arrow::Direction::up;

		startPos.x = m_Shape.left + m_Shape.width / 2;
		startPos.y = m_Shape.bottom + m_Shape.height;

		m_StartPos = startPos;
		m_pArrow->SetArrowOn(startPos, direction);
		
		return;
	}

	// Changes direction when looking left or right
	if (m_IsLookingLeft)
	{
		direction = Arrow::Direction::left;
		
		startPos.x = m_Shape.left;
		startPos.y = m_Shape.bottom + m_Shape.height / 2;
	}
	else
	{
		direction = Arrow::Direction::right;

		startPos.x = m_Shape.left + m_Shape.width;
		startPos.y = m_Shape.bottom + m_Shape.height / 2;
	}

	m_StartPos = startPos;
	m_pArrow->SetArrowOn(startPos, direction);
}
void Avatar::UpdateArrow(float elapsedSec, const Level& level)
{
	if (!m_IsShot)
	{
		return;
	}

	// If hit a wall, deactivate
	const bool hitHorizontal{ level.IsOnGround(m_pArrow->GetShape()) };
	const bool hitLeft{ level.IsAgainstLeftWall(m_pArrow->GetShape()) };
	const bool hitRight{ level.IsAgainstRightWall(m_pArrow->GetShape()) };
	if (hitHorizontal || hitLeft || hitRight)
	{
		DeactivateArrow();
		return;
	}

	m_ArrowSec += elapsedSec;

	const Arrow::Direction direction{ m_pArrow->GetDirection() };
	const Rectf arrowShape{ m_pArrow->GetShape() };

	const float distAway{ 100 };
	bool isFar{};

	// checks when to despawn arrow
	switch (direction)
	{
	case Arrow::Direction::left:
		isFar = arrowShape.left + arrowShape.width < m_StartPos.x - distAway;
		if (isFar)
		{
			m_IsShot = false;
			m_ArrowSec = 0;

			m_pArrow->SetArrowOff();
			return;
		}

		break;
	case Arrow::Direction::right:
		isFar = arrowShape.left > m_StartPos.x + distAway;
		if (isFar)
		{
			m_IsShot = false;
			m_ArrowSec = 0;

			m_pArrow->SetArrowOff();
			return;
		}

		break;
	case Arrow::Direction::up:
		isFar = arrowShape.bottom > m_StartPos.y + distAway;
		if (isFar)
		{
			m_IsShot = false;
			m_ArrowSec = 0;

			m_pArrow->SetArrowOff();
			return;
		}

		break;
	}

	m_pArrow->Update(elapsedSec);
}