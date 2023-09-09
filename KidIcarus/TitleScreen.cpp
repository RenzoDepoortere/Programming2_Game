#include "pch.h"
#include "TitleScreen.h"
#include "Texture.h"
#include "SoundManager.h"

TitleScreen::TitleScreen(const Rectf& windowRect)
	: m_Window{windowRect}
	, m_TopPos{ 250,340 }
	, m_BotPos{250,220}
	, m_InTitleScreen{true}
	, m_HasStarted{false}
	, m_TopSelected{true}
	, m_LoadFile{false}
	, m_HasPressed{false}
	, m_HasToggled{false}
	, m_pTitleScreen{new Texture{"Resources/HUD/StartScreen.png"}}
	, m_pSelectScreen{new Texture{"Resources/HUD/SelectScreen.png"}}
	, m_pCursorTexture{ new Texture{"Resources/HUD/Arrow.png"} }
{
	m_CursorPos = m_TopPos;
}

TitleScreen::~TitleScreen()
{
	delete m_pTitleScreen;
	m_pTitleScreen = nullptr;

	delete m_pSelectScreen;
	m_pSelectScreen = nullptr;

	delete m_pCursorTexture;
	m_pCursorTexture = nullptr;
}

void TitleScreen::Draw() const
{
	if (m_InTitleScreen)
	{

		if (m_HasStarted)
		{
			m_pSelectScreen->Draw(m_Window);

			m_pCursorTexture->Draw(m_CursorPos);
		}
		else
		{
			m_pTitleScreen->Draw(m_Window);
		}
	}
}

void TitleScreen::Update(SoundManager& soundManager)
{
	if (!m_InTitleScreen)
	{
		return;
	}

	const Uint8* pStates = SDL_GetKeyboardState(nullptr);

	// Press Enter
	if (pStates[SDL_SCANCODE_RETURN])
	{
		Pressed(soundManager);
	}
	else
	{
		m_HasPressed = false;
	}

	// Return, when toggling has no use
	if (!m_HasStarted)
	{
		return;
	}

	// Toggle Betw options
	if (pStates[SDL_SCANCODE_W] || pStates[SDL_SCANCODE_UP] || pStates[SDL_SCANCODE_S] || pStates[SDL_SCANCODE_DOWN])
	{
		Toggle(soundManager);
	}
	else
	{
		m_HasToggled = false;
	}
}

void TitleScreen::SetInTitleScreen(bool setStarted)
{
	m_InTitleScreen = setStarted;
	m_HasStarted = false;
}
bool TitleScreen::GetInTitleScreen() const
{
	return m_InTitleScreen;
}

void TitleScreen::SetLoadFile(bool setLoadFile)
{
	m_LoadFile = setLoadFile;
}
bool TitleScreen::GetLoadFile() const
{
	return m_LoadFile;
}

void TitleScreen::Pressed(SoundManager& soundManager)
{
	if (m_HasPressed)
	{
		return;
	}

	if (m_HasStarted)
	{
		if (m_TopSelected)
		{
			m_LoadFile = false;
		}
		else
		{
			m_LoadFile = true;
		}

		m_InTitleScreen = false;
		m_HasStarted = false;
	}
	else
	{
		m_HasStarted = true;
	}

	soundManager.PlayEffect(SoundManager::Effect::Select);
	m_HasPressed = true;
}

void TitleScreen::Toggle(SoundManager& soundManager)
{
	if (m_HasToggled)
	{
		return;
	}

	if (m_TopSelected)
	{
		m_TopSelected = false;
		m_CursorPos = m_BotPos;
	}
	else
	{
		m_TopSelected = true;
		m_CursorPos = m_TopPos;
	}

	soundManager.PlayEffect(SoundManager::Effect::Select);
	m_HasToggled = true;
}
