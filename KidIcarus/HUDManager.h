#pragma once
#include "PauseScreen.h"

class Texture;
class HealthBar;
class Score;
class SoundManager;

class HUDManager final
{
public:
	explicit HUDManager(int currHealth, int maxHealth, const Rectf& windowRect, int soundVolume);
	~HUDManager();
	HUDManager(const HUDManager& other) = delete;
	HUDManager& operator= (const HUDManager& rhs) = delete;
	HUDManager(HUDManager&& other) = delete;
	HUDManager& operator= (HUDManager&& rhs) = delete;

	void Draw() const;
	void Update(SoundManager& soundManager);

	void SetPlayerDead(bool isDead);
	void SetPlayerHealth(int healthSetter);

	void SetPlayerScore(int newScore); 

	bool GetPaused() const;

	bool GetShouldSave();
	void SetShouldSave(bool setShouldSave);

	PauseScreen::Selected GetPauseScreenSelected() const;

private:
	const Rectf m_Window;
	bool m_PlayerDead;

	const Texture* m_pDeathScreenTexture;
	const Texture* m_pPlayerHud;
	const Texture* m_pCursorTexture;

	HealthBar* m_pHealthBar;
	Score* m_pScore;
	PauseScreen* m_pPauseScreen;
};

