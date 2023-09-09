#include "pch.h"
#include "Items.h"
#include "Texture.h"

#include <iostream>

Texture* Items::m_pTexture{ nullptr };
int Items::m_InstanceCntr{ 0 };

Items::Items(const std::string& texturePath, const Size& size, const Point2f& startPos)
	: m_Size{size}
	, m_Shape{}
	, m_SrcRect{}
	, m_TimeAlive{}
{
	// Create texture, if not done before
	if (m_InstanceCntr == 0)
	{
		m_pTexture = new Texture{ texturePath };
	}

	// Assign SrcRects
	const float smallSize{ 24 };
	const float halfHeight{ 48 }, halfWidth{ 24 };
	const float bigHeight{ halfHeight }, bigWidth{ 42 };

	float sheetLeft{}, sheetTop{ 3 };

	switch (m_Size)
	{
	case Items::Size::small:
		sheetLeft = 3;

		m_SrcRect.width = smallSize;
		m_SrcRect.height = smallSize;
		m_SrcRect.left = sheetLeft;
		m_SrcRect.bottom = sheetTop + m_SrcRect.height;

		m_Shape.width = smallSize;
		m_Shape.height = smallSize;

		break;
	case Items::Size::half:
		sheetLeft = 30;

		m_SrcRect.width = halfWidth;
		m_SrcRect.height = halfHeight;
		m_SrcRect.left = sheetLeft;
		m_SrcRect.bottom = sheetTop + m_SrcRect.height;

		m_Shape.width = halfWidth;
		m_Shape.height = halfHeight;

		break;
	case Items::Size::big:
		sheetLeft = 57;

		m_SrcRect.width = bigWidth;
		m_SrcRect.height = bigHeight;
		m_SrcRect.left = sheetLeft;
		m_SrcRect.bottom = sheetTop + m_SrcRect.height;

		m_Shape.width = bigWidth;
		m_Shape.height = bigHeight;

		break;
	}

	m_Shape.left = startPos.x - m_Shape.width / 2;
	m_Shape.bottom = startPos.y - m_Shape.height / 2;

	++m_InstanceCntr;
}

Items::~Items()
{
	if (m_InstanceCntr == 1)
	{
		delete m_pTexture;
		m_pTexture = nullptr;
	}

	--m_InstanceCntr;
}

void Items::Draw() const
{
	m_pTexture->Draw(m_Shape, m_SrcRect);
}

void Items::UpdateTime(float elapsedSec)
{
	m_TimeAlive += elapsedSec;
}

Rectf Items::GetShape() const
{
	return m_Shape;
}

Items::Size Items::GetSize() const
{
	return m_Size;
}

float Items::GetTime() const
{
	return m_TimeAlive;
}