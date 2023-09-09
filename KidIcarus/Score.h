#pragma once
#include <vector>

class Texture;

class Score final
{
public:
	explicit Score(const Rectf& windowRect, const Texture*& texture);
	~Score() = default;
	Score(const Score& other) = delete;
	Score& operator= (const Score& rhs) = delete;
	Score(Score&& other) = delete;
	Score& operator= (Score&& other) = delete;

	void Draw() const;
	void SetScore(int scoreSetter);

private:
	// Variables
	const Rectf m_Window;
	std::vector<int> m_DigitArr;

	const Texture* m_pTexture;
	const float m_TextureHeight, m_TextureWidth;
	Rectf m_SrcRect;
};

