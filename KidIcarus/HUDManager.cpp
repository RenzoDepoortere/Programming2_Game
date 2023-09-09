#include "pch.h"
#include "HUDManager.h"
#include "Texture.h"
#include "HealthBar.h"
#include "Score.h"
#include "TitleScreen.h"

HUDManager::HUDManager(int currHealth, int maxHealth, const Rectf& windowRect, int soundVolume)
	: m_Window{ windowRect }
	, m_PlayerDead{ false }
	, m_pDeathScreenTexture{ new Texture{"Resources/HUD/DeathScreen.png"} }
	, m_pPlayerHud{ new Texture{"Resources/HUD/PlayerHUD.png"} }
	, m_pCursorTexture{ new Texture{ "Resources/HUD/Arrow.png" } }
{
	m_pHealthBar = new HealthBar{currHealth, maxHealth, windowRect, m_pPlayerHud };
	m_pScore = new Score{ windowRect,m_pPlayerHud };

	m_pPauseScreen = new PauseScreen{ windowRect,m_pCursorTexture, soundVolume };
}

HUDManager::~HUDManager()
{
	delete m_pDeathScreenTexture;
	m_pDeathScreenTexture = nullptr;

	delete m_pHealthBar;
	m_pHealthBar = nullptr;

	delete m_pScore;
	m_pScore = nullptr;

	delete m_pPlayerHud;
	m_pPlayerHud = nullptr;

	delete m_pCursorTexture;
	m_pCursorTexture = nullptr;

	delete m_pPauseScreen;
	m_pPauseScreen = nullptr;
}

void HUDManager::Draw() const
{
	m_pHealthBar->Draw();

	m_pScore->Draw();

	m_pPauseScreen->Draw();

	if (m_PlayerDead)
	{
		m_pDeathScreenTexture->Draw(m_Window);
	}	
}
void HUDManager::Update(SoundManager& soundManager)
{
	m_pPauseScreen->Update(soundManager);
}

void HUDManager::SetPlayerDead(bool isDead)
{
	m_PlayerDead = isDead;
}
void HUDManager::SetPlayerHealth(int healthSetter)
{
	m_pHealthBar->SetHealthState(healthSetter);
}

void HUDManager::SetPlayerScore(int newScore)
{
	m_pScore->SetScore(newScore);
}

bool HUDManager::GetPaused() const
{
	return m_pPauseScreen->GetPaused();
}

bool HUDManager::GetShouldSave()
{
	return m_pPauseScreen->GetShouldSave();
}
void HUDManager::SetShouldSave(bool setShouldSave)
{
	m_pPauseScreen->SetShouldSave(setShouldSave);
}

PauseScreen::Selected HUDManager::GetPauseScreenSelected() const
{
	return m_pPauseScreen->GetSelected();
}