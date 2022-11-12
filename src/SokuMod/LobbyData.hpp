//
// Created by PinkySmile on 10/11/2022.
//

#ifndef SOKULOBBIES_LOBBYDATA_HPP
#define SOKULOBBIES_LOBBYDATA_HPP


#include <map>
#include <vector>
#include <string>
#include <SokuLib.hpp>

#define EMOTE_SIZE 32
#define CHR_STATS_MAGIC 0xF2A6E790

class LobbyData {
private:
	void _saveStats();
	void _loadStats();
	void _loadAvatars();
	void _loadBackgrounds();
	void _loadEmotes();
	void _loadArcades();

	unsigned _getExpectedMagic();
	void _loadCharacterStats(std::istream &stream);
	void _loadCharacterCardUsage(std::istream &stream);
	void _loadMatchupStats(std::istream &stream);
	void _saveCharacterStats(std::ostream &stream);
	void _saveCharacterCardUsage(std::ostream &stream);
	void _saveMatchupStats(std::ostream &stream);

public:
	//Stats
	struct CharacterStatEntry {
		unsigned wins;
		unsigned losses;
		unsigned againstWins;
		unsigned againstLosses;
	};
	struct MatchupStatEntry {
		unsigned wins;
		unsigned losses;
	};
	struct CardStatEntry {
		unsigned inDeck;
		unsigned used;
		unsigned burnt;
	};
	struct CardChrStatEntry {
		unsigned totalCards;
		CardStatEntry cards[35];
	};

	struct Avatar {
		unsigned short id = 0;
		std::string name;
		float scale = 0;
		SokuLib::DrawUtils::Sprite sprite;
		unsigned accessoriesPlacement = 0;
		unsigned animationsStep = 0;
		unsigned nbAnimations = 0;

		Avatar() = default;
		Avatar(const Avatar &) { assert(false); }
	};
	struct Background {
		unsigned short id = 0;
		SokuLib::DrawUtils::Sprite bg;
		SokuLib::DrawUtils::Sprite fg;
		unsigned groundPos = 0;
		float parallaxFactor = 0;
		unsigned platformInterval = 0;
		unsigned platformWidth = 0;
		unsigned platformCount = 0;

		Background() = default;
		Background(const Background &) { assert(false); }
	};
	struct Emote {
		unsigned short id = 0;
		std::string filepath;
		std::vector<std::string> alias;
		SokuLib::DrawUtils::Sprite sprite;

		Emote() = default;
		Emote(const Emote &) { assert(false); }
	};
	struct ArcadeAnimation {
		std::string file;
		SokuLib::DrawUtils::Sprite sprite;
		unsigned tilePerLine;
		SokuLib::Vector2u size;
		unsigned frameRate;
		unsigned frameCount;
		bool loop;

		ArcadeAnimation() = default;
		ArcadeAnimation(const ArcadeAnimation &) { assert(false); }
	};
	struct ArcadeSkin {
		std::string file;
		SokuLib::DrawUtils::Sprite sprite;
		SokuLib::Vector2i animationOffsets;
		unsigned frameRate;
		unsigned frameCount;

		ArcadeSkin() = default;
		ArcadeSkin(const ArcadeSkin &) { assert(false); }
	};
	struct ArcadeData {
		ArcadeAnimation intro;
		ArcadeAnimation select;
		std::vector<ArcadeAnimation> game;
		std::vector<ArcadeSkin> skins;
	};

	ArcadeData arcades;
	std::vector<Emote> emotes;
	std::vector<Avatar> avatars;
	std::vector<Background> backgrounds;
	std::map<std::string, Emote *> emotesByName;
	std::map<unsigned char, CharacterStatEntry> loadedCharacterStats;
	std::map<unsigned char, CardChrStatEntry> loadedCharacterCardUsage;
	std::map<std::pair<unsigned char, unsigned char>, MatchupStatEntry> loadedMatchupStats;

	LobbyData();
	~LobbyData();
	bool isLocked(const Emote &emote);
	bool isLocked(const Avatar &avatar);
	bool isLocked(const Background &background);
};

extern std::unique_ptr<LobbyData> lobbyData;

#endif //SOKULOBBIES_LOBBYDATA_HPP