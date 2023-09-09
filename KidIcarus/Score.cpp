#include "pch.h"
#include "Score.h"
#include "Texture.h"

#include <iostream>

Score::Score(const Rectf& windowRect, const Texture*& texture)
	: m_Window{windowRect}
	, m_pTexture{texture}
	, m_TextureHeight{27}
	, m_TextureWidth{290}
{
	const float sheetLeft{}, sheetTop{ 34 }, srcWidth{ 29 };

	m_SrcRect.width = srcWidth;
	m_SrcRect.height = m_TextureHeight;
	m_SrcRect.left = sheetLeft;
	m_SrcRect.bottom = sheetTop + m_SrcRect.height;

	// Init arr of possible amount of digits
	const int nrDigits{ 3 };
	for (int idx{}; idx < nrDigits; idx++)
	{
		m_DigitArr.push_back(0);
	}
}

void Score::Draw() const
{
	float buffer{};

	// Draw Heart
	Point2f drawPoint { m_Window.left + 25.f,
							(m_Window.bottom + m_Window.height) - m_SrcRect.height - 10.f };

	buffer = 65.f;
	Rectf srcRect{ m_SrcRect };
	srcRect.bottom = buffer + srcRect.height;

	m_pTexture->Draw(drawPoint, srcRect);

	
	// Draw Numbers
	srcRect = m_SrcRect;

	buffer = 10.f;
	drawPoint.x += buffer + srcRect.width;

	for (size_t idx{}; idx < m_DigitArr.size(); idx++)
	{
		srcRect.left = m_SrcRect.left + m_DigitArr[m_DigitArr.size() - idx - 1] * m_SrcRect.width;
		m_pTexture->Draw(drawPoint, srcRect);

		// Prepares drawPoint for next digit
		buffer = 5.f;
		drawPoint.x += buffer + srcRect.width;
	}
}

void Score::SetScore(int scoreSetter)
{
	// Split Digits up
	int showDigit{}, score{ scoreSetter };
	for (size_t idx{}; idx < m_DigitArr.size(); idx++)
	{
		showDigit = score % 10;
		m_DigitArr[idx] = showDigit;

		score = (score - showDigit) / 10;
	}
}