#include "pch.h"
#include "SoundManager.h"
#include "SoundStream.h"
#include "SoundEffect.h"

SoundManager::SoundManager(int soundVolume)
	: m_PickupMaxVolume{50}
	, m_EnemyHitMaxVolume{75}
	, m_CurrentVolume{soundVolume}
	, m_CurrentTheme{Theme::Title}
{
	m_pMusicMap.emplace(Theme::Level, new SoundStream{ "Resources/Sounds/The Underworld.mp3" });
	m_pMusicMap.emplace(Theme::Dungeon, new SoundStream{ "Resources/Sounds/Dungeon.mp3" });
	m_pMusicMap.emplace(Theme::Death, new SoundStream{ "Resources/Sounds/Game Over.mp3" });
	m_pMusicMap.emplace(Theme::Title, new SoundStream{ "Resources/Sounds/Title BGM.mp3" });

	m_pEffectMap.emplace(Effect::Shoot, new SoundEffect{ "Resources/Sounds/Shoot.wav" });
	m_pEffectMap.emplace(Effect::Jump, new SoundEffect{ "Resources/Sounds/Jump.wav" });
	m_pEffectMap.emplace(Effect::EnemyHit, new SoundEffect{ "Resources/Sounds/EnemyHit.mp3" });
	m_pEffectMap.emplace(Effect::PickUp, new SoundEffect{ "Resources/Sounds/PickUp.wav" });
	m_pEffectMap.emplace(Effect::Hurt, new SoundEffect{ "Resources/Sounds/Hurt.wav" });
	m_pEffectMap.emplace(Effect::Pause, new SoundEffect{ "Resources/Sounds/Pause.wav" });
	m_pEffectMap.emplace(Effect::Select, new SoundEffect{ "Resources/Sounds/Select.wav" });
	m_pEffectMap.emplace(Effect::Saved, new SoundEffect{ "Resources/Sounds/Saved.wav" });

	m_pEffectMap[Effect::PickUp]->SetVolume(m_PickupMaxVolume);
	m_pEffectMap[Effect::EnemyHit]->SetVolume(m_EnemyHitMaxVolume);
}

SoundManager::~SoundManager()
{
	for (const std::pair< Theme, SoundStream*>& element : m_pMusicMap)
	{
		delete element.second;
	}

	for (const std::pair< Effect, SoundEffect*>& element : m_pEffectMap)
	{
		delete element.second;
	}
}

void SoundManager::Update()
{
	// Max volume
	if (m_CurrentVolume == 9)
	{
		// Music
		for (const std::pair< Theme, SoundStream*>& element : m_pMusicMap)
		{
			element.second->SetVolume(100);
		}

		// Effects
		for (const std::pair<Effect, SoundEffect*>& element : m_pEffectMap)
		{
			// Max PickUp volume
			if (element.first == Effect::PickUp)
			{
				element.second->SetVolume(m_PickupMaxVolume);
			}
			// Max EnemyHit volume
			else if (element.first == Effect::EnemyHit)
			{
				element.second->SetVolume(m_EnemyHitMaxVolume);
			}
			// Others
			else
			{
				element.second->SetVolume(100);
			}
		}

		return;
	}

	// No volume
	if (m_CurrentVolume == 0)
	{
		// Music
		for (const std::pair< Theme, SoundStream*>& element : m_pMusicMap)
		{
			element.second->SetVolume(0);
		}

		// Effects
		for (const std::pair<Effect, SoundEffect*>& element : m_pEffectMap)
		{
			element.second->SetVolume(0);
		}

		return;
	}

	// Multiplier
	const int multiplier{ 11 };

	// Music
	for (const std::pair< Theme, SoundStream*>& element : m_pMusicMap)
	{
		element.second->SetVolume(m_CurrentVolume * multiplier);
	}

	// Effects
	for (const std::pair<Effect, SoundEffect*>& element : m_pEffectMap)
	{
		// Adjusted PickUp volume
		if (element.first == Effect::PickUp)
		{
			element.second->SetVolume(m_PickupMaxVolume * (m_CurrentVolume * multiplier) / 100);
		}
		// Adjusted EnemyHit volume
		else if (element.first == Effect::EnemyHit)
		{
			element.second->SetVolume(m_EnemyHitMaxVolume * (m_CurrentVolume * multiplier) / 100);
		}
		// Others
		else
		{
			element.second->SetVolume(m_CurrentVolume * multiplier);
		}
	}
}

void SoundManager::PlayTheme(Theme playTheme, bool repeat)
{
	if (m_pMusicMap.contains(playTheme))
	{
		m_pMusicMap[playTheme]->Play(repeat);
	}
}
void SoundManager::PauseTheme(bool isPaused)
{
	if (isPaused)
	{
		m_pMusicMap[m_CurrentTheme]->Pause();
	}
	else
	{
		m_pMusicMap[m_CurrentTheme]->Resume();
	}
}

void SoundManager::PlayEffect(Effect playEffect, int nrLoops)
{
	if (m_pEffectMap.contains(playEffect))
	{
		m_pEffectMap[playEffect]->Play(nrLoops);
	}
}

bool SoundManager::IsMusicPlaying() const
{
	for (const std::pair<Theme, SoundStream*>& element : m_pMusicMap)
	{
		if (element.second->IsPlaying())
		{
			return true;
		}
	}

	return false;
}

void SoundManager::SetVolume(int newVolume)
{
	m_CurrentVolume = newVolume;
}
int SoundManager::GetVolume() const
{
	return m_CurrentVolume;
}