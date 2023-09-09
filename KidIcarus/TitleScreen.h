#pragma once
class Texture;
class SoundManager;

class TitleScreen final
{
public:
	explicit TitleScreen(const Rectf& windowRect);
	~TitleScreen();
	TitleScreen(const TitleScreen& other) = delete;
	TitleScreen& operator= (const TitleScreen& rhs) = delete;
	TitleScreen(TitleScreen&& other) = delete;
	TitleScreen& operator= (TitleScreen&& rhs) = delete;

	void Draw() const;
	void Update(SoundManager& soundManager);

	void SetInTitleScreen(bool setInTitleScreen);
	bool GetInTitleScreen() const;

	void SetLoadFile(bool setLoadFile);
	bool GetLoadFile() const;

private:
	const Rectf m_Window;
	Point2f m_CursorPos;
	const Point2f m_TopPos;
	const Point2f m_BotPos;

	bool m_InTitleScreen;
	bool m_HasStarted;
	bool m_TopSelected;
	bool m_LoadFile;

	bool m_HasPressed;
	bool m_HasToggled;

	const Texture* m_pTitleScreen;
	const Texture* m_pSelectScreen;
	const Texture* m_pCursorTexture;

	//Functions
	void Pressed(SoundManager& soundManager);
	void Toggle(SoundManager& soundManager);
};

