#include "pch.h"
#include "Arrow.h"
#include "Texture.h"
#include "Level.h"

Arrow::Arrow(const std::string& texturePath, float flyingSpeed)
	: m_pTexture{new Texture{texturePath}}
	, m_Speed{flyingSpeed}
	, m_Direction{}
	, m_Shape{}
	, m_IsActive{ false }
{
	
}

Arrow::~Arrow()
{
	delete m_pTexture;
	m_pTexture = nullptr;
}

void Arrow::Draw() const
{
	if (!m_IsActive)
	{
		return;
	}

	glPushMatrix();

	// Draws Arrow
	if (m_Direction == Direction::left)
	{
		glTranslatef(m_Shape.left + m_Shape.width / 2, m_Shape.bottom, 0);
		glScalef(-1, 1, 1);
		glTranslatef(-m_Shape.left - m_Shape.width / 2, -m_Shape.bottom, 0);
	}

	m_pTexture->Draw(m_Shape, m_SrcRect);

	glPopMatrix();
}

void Arrow::Update(float elapsedSec)
{
	if (!m_IsActive)
	{
		return;
	}

	// Direction decides how to apply speed
	switch (m_Direction)
	{
	case Arrow::Direction::left:
		m_Shape.left -= m_Speed * elapsedSec;

		break;
	case Arrow::Direction::right:
		m_Shape.left += m_Speed * elapsedSec;

		break;
	case Arrow::Direction::up:
		m_Shape.bottom += m_Speed* elapsedSec;

		break;
	}
}

void Arrow::SetArrowOn(const Point2f& startPos, Direction direction)
{
	m_IsActive = true;
	m_Direction = direction;

	// Adjusts shapes according to direction
	float width{ 22 }, height{ 21 };

	switch (direction)
	{
	case Arrow::Direction::left:
		m_SrcRect.width = width;
		m_SrcRect.height = height;
		m_SrcRect.left = 243;
		m_SrcRect.bottom = 141 + m_SrcRect.height;

		m_Shape.width = width;
		m_Shape.height = height;
		m_Shape.left = startPos.x - m_Shape.width;
		m_Shape.bottom = startPos.y - m_Shape.height / 2;

		break;

	case Arrow::Direction::right:
		m_SrcRect.width = width;
		m_SrcRect.height = height;
		m_SrcRect.left = 243;
		m_SrcRect.bottom = 141 + m_SrcRect.height;

		m_Shape.width = width;
		m_Shape.height = height;
		m_Shape.left = startPos.x;
		m_Shape.bottom = startPos.y - m_Shape.height / 2;

		break;
	case Arrow::Direction::up:
		width = 21;
		height = 21;

		m_SrcRect.width = width;
		m_SrcRect.height = height;
		m_SrcRect.left = 214;
		m_SrcRect.bottom = 141 + m_SrcRect.height;

		m_Shape.width = width;
		m_Shape.height = height;
		m_Shape.left = startPos.x - m_Shape.width / 2;
		m_Shape.bottom = startPos.y;

		break;
	}
}

void Arrow::SetArrowOff()
{
	m_IsActive = false;
}

Rectf Arrow::GetShape() const
{
	if (!m_IsActive)
	{
		return Rectf{ 0,0,0,0 };
	}

	return m_Shape;
}

Arrow::Direction Arrow::GetDirection() const
{
	return m_Direction;
}