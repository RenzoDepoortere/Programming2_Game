#pragma once
class Texture;

class HealthBar final
{
public:
	explicit HealthBar(int currHealth, int maxHealth, const Rectf& windowRect, const Texture*& texture);
	~HealthBar() = default;
	HealthBar(const HealthBar& other) = delete;
	HealthBar& operator= (const HealthBar& rhs) = delete;
	HealthBar(HealthBar&& other) = delete;
	HealthBar& operator= (HealthBar&& other) = delete;

	void Draw() const;
	void SetHealthState(int healthSetter);

private:
	const Rectf m_WindowRect;
	const int m_MaxHealth;
	int m_Health;

	const Texture* m_pTexture;
	const float m_TextureHeight, m_TextureWidth;
	Rectf m_SrcRect;

};

