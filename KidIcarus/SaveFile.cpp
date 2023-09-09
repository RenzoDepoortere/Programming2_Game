#include "pch.h"
#include "SaveFile.h"

#include <iostream>
#include <sstream>
#include <fstream>

SaveFile::SaveFile(const std::string& saveFilePath)
	: m_SaveFile{saveFilePath}
{
	LoadContent();
}

void SaveFile::SaveVariables()
{
	std::ofstream outStream{ m_SaveFile };
	if (!outStream)
	{
		std::cout << "Error opening: " << m_SaveFile << '\n';
	}

	// Variables To String
	const std::string writeText{VariablesToString()};
	for (size_t i{}; i < writeText.size(); i++)
	{
		outStream << writeText[i];
	}
}

int SaveFile::GetSoundVolume() const
{
	return m_SoundVolume;
}
void SaveFile::SetSoundVolume(int soundVolume)
{
	m_SoundVolume = soundVolume;
}

int SaveFile::GetHealthState() const
{
	return m_HealthState;
}
void SaveFile::SetHealthState(int healthState)
{
	m_HealthState = healthState;
}

int SaveFile::GetScore() const
{
	return m_Score;
}
void SaveFile::SetScore(int score)
{
	m_Score = score;
}

Point2f SaveFile::GetPlayerPos() const
{
	return m_PlayerPos;
}
void SaveFile::SetPlayerPos(const Point2f& playerPos)
{
	m_PlayerPos = playerPos;
}

Point2f SaveFile::GetCameraPos() const
{
	return m_CameraPos;
}
void SaveFile::SetCameraPos(const Point2f& cameraPos)
{
	m_CameraPos = cameraPos;
}

Rectf SaveFile::GetLevelBoundaries() const
{
	return m_LevelBoundaries;
}
void SaveFile::SetLevelBoundaries(const Rectf& levelBoundaries)
{
	m_LevelBoundaries = levelBoundaries;
}

void SaveFile::LoadContent()
{
	std::ifstream inStream{ m_SaveFile };
	if (!inStream)
	{
		std::cout << "Error opening " << m_SaveFile << '\n';
		return;
	}


	std::string element{};
	std::string seenString{};

	do
	{
		std::getline(inStream, element, '>');
		if (element.find('/') == (element.size() - 1))
		{
			element += '>';
		}

		LoadVariables(element);

	} while (element.find('<') != std::string::npos);
}

void SaveFile::LoadVariables(const std::string& variablesElement)
{
	if (variablesElement.find("Sound") != std::string::npos)
	{
		m_SoundVolume = std::stoi(GetAttributeValue("Volume", variablesElement));
	}

	if (variablesElement.find("Player") != std::string::npos)
	{
		m_HealthState = std::stoi(GetAttributeValue("Health", variablesElement));
		m_Score = std::stoi(GetAttributeValue("Score", variablesElement));
		m_PlayerPos = ToPoint2f(GetAttributeValue("Pos", variablesElement));
	}

	if (variablesElement.find("Camera") != std::string::npos)
	{
		m_CameraPos = ToPoint2f(GetAttributeValue("Pos", variablesElement));
	}

	if (variablesElement.find("Level") != std::string::npos)
	{
		const Point2f levelPos{ ToPoint2f(GetAttributeValue("Pos",variablesElement)) };
		m_LevelBoundaries.left = levelPos.x;
		m_LevelBoundaries.bottom = levelPos.y;
		m_LevelBoundaries.width = std::stof(GetAttributeValue("Width", variablesElement));
		m_LevelBoundaries.height = std::stof(GetAttributeValue("Height", variablesElement));
	}
}

std::string SaveFile::GetAttributeValue(const std::string& attrName, const std::string& element) const
{
	std::string attribute;
	std::string input;

	const std::size_t attrSize{ attrName.size() };
	std::size_t attrPos{ element.find(attrName) };

	// Checks if found name is actual variable
	const std::string checkString{ element.substr(attrPos + 1, attrName.size() + 1) };
	if (checkString.find('=') == std::string::npos)
	{
		attrPos = element.find(attrName, attrPos + 1);
	}

	if (attrPos != std::string::npos)
	{
		const std::size_t firstPos{ element.find('"', attrPos) };
		const std::size_t secPos{ element.find('"',firstPos + 1) };
		const std::size_t valueSize{ (secPos - 1) - firstPos };

		attribute = element.substr(firstPos + 1, valueSize);
	}

	return attribute;
}

Point2f SaveFile::ToPoint2f(const std::string& point2fStr) const
{
	Point2f point;

	std::size_t firstSize{ point2fStr.find(',') };
	const std::size_t secondSize{ point2fStr.size() - (firstSize + 1) };

	std::string pieceStr{ point2fStr.substr(0, firstSize) };
	point.x = std::stof(pieceStr);

	pieceStr = point2fStr.substr(firstSize + 1, secondSize);
	point.y = std::stof(pieceStr);

	return point;
}

std::string SaveFile::VariablesToString() const
{
	// Sound
	std::string object{"<Sound" };
	object += "\n Volume=\"" + std::to_string(m_SoundVolume) + '"';
	object += "\n/> \n\n";

	// Player
	object += "<Player";
	object += "\n Health=\"" + std::to_string(m_HealthState) + '"';
	object += "\n Score=\"" + std::to_string(m_Score) + '"';
	object += "\n Pos=\"" + std::to_string(m_PlayerPos.x) + "," + std::to_string(m_PlayerPos.y) + '"';
	object += "\n/> \n\n";

	// Camera
	object += "<Camera";
	object += "\n Pos=\"" + std::to_string(m_CameraPos.x) + "," + std::to_string(m_CameraPos.y) + '"';
	object += "\n/> \n\n";

	// Level
	object += "<Level";
	object += "\n Pos=\"" + std::to_string(m_LevelBoundaries.left) + "," + std::to_string(m_LevelBoundaries.bottom) + '"';
	object += "\n Width=\"" + std::to_string(m_LevelBoundaries.width) + '"';
	object += "\n Height=\"" + std::to_string(m_LevelBoundaries.height) + '"';
	object += "\n/> \n\n";

	return object;
}