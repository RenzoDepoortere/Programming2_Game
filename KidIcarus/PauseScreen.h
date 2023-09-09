#pragma once
#include <unordered_map>

class Texture;
class SoundManager;

class PauseScreen final
{
public:
	enum class Selected
	{
		top, middle, bottom
	};

	explicit PauseScreen(const Rectf& windowRect, const Texture* cursorTexture, int soundVolume);
	~PauseScreen();
	PauseScreen(const PauseScreen& other) = delete;
	PauseScreen& operator= (const PauseScreen& rhs) = delete;
	PauseScreen(PauseScreen&& other) = delete;
	PauseScreen& operator= (PauseScreen&& rhs) = delete;

	void Draw() const;
	void Update(SoundManager& soundManager);

	bool GetPaused() const;

	bool GetShouldSave();
	void SetShouldSave(bool setShouldSave);

	Selected GetSelected() const;

private:
	const Rectf m_Window;
	
	Selected m_CurrentSelected;
	std::unordered_map<Selected, Point2f> m_ButtonsMap;

	bool m_IsPaused;
	bool m_ShouldSave;

	bool m_TopSelected;
	bool m_SettingsPicked;

	int m_SoundVolume;
	int m_MaxVolume;

	bool m_EscapePressed;
	bool m_UpPressed;
	bool m_DownPressed;
	bool m_EnterPressed;
	bool m_LeftPressed;
	bool m_RightPressed;

	Point2f m_CursorPos;

	// SettingsPos
	const float m_SoundYPos;
	const float m_SoundMaxXPos;
	const float m_SoundXJump;

	const Point2f m_SoundResumePos;

	Point2f m_SelectorPos;

	const Texture* m_pScreenTexture;
	const Texture* m_pCursorTexture;
	const Texture* m_pSettingsTexture;
	const Texture* m_pSelectorTexture;

	// Functions
	void EnableScreen(SoundManager& soundManager);
	void EnterPressed(SoundManager& soundManager);

	void ShiftUp(SoundManager& soundManager);
	void ShiftDown(SoundManager& soundManager);
	void ShiftLeft(SoundManager& soundManager);
	void ShiftRight(SoundManager& soundManager);

	void SettingsToggle();
};

