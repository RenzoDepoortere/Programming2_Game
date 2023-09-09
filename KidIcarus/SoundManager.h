#pragma once
#include <unordered_map>
class SoundStream;
class SoundEffect;

class SoundManager final
{
public:
	enum class Theme
	{
		Level, Dungeon, Death, Title
	};

	enum class Effect
	{
		Shoot , Jump, PickUp, EnemyHit, Hurt, Pause, Select, Saved
	};

	explicit SoundManager(int soundVolume);
	~SoundManager();
	SoundManager(const SoundManager& other) = delete;
	SoundManager& operator= (const SoundManager& rhs) = delete;
	SoundManager(SoundManager&& other) = delete;
	SoundManager& operator= (SoundManager&& rhs) = delete;

	void Update();

	void PlayTheme(Theme playTheme, bool repeat);
	void PauseTheme(bool isPaused);

	void PlayEffect(Effect playEffect, int nrLoops = 0);
	bool IsMusicPlaying() const;

	void SetVolume(int newVolume);
	int GetVolume() const;

private:
	const int m_PickupMaxVolume;
	const int m_EnemyHitMaxVolume;
	
	int m_CurrentVolume;

	Theme m_CurrentTheme;

	std::unordered_map<Theme, SoundStream*> m_pMusicMap;
	std::unordered_map<Effect, SoundEffect*> m_pEffectMap;

};

