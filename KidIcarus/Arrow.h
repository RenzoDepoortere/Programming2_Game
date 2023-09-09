#pragma once

class Texture;
class Level;

class Arrow final
{
public:
	enum class Direction
	{
		left, right, up
	};
	
	explicit Arrow(const std::string& texturePath, float flyingSpeed);
	~Arrow();
	Arrow(const Arrow& other) = delete;
	Arrow& operator= (const Arrow& rhs) = delete;
	Arrow(Arrow&& other) = delete;
	Arrow& operator= (Arrow&& rhs) = delete;

	void Draw() const;
	void Update(float elapsedSec);
	
	void SetArrowOn(const Point2f& startPos, Direction direction);
	void SetArrowOff();

	Rectf GetShape() const;
	Direction GetDirection() const;

private:
	const Texture* m_pTexture;
	const float m_Speed;
	Direction m_Direction;
	Rectf m_Shape;
	Rectf m_SrcRect;
	bool m_IsActive;

};

