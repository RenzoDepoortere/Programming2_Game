#include "pch.h"
#include "PauseScreen.h"
#include "Texture.h"
#include "SoundManager.h"

PauseScreen::PauseScreen(const Rectf& windowRect, const Texture* cursorTexture, int soundVolume)
	: m_Window{windowRect}
	, m_CurrentSelected{Selected::top}
	, m_IsPaused{false}
	, m_ShouldSave{false}
	, m_TopSelected{true}
	, m_SettingsPicked{false}
	, m_SoundVolume{soundVolume}
	, m_MaxVolume{9}
	, m_EscapePressed{false}
	, m_UpPressed{false}
	, m_DownPressed{false}
	, m_EnterPressed{false}
	, m_LeftPressed{false}
	, m_RightPressed{false}
	, m_SoundYPos{215.f}
	, m_SoundMaxXPos{ m_Window.width - 215.f }
	, m_SoundXJump{50.f}
	, m_SoundResumePos{m_Window.width - 220.f, 15.f}
	, m_pScreenTexture{new Texture{"Resources/HUD/PauseScreen.png"}}
	, m_pCursorTexture{cursorTexture}
	, m_pSettingsTexture{new Texture{"Resources/HUD/Settings.png"}}
	, m_pSelectorTexture{new Texture{"Resources/HUD/Selector.png"}}
{
	m_ButtonsMap.emplace(Selected::top, Point2f{ 200,310 });
	m_ButtonsMap.emplace(Selected::middle, Point2f{ 200,205 });
	m_ButtonsMap.emplace(Selected::bottom, Point2f{ 200,105 });

	m_CursorPos = m_ButtonsMap[m_CurrentSelected];

	m_SelectorPos.x = m_SoundMaxXPos - (m_MaxVolume - m_SoundVolume) * m_SoundXJump + 2.5f;
	m_SelectorPos.y = m_SoundYPos;
}

PauseScreen::~PauseScreen()
{
	delete m_pScreenTexture;
	m_pScreenTexture = nullptr;

	delete m_pSettingsTexture;
	m_pSettingsTexture = nullptr;

	delete m_pSelectorTexture;
	m_pSelectorTexture = nullptr;
}

void PauseScreen::Draw() const
{
	// Draw if paused
	if (m_IsPaused)
	{
		m_pScreenTexture->Draw(m_Window);

		// Draw Settings if picked
		if (m_SettingsPicked)
		{
			m_pSettingsTexture->Draw(m_Window);

			m_pSelectorTexture->Draw(m_SelectorPos);
		}

		m_pCursorTexture->Draw(m_CursorPos);
	}
}

void PauseScreen::Update(SoundManager& soundManager)
{
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);

	// Press ESCAPE
	if (pStates[SDL_SCANCODE_ESCAPE])
	{
		EnableScreen(soundManager);
	}
	else
	{
		m_EscapePressed = false;
	}

	if (!m_IsPaused)
	{
		return;
	}

	// Press UP
	if (pStates[SDL_SCANCODE_W] || pStates[SDL_SCANCODE_UP])
	{
		ShiftUp(soundManager);
	}
	else
	{
		m_UpPressed = false;
	}

	// Press DOWN
	if (pStates[SDL_SCANCODE_S] || pStates[SDL_SCANCODE_DOWN])
	{
		ShiftDown(soundManager);
	}
	else
	{
		m_DownPressed = false;
	}

	// Press ENTER
	if (pStates[SDL_SCANCODE_RETURN])
	{
		EnterPressed(soundManager);
	}
	else
	{
		m_EnterPressed = false;
	}

	// Press LEFT
	if (pStates[SDL_SCANCODE_LEFT] || pStates[SDL_SCANCODE_A])
	{
		ShiftLeft(soundManager);
	}
	else
	{
		m_LeftPressed = false;
	}

	// Press RIGHT
	if (pStates[SDL_SCANCODE_RIGHT] || pStates[SDL_SCANCODE_D])
	{
		ShiftRight(soundManager);
	}
	else
	{
		m_RightPressed = false;
	}
}

bool PauseScreen::GetPaused() const
{
	return m_IsPaused;
}

bool PauseScreen::GetShouldSave()
{
	return m_ShouldSave;
}
void PauseScreen::SetShouldSave(bool setShouldSave)
{
	m_ShouldSave = setShouldSave;
}

PauseScreen::Selected PauseScreen::GetSelected() const
{
	return m_CurrentSelected;
}

void PauseScreen::EnableScreen(SoundManager& soundManager)
{
	if (m_EscapePressed)
	{
		return;
	}

	// Toggle between on and off when ESC pressed
	m_IsPaused = !m_IsPaused;

	// Sets Cursor back to top-pos
	if (m_IsPaused)
	{
		m_SettingsPicked = false;
		m_CurrentSelected = Selected::top;

		m_CursorPos = m_ButtonsMap[m_CurrentSelected];

		soundManager.PlayEffect(SoundManager::Effect::Pause);
	}

	m_EscapePressed = true;
}
void PauseScreen::EnterPressed(SoundManager& soundManager)
{
	if (m_EnterPressed)
	{
		return;
	}

	// When in SETTINGS
	if (m_SettingsPicked)
	{
		// Change Sound-Volume
		if (m_TopSelected)
		{
			m_SelectorPos.x = m_CursorPos.x + 2.5f;

			soundManager.SetVolume(m_SoundVolume);
		}
		// Resume to previous screen
		else
		{
			m_CurrentSelected = Selected::top;
			m_SettingsPicked = false;

			m_CursorPos = m_ButtonsMap[m_CurrentSelected];
		}

		soundManager.PlayEffect(SoundManager::Effect::Select);
		m_EnterPressed = true;
		return;
	}


	switch (m_CurrentSelected)
	{
		// If RESUME selected, close screen
	case PauseScreen::Selected::top:
		m_IsPaused = false;

		m_CursorPos = m_ButtonsMap[m_CurrentSelected];

		soundManager.PlayEffect(SoundManager::Effect::Select);

		break;
	case PauseScreen::Selected::middle:
		m_SettingsPicked = true;
		m_TopSelected = true;

		m_CursorPos.x = m_SoundMaxXPos - (m_MaxVolume - m_SoundVolume) * m_SoundXJump;
		m_CursorPos.y = m_SoundYPos;

		soundManager.PlayEffect(SoundManager::Effect::Select);

		break;
	case PauseScreen::Selected::bottom:
		m_ShouldSave = true;
		
		soundManager.PlayEffect(SoundManager::Effect::Saved);

		break;
	}

	m_EnterPressed = true;
}

void PauseScreen::ShiftUp(SoundManager& soundManager)
{
	if (m_UpPressed)
	{
		return;
	}

	// If settings picked
	if (m_SettingsPicked)
	{
		SettingsToggle();

		soundManager.PlayEffect(SoundManager::Effect::Select);
		m_UpPressed = true;
		return;
	}

	// Switch between options
	switch (m_CurrentSelected)
	{
	case PauseScreen::Selected::top:
		m_CurrentSelected = Selected::bottom;

		break;
	case PauseScreen::Selected::middle:
		m_CurrentSelected = Selected::top;

		break;
	case PauseScreen::Selected::bottom:
		m_CurrentSelected = Selected::middle;

		break;
	}

	soundManager.PlayEffect(SoundManager::Effect::Select);
	m_CursorPos = m_ButtonsMap[m_CurrentSelected];
	m_UpPressed = true;
}
void PauseScreen::ShiftDown(SoundManager& soundManager)
{
	if (m_DownPressed)
	{
		return;
	}

	// If settings picked, toggle between sound-settings and resume button
	if (m_SettingsPicked)
	{
		SettingsToggle();

		soundManager.PlayEffect(SoundManager::Effect::Select);
		m_DownPressed = true;
		return;
	}


	// Switch between options
	switch (m_CurrentSelected)
	{
	case PauseScreen::Selected::top:
		m_CurrentSelected = Selected::middle;

		break;
	case PauseScreen::Selected::middle:
		m_CurrentSelected = Selected::bottom;

		break;
	case PauseScreen::Selected::bottom:
		m_CurrentSelected = Selected::top;

		break;
	}

	soundManager.PlayEffect(SoundManager::Effect::Select);
	m_CursorPos = m_ButtonsMap[m_CurrentSelected];
	m_DownPressed = true;
}
void PauseScreen::ShiftLeft(SoundManager& soundManager)
{
	if (m_LeftPressed)
	{
		return;
	}

	if (!m_SettingsPicked || (m_SettingsPicked && !m_TopSelected))
	{
		return;
	}

	// Lower Volume
	if (m_SoundVolume > 0)
	{
		--m_SoundVolume;
		m_CursorPos.x = m_SoundMaxXPos - (m_MaxVolume - m_SoundVolume) * m_SoundXJump;

		soundManager.PlayEffect(SoundManager::Effect::Select);
	}

	m_LeftPressed = true;
}
void PauseScreen::ShiftRight(SoundManager& soundManager)
{
	if (m_RightPressed)
	{
		return;
	}

	if (!m_SettingsPicked || (m_SettingsPicked && !m_TopSelected))
	{
		return;
	}

	// Higher Volume
	if (m_SoundVolume < m_MaxVolume)
	{
		++m_SoundVolume;
		m_CursorPos.x = m_SoundMaxXPos - (m_MaxVolume - m_SoundVolume) * m_SoundXJump;

		soundManager.PlayEffect(SoundManager::Effect::Select);
	}

	m_RightPressed = true;
}

void PauseScreen::SettingsToggle()
{
	//toggle between sound - settings and resume button

	if (m_TopSelected)
	{
		m_TopSelected = false;
		m_CursorPos = m_SoundResumePos;
	}
	else
	{
		m_TopSelected = true;

		m_CursorPos.x = m_SoundMaxXPos - (m_MaxVolume - m_SoundVolume) * m_SoundXJump;
		m_CursorPos.y = m_SoundYPos;
	}
}