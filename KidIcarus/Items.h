#pragma once
class Texture;

class Items final
{
public:
	enum class Size
	{
		small, half, big
	};

	explicit Items(const std::string& texturePath, const Size& size, const Point2f& startPos);
	~Items();
	Items(const Items& other) = delete;
	Items& operator= (const Items& rhs) = delete;
	Items(Items&& other) = delete;
	Items& operator= (Items&& other) = delete;

	void Draw() const;
	void UpdateTime(float elapsedSec);

	Rectf GetShape() const;
	Size GetSize() const;
	float GetTime() const;

private:
	static Texture* m_pTexture;
	static int m_InstanceCntr;

	Size m_Size;
	Rectf m_Shape;
	Rectf m_SrcRect;

	float m_TimeAlive;
};

