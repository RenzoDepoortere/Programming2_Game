#include "pch.h"
#include "HealthBar.h"
#include "Texture.h"

HealthBar::HealthBar(int currHealth, int maxHealth, const Rectf& windowRect, const Texture*& texture)
	: m_WindowRect{windowRect}
	, m_MaxHealth{maxHealth}
	, m_Health{currHealth}
	, m_pTexture{ texture }
	, m_TextureHeight{27}
	, m_TextureWidth{232}
{
	const float sheetLeft{}, sheetTop{}, srcWidth{29};

	m_SrcRect.width = srcWidth;
	m_SrcRect.height = m_TextureHeight;
	m_SrcRect.left = m_SrcRect.width * (m_MaxHealth - currHealth);
	m_SrcRect.bottom = sheetTop + m_SrcRect.height;
}

void HealthBar::Draw() const
{
	const Point2f drawPoint{ m_WindowRect.left + 25.f,
							(m_WindowRect.bottom + m_WindowRect.height) - m_SrcRect.height - 50.f };

	m_pTexture->Draw(drawPoint, m_SrcRect);
}

void HealthBar::SetHealthState(int healthSetter)
{
	m_Health = healthSetter;
	m_SrcRect.left = m_SrcRect.width * (m_MaxHealth - healthSetter);
}