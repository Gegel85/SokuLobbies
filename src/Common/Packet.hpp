//
// Created by PinkySmile on 30/09/2022.
//

#ifndef SOKULOBBIES_PACKET_HPP
#define SOKULOBBIES_PACKET_HPP


#include <cstdint>
#include <string>

#define PROTOCOL_VERSION 13

namespace Lobbies
{
#pragma pack(push, 1)
	enum Opcode : uint8_t {
		OPCODE_HELLO,
		OPCODE_OLLEH,
		OPCODE_PLAYER_JOIN,
		OPCODE_PLAYER_LEAVE,
		OPCODE_KICKED,
		OPCODE_MOVE,
		OPCODE_POSITION,
		OPCODE_GAME_REQUEST,
		OPCODE_GAME_START,
		OPCODE_PING,
		OPCODE_PONG,
		OPCODE_SETTINGS_UPDATE,
		OPCODE_MESSAGE,
		OPCODE_ARCADE_ENGAGE,
		OPCODE_ARCADE_LEAVE,
		OPCODE_IMPORTANT_MESSAGE,
		OPCODE_BATTLE_STATUS_UPDATE,
		OPCODE_INVALID
	};

	enum BattleStatus : uint8_t {
		BATTLE_STATUS_IDLE,
		BATTLE_STATUS_WAITING,
		BATTLE_STATUS_PLAYING,
		BATTLE_STATUS_SPECTATING
	};

	enum HostPreference : uint8_t {
		/*  0h */ HOSTPREF_CLIENT_ONLY,
		/*  1h */ HOSTPREF_HOST_ONLY,
		/*  2h */ HOSTPREF_NO_PREF,
		/*  3h */ HOSTPREF_HOST_PREF_MASK,
		/*  4h */ HOSTPREF_ACCEPT_RELAY,
		/*  8h */ HOSTPREF_ACCEPT_HOSTLIST = 8,
		/* 10h */ HOSTPREF_PREFER_RANKED = 0x10,
	};

	struct PlayerCustomization {
		uint32_t title;
		uint32_t avatar;
		uint32_t head;
		uint32_t body;
		uint32_t back;
		uint32_t env;
		uint32_t feet;
	};

	struct LobbySettings {
		HostPreference hostPref;
	};

	struct Soku2VersionInfo {
		unsigned char major;
		unsigned char minor;
		char letter;
		bool forceSoku2;
	};

	struct PacketHello {
	private:
		Opcode opcode;

	public:
		uint32_t modVersion = PROTOCOL_VERSION;
		unsigned char versionString[16];
		Soku2VersionInfo soku2Info;
		uint64_t uniqueId;
		char name[128];
		char password[32] = {0};
		PlayerCustomization custom;
		LobbySettings settings;

		PacketHello(const Soku2VersionInfo &soku2Info, unsigned char versionString[16], const std::string &name, const PlayerCustomization &custom, const LobbySettings &settings);
		PacketHello(const Soku2VersionInfo &soku2Info, unsigned char versionString[16], const std::string &name, const PlayerCustomization &custom, const LobbySettings &settings, const std::string &pwd);
		std::string toString() const;
	};

	struct PacketOlleh {
	private:
		Opcode opcode;

	public:
		uint32_t id;
		char name[32];
		char realName[64];
		uint32_t bg = 0;
		char music[8] = "op";
		char data[2048]; // This is the lobby customization

		PacketOlleh(const std::string &roomName, const std::string &realName, uint32_t id);
		std::string toString() const;
	};

	struct PacketPlayerJoin {
	private:
		Opcode opcode;

	public:
		uint32_t id;
		char name[64];
		PlayerCustomization custom;

		PacketPlayerJoin(uint32_t id, const std::string &name, PlayerCustomization custom);
		std::string toString() const;
	};

	struct PacketPlayerLeave {
	private:
		Opcode opcode;

	public:
		uint32_t id;

		PacketPlayerLeave(uint32_t id);
		std::string toString() const;
	};

	struct PacketKicked {
	private:
		Opcode opcode;

	public:
		char message[2048];

		PacketKicked(const std::string &msg);
		std::string toString() const;
	};

	struct PacketMove {
	private:
		Opcode opcode;

	public:
		uint32_t id;
		uint8_t dir;

		PacketMove(uint32_t id, uint8_t dir);
		std::string toString() const;
	};

	struct PacketPosition {
	private:
		Opcode opcode;

	public:
		uint32_t id;
		uint32_t x;
		uint32_t y;
		uint8_t dir;
		BattleStatus status;

		PacketPosition(uint32_t id, uint32_t x, uint32_t y, uint8_t dir, BattleStatus status);
		std::string toString() const;
	};

	struct PacketGameRequest {
	private:
		Opcode opcode;

	public:
		uint32_t consoleId;

		PacketGameRequest(uint32_t consoleId);
		std::string toString() const;
	};

	struct PacketGameStart {
	private:
		Opcode opcode;

	public:
		bool spectator;
		char ip[16];
		uint16_t port;
		char ipv6[46];
		uint16_t port6;

		PacketGameStart(const std::string &ip, uint16_t port, const std::string &ipv6, uint16_t port6, bool spectator);
		std::string toString() const;
	};

	struct PacketPing {
	private:
		Opcode opcode;

	public:
		PacketPing();
		std::string toString() const;
	};

	struct PacketPong {
	private:
		Opcode opcode;

	public:
		char name[32];
		uint8_t maxPlayers;
		uint8_t currentPlayers;
		bool requiresPwd = false;

		PacketPong(const std::string &roomName, uint8_t maxPlayers, uint8_t currentPlayers, const char *password);
		std::string toString() const;
	};

	struct PacketSettingsUpdate {
	private:
		Opcode opcode;

	public:
		uint32_t id;
		PlayerCustomization custom;
		LobbySettings settings;

		PacketSettingsUpdate(uint32_t id, const PlayerCustomization &custom, const LobbySettings settings);
		std::string toString() const;
	};

	struct PacketMessage {
	private:
		Opcode opcode;

	public:
		int32_t channelId;
		uint32_t playerId;
		char message[1240];

		PacketMessage(int32_t channelId, uint32_t playerId, const std::string &message);
		std::string toString() const;
	};

	struct PacketArcadeEngage {
	private:
		Opcode opcode;

	public:
		uint32_t id;
		uint32_t machineId;

		PacketArcadeEngage(uint32_t id, uint32_t machineId);
		std::string toString() const;
	};

	struct PacketArcadeLeave {
	private:
		Opcode opcode;

	public:
		uint32_t id;

		PacketArcadeLeave(uint32_t id);
		std::string toString() const;
	};

	struct PacketImportantMessage {
	private:
		Opcode opcode;

	public:
		char message[1024];

		PacketImportantMessage(const std::string &msg);
		std::string toString() const;
	};

	struct PacketBattleStatusUpdate {
	private:
		Opcode opcode;

	public:
		uint32_t playerId;
		BattleStatus newStatus;

		PacketBattleStatusUpdate(uint32_t playerId, BattleStatus newStatus);
		std::string toString() const;
	};

	union Packet {
		Opcode opcode;
		PacketHello hello;
		PacketOlleh olleh;
		PacketPlayerJoin playerJoin;
		PacketPlayerLeave playerLeave;
		PacketKicked kicked;
		PacketMove move;
		PacketPosition position;
		PacketGameRequest gameRequest;
		PacketGameStart gameStart;
		PacketPing ping;
		PacketPong pong;
		PacketSettingsUpdate settingsUpdate;
		PacketMessage message;
		PacketArcadeEngage arcadeEngage;
		PacketArcadeLeave arcadeLeave;
		PacketImportantMessage importantMsg;
		PacketBattleStatusUpdate battleStatusUpdate;

		Packet();
		std::string toString() const;
	};
#pragma pack(pop)
}


#endif //SOKULOBBIES_PACKET_HPP
