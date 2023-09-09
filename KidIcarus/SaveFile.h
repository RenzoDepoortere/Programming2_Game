#pragma once

class SaveFile final
{
public:

	explicit SaveFile(const std::string& saveFilePath);
	~SaveFile() = default;
	SaveFile(const SaveFile& other) = delete;
	SaveFile& operator= (const SaveFile& rhs) = delete;
	SaveFile(SaveFile&& other) = delete;
	SaveFile& operator= (SaveFile&& rhs) = delete;

	void SaveVariables();

	int GetSoundVolume() const;
	void SetSoundVolume(int soundVolume);

	int GetHealthState() const;
	void SetHealthState(int healthState);

	int GetScore() const;
	void SetScore(int score);

	Point2f GetPlayerPos() const;
	void SetPlayerPos(const Point2f& playerPos);

	Point2f GetCameraPos() const;
	void SetCameraPos(const Point2f& cameraPos);

	Rectf GetLevelBoundaries() const;
	void SetLevelBoundaries(const Rectf& levelBoundaries);

private:
	// Variables
	const std::string m_SaveFile;
	
	int m_SoundVolume;
	int m_HealthState;
	int m_Score;
	Point2f m_PlayerPos;
	Point2f m_CameraPos;
	Rectf m_LevelBoundaries;

	// Functions
	void LoadContent();
	void LoadVariables(const std::string& variablesElement);
	std::string GetAttributeValue(const std::string& attrName, const std::string& element) const;
	Point2f ToPoint2f(const std::string& point2fStr) const;
	std::string VariablesToString() const;
};

