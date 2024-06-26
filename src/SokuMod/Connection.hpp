//
// Created by PinkySmile on 01/10/2022.
//

#ifndef SOKULOBBIES_CONNECTION_HPP
#define SOKULOBBIES_CONNECTION_HPP


#include <thread>
#include <mutex>
#include <optional>
#include <functional>
#include <Packet.hpp>
#include <Vector2.hpp> //From SokuLib
#include "Player.hpp"
#include "Socket.hpp"
#include "LobbyData.hpp"

#define PLAYER_H_SPEED 4
#define PLAYER_V_SPEED 4

class Connection {
public:
	struct LobbyInfo {
		std::string name;
		uint8_t maxPlayers = 0;
		uint8_t currentPlayers = 0;
		bool hasPwd = false;
	};

private:
	mutable std::mutex _messagesMutex;
	mutable std::mutex _playerMutex;
	std::mutex _infoMutex;
	std::thread _netThread;
	std::thread _posThread;
	bool _connected = true;
	bool _init = false;
	char _uniqueId[16];
	std::string _name;
	Socket _socket;
	LobbyInfo _info;
	const Player &_initParams;
	Player *_me = nullptr;
	std::optional<std::string> _pwd;
	std::map<uint32_t, uint32_t> _machines;
	std::map<uint32_t, Player> _players;
	std::vector<std::string> _messages; //TODO: properly handle channels

	static std::vector<std::function<void (Player &, const LobbyData::Avatar &)>> _playerUpdateHandles;

	void _netLoop();
	void _posLoop();
	bool _handlePacket(const Lobbies::Packet &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketHello &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketOlleh &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketPlayerJoin &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketPlayerLeave &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketKicked &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketMove &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketPosition &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketGameRequest &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketGameStart &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketPing &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketPong &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketSettingsUpdate &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketArcadeEngage &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketArcadeLeave &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketMessage &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketImportantMessage &packet, size_t &size);
	bool _handlePacket(const Lobbies::PacketBattleStatusUpdate &packet, size_t &size);

public:
	std::function<void (const std::string &ip, unsigned short port, bool spectate)> onConnectRequest;
	std::function<void (int32_t channel, int32_t player, const std::string &msg)> onMsg;
	std::function<void (const Player &, uint32_t id)> onArcadeEngage;
	std::function<void (const Player &, uint32_t id)> onArcadeLeave;
	std::function<void (const Lobbies::PacketOlleh &)> onConnect;
	std::function<void (const std::string &msg)> onImpMsg;
	std::function<void (const std::string &msg)> onError;
	std::function<void (const Player &)> onPlayerJoin;
	std::function<unsigned short ()> onHostRequest;
	std::function<void ()> onDisconnect;
	std::mutex meMutex;
	std::mutex functionMutex;

	Connection(const std::string &host, unsigned short port, const Player &initParams);
	~Connection();
	void startThread();
	void setPassword(const std::string &pwd);
	void error(const std::string &msg);
	void connect();
	void disconnect();
	void send(const void *packet, size_t size);
	bool isInit() const;
	bool isConnected() const;
	const LobbyInfo getLobbyInfo() const;
	bool sendGameInfo();
	Player *getMe();
	const Player *getMe() const;
	std::vector<Player> getPlayers() const;
	std::vector<std::string> getMessages() const;
	void updatePlayers(const std::vector<LobbyData::Avatar> &avatars);
};


#endif //SOKULOBBIES_CONNECTION_HPP
