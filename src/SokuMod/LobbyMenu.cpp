//
// Created by PinkySmile on 02/10/2022.
//

#include <iostream>
#include <fstream>
#include <filesystem>
#include <../directx/dinput.h>
#include "nlohmann/json.hpp"
#include "LobbyMenu.hpp"
#include "InLobbyMenu.hpp"
#include "Exceptions.hpp"

#define CRenderer_Unknown1 ((void (__thiscall *)(int, int))0x404AF0)

extern wchar_t profileFolderPath[MAX_PATH];
extern char servHost[64];
extern unsigned short servPort;

void displaySokuCursor(SokuLib::Vector2i pos, SokuLib::Vector2u size)
{
	SokuLib::Sprite (&CursorSprites)[3] = *(SokuLib::Sprite (*)[3])0x89A6C0;

	//0x443a50 -> Vanilla display cursor
	CursorSprites[0].scale.x = size.x * 0.00195313f;
	CursorSprites[0].scale.y = size.y / 16.f;
	pos.x -= 7;
	CursorSprites[0].render(pos.x, pos.y);
	CRenderer_Unknown1(0x896B4C, 2);
	CursorSprites[1].rotation = *(float *)0x89A450 * 4.00000000f;
	CursorSprites[1].render(pos.x, pos.y + 8.00000000f);
	CursorSprites[2].rotation = -*(float *)0x89A450 * 4.00000000f;
	CursorSprites[2].render(pos.x - 14.00000000f, pos.y - 1.00000000f);
	CRenderer_Unknown1(0x896B4C, 1);
}

LobbyMenu::LobbyMenu(SokuLib::MenuConnect *parent) :
	_parent(parent),
	avatars()
{
	std::ifstream ips{std::filesystem::path(profileFolderPath) / "ip.txt"};
	std::string ip;

	std::getline(ips, ip);
	if (ip.empty())
		ip = "localhost";
	ips.close();

	auto path = std::filesystem::path(profileFolderPath) / "assets/avatars/list.json";
	std::ifstream stream{path};
	nlohmann::json j;

	if (stream.fail())
		throw std::runtime_error("Cannot open file " + path.string());
	stream >> j;
	stream.close();
	this->avatars.reserve(j.size());
	for (auto &val : j) {
		this->avatars.emplace_back();

		auto &avatar = this->avatars.back();

		avatar.accessoriesPlacement = val["accessories"];
		avatar.nbAnimations = val["animations"];
		avatar.animationsStep = val["anim_step"];
		avatar.sprite.texture.loadFromFile((std::filesystem::path(profileFolderPath) / val["spritesheet"].get<std::string>()).string().c_str());
		avatar.sprite.rect.width = avatar.sprite.texture.getSize().x / avatar.nbAnimations;
		avatar.sprite.rect.height = avatar.sprite.texture.getSize().y / 2;
		avatar.sprite.setSize({
			static_cast<unsigned int>(avatar.sprite.rect.width * val["scale"].get<float>()),
			static_cast<unsigned int>(avatar.sprite.rect.height * val["scale"].get<float>())
		});
	}

	path = std::filesystem::path(profileFolderPath) / "assets/backgrounds/list.json";
	stream.open(path);
	if (stream.fail())
		throw std::runtime_error("Cannot open file " + path.string());
	stream >> j;
	stream.close();
	this->backgrounds.reserve(j.size());
	for (auto &val : j) {
		this->backgrounds.emplace_back();

		auto &bg = this->backgrounds.back();

		bg.groundPos = val["ground"];
		bg.parallaxFactor = val["parallax_factor"];
		bg.platformInterval = val["platform_interval"];
		bg.platformWidth = val["platform_width"];
		bg.platformCount = val["platform_count"];
		bg.fg.texture.loadFromFile((std::filesystem::path(profileFolderPath) / val["fg"].get<std::string>()).string().c_str());
		bg.fg.setSize(bg.fg.texture.getSize());
		bg.fg.rect.width = bg.fg.getSize().x;
		bg.fg.rect.height = bg.fg.getSize().y;
		bg.bg.texture.loadFromFile((std::filesystem::path(profileFolderPath) / val["bg"].get<std::string>()).string().c_str());
		bg.bg.setSize(bg.bg.texture.getSize());
		bg.bg.rect.width = bg.bg.getSize().x;
		bg.bg.rect.height = bg.bg.getSize().y;
	}

	this->title.texture.loadFromFile((std::filesystem::path(profileFolderPath) / "assets/menu/title.png").string().c_str());
	this->title.setSize(this->title.texture.getSize());
	this->title.setPosition({23, 6});
	this->title.rect.width = this->title.getSize().x;
	this->title.rect.height = this->title.getSize().y;

	this->ui.texture.loadFromFile((std::filesystem::path(profileFolderPath) / "assets/menu/lobbylist.png").string().c_str());
	this->ui.setSize(this->ui.texture.getSize());
	this->ui.rect.width = this->ui.getSize().x;
	this->ui.rect.height = this->ui.getSize().y;

	//TODO: Save and load this in a file
	this->_loadedSettings.settings.hostPref = Lobbies::HOSTPREF_NO_PREF;
	this->_loadedSettings.player.title = 0;
	this->_loadedSettings.player.avatar = time(nullptr) % this->avatars.size();//0;
	this->_loadedSettings.player.head = 0;
	this->_loadedSettings.player.body = 0;
	this->_loadedSettings.player.back = 0;
	this->_loadedSettings.player.env = 0;
	this->_loadedSettings.player.feet = 0;
	this->_loadedSettings.name = SokuLib::profile1.name.operator std::string();
	this->_loadedSettings.pos.x = 20;

	SokuLib::FontDescription desc;
	bool hasEnglishPatch = (*(int *)0x411c64 == 1);

	desc.r1 = 255;
	desc.r2 = 255;
	desc.g1 = 255;
	desc.g2 = 255;
	desc.b1 = 255;
	desc.b2 = 255;
	desc.height = 12 + hasEnglishPatch * 2;
	desc.weight = FW_NORMAL;
	desc.italic = 0;
	desc.shadow = 1;
	desc.bufferSize = 1000000;
	desc.charSpaceX = 0;
	desc.charSpaceY = hasEnglishPatch * -2;
	desc.offsetX = 0;
	desc.offsetY = 0;
	desc.useOffset = 0;
	strcpy(desc.faceName, "MonoSpatialModSWR");
	desc.weight = FW_REGULAR;
	this->_defaultFont12.create();
	this->_defaultFont12.setIndirect(desc);
	desc.height = 16 + hasEnglishPatch * 2;
	this->_defaultFont16.create();
	this->_defaultFont16.setIndirect(desc);

	this->_loadingText.texture.createFromText("Connecting to server...", this->_defaultFont16, {600, 74});
	this->_loadingText.setSize({
		this->_loadingText.texture.getSize().x,
		this->_loadingText.texture.getSize().y
	});
	this->_loadingText.rect.width = this->_loadingText.texture.getSize().x;
	this->_loadingText.rect.height = this->_loadingText.texture.getSize().y;
	this->_loadingText.setPosition({164, 218});

	this->_messageBox.texture.loadFromGame("data/menu/21_Base.cv2");
	this->_messageBox.setSize({
		this->_messageBox.texture.getSize().x,
		this->_messageBox.texture.getSize().y
	});
	this->_messageBox.rect.width = this->_messageBox.texture.getSize().x;
	this->_messageBox.rect.height = this->_messageBox.texture.getSize().y;
	this->_messageBox.setPosition({155, 203});

	this->_loadingGear.texture.loadFromGame("data/scene/logo/gear.bmp");
	this->_loadingGear.setSize({
		this->_loadingGear.texture.getSize().x,
		this->_loadingGear.texture.getSize().y
	});
	this->_loadingGear.rect.width = this->_loadingGear.texture.getSize().x;
	this->_loadingGear.rect.height = this->_loadingGear.texture.getSize().y;

	this->_netThread = std::thread(&LobbyMenu::_netLoop, this);
	this->_connectThread = std::thread(&LobbyMenu::_connectLoop, this);
	this->_masterThread = std::thread(&LobbyMenu::_masterServerLoop, this);
}

LobbyMenu::~LobbyMenu()
{
	this->_open = false;
	if (this->_netThread.joinable())
		this->_netThread.join();
	this->_mainServer.disconnect();
	if (this->_masterThread.joinable())
		this->_masterThread.join();
	if (this->_connectThread.joinable())
		this->_connectThread.join();
}

void LobbyMenu::_netLoop()
{
	while (this->_open) {
		Lobbies::PacketPing ping;

		this->_connectionsMutex.lock();
		for (auto &c : this->_connections)
			if (c->c && !c->c->isInit() && c->c->isConnected())
				c->c->send(&ping, sizeof(ping));
		this->_connectionsMutex.unlock();
		for (int i = 0; i < 20 && this->_open; i++)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void LobbyMenu::_()
{
	puts("_ !");
	*(int *)0x882a94 = 0x16;
}

int LobbyMenu::onProcess()
{
	if (SokuLib::checkKeyOneshot(DIK_ESCAPE, 0, 0, 0)) {
		SokuLib::playSEWaveBuffer(0x29);
		this->_open = false;
		return false;
	}
	if (this->_mainServer.isDisconnected()) {
		this->_loadingGear.setRotation(this->_loadingGear.getRotation() + 0.1);
		return true;
	}
	if (this->_menuState == 0) {
		if (SokuLib::inputMgrs.input.b == 1) {
			SokuLib::playSEWaveBuffer(0x29);
			this->_open = false;
			return false;
		}
		if (std::abs(SokuLib::inputMgrs.input.verticalAxis) == 1 || (std::abs(SokuLib::inputMgrs.input.verticalAxis) > 36 && std::abs(SokuLib::inputMgrs.input.verticalAxis) % 6 == 0)) {
			if (SokuLib::inputMgrs.input.verticalAxis < 0) {
				this->_menuCursor += 8;
				this->_menuCursor--;
			} else
				this->_menuCursor++;
			this->_menuCursor %= 8;
			SokuLib::playSEWaveBuffer(0x27);
		}
		if (SokuLib::inputMgrs.input.a == 1) {
			SokuLib::playSEWaveBuffer(0x28);
			switch (this->_menuCursor) {
			case 1:
				this->_menuState = 1;
				break;
			case 0:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				MessageBox(SokuLib::window, "Not implemented", "Not implemented", MB_ICONINFORMATION);
				break;
			case 7:
				return false;
			}
		}
	} else if (this->_menuState == 1) {
		if (SokuLib::inputMgrs.input.b == 1) {
			SokuLib::playSEWaveBuffer(0x29);
			this->_menuState = 0;
			return true;
		}
		this->_connectionsMutex.lock();
		if (std::abs(SokuLib::inputMgrs.input.verticalAxis) == 1 || (std::abs(SokuLib::inputMgrs.input.verticalAxis) > 36 && std::abs(SokuLib::inputMgrs.input.verticalAxis) % 6 == 0)) {
			if (SokuLib::inputMgrs.input.verticalAxis < 0) {
				this->_lobbyCtr += this->_connections.size();
				this->_lobbyCtr--;
			} else
				this->_lobbyCtr++;
			if (!this->_connections.empty())
				this->_lobbyCtr %= this->_connections.size();
			SokuLib::playSEWaveBuffer(0x27);
		}
		if (SokuLib::inputMgrs.input.a == 1) {
			if (this->_lobbyCtr < this->_connections.size() && this->_connections[this->_lobbyCtr]->c && this->_connections[this->_lobbyCtr]->c->isConnected()) {
				SokuLib::activateMenu(new InLobbyMenu(this, this->_parent, *this->_connections[this->_lobbyCtr]->c));
				this->_active = false;
				SokuLib::playSEWaveBuffer(0x28);
			} else
				SokuLib::playSEWaveBuffer(0x29);
		}
		this->_connectionsMutex.unlock();
	}
	return true;
}

int LobbyMenu::onRender()
{
	this->title.draw();
	if (this->_mainServer.isDisconnected()) {
		this->_messageBox.draw();
		this->_loadingText.draw();
		if (this->_lastError.empty()) {
			this->_loadingGear.setRotation(-this->_loadingGear.getRotation());
			this->_loadingGear.setPosition({412, 227});
			this->_loadingGear.draw();
			this->_loadingGear.setRotation(-this->_loadingGear.getRotation());
			this->_loadingGear.setPosition({412 + 23, 227 - 18});
			this->_loadingGear.draw();
		}
		return 0;
	}
	if (this->_menuCursor == 7)
		displaySokuCursor({50, 366}, {180, 16});
	else
		displaySokuCursor({50, static_cast<int>(126 + this->_menuCursor * 24)}, {180, 16});
	this->ui.draw();
	this->_connectionsMutex.lock();
	if (this->_menuState == 1)
		displaySokuCursor({312, static_cast<int>(120 + this->_lobbyCtr * 16)}, {220, 16});
	for (int i = 0; i < this->_connections.size(); i++) {
		this->_connections[i]->name.setPosition({312, 120 + i * 16});
		this->_connections[i]->name.draw();
		this->_connections[i]->playerCount.setPosition({static_cast<int>(619 - this->_connections[i]->playerCount.getSize().x), 120 + i * 16});
		this->_connections[i]->playerCount.draw();
	}
	this->_connectionsMutex.unlock();
	return 0;
}

void LobbyMenu::setActive()
{
	this->_active = true;
}

void LobbyMenu::_masterServerLoop()
{
	while (this->_open) {
		bool _locked = false;

		if (this->_mainServer.isDisconnected())
			try {
				this->_loadingText.texture.createFromText("Connecting to server...", this->_defaultFont16, {600, 74});
				this->_lastError.clear();
				this->_mainServer.connect(servHost, servPort);
				puts("Connected!");
			} catch (std::exception &e) {
				this->_loadingText.texture.createFromText(("Connection failed:<br><color FF0000>" + std::string(e.what()) + "</color>").c_str(), this->_defaultFont16, {600, 74});
				this->_lastError = e.what();
				goto fail;
			}

		try {
			char packet[6] = {0};
			std::vector<std::pair<unsigned short, std::string>> elems;

			packet[0] = 1;
			this->_mainServer.send(packet, 3);
			this->_mainServer.read(packet, 6);
			while (packet[0] || packet[1]) {
				struct in_addr addr;

				addr.S_un.S_un_b.s_b1 = packet[2];
				addr.S_un.S_un_b.s_b2 = packet[3];
				addr.S_un.S_un_b.s_b3 = packet[4];
				addr.S_un.S_un_b.s_b4 = packet[5];
				elems.emplace_back(packet[0] | packet[1] << 8, inet_ntoa(addr));
				this->_mainServer.read(packet, 6);
			}
			_locked = true;
			this->_connectionsMutex.lock();
			this->_connections.erase(std::remove_if(this->_connections.begin(), this->_connections.end(), [&elems](const std::shared_ptr<Entry> &e){
				return (!e->c || !e->c->isConnected()) && std::find_if(elems.begin(), elems.end(), [&e](const std::pair<unsigned short, std::string> &e1){
					return e->port == e1.first && e->ip == e1.second;
				}) == elems.end();
			}), this->_connections.end());
			this->_connectionsMutex.unlock();
			for (auto &elem : elems) {
				this->_connectionsMutex.lock();
				if (std::find_if(this->_connections.begin(), this->_connections.end(), [&elem](const std::shared_ptr<Entry> &e){
					return e->ip == elem.second && e->port == elem.first;
				}) == this->_connections.end()) {
					this->_connections.emplace_back(new Entry{std::shared_ptr<Connection>(), elem.second, elem.first});

					auto c = this->_connections.back();

					this->_connectionsMutex.unlock();
					c->name.texture.createFromText("Connection to the lobby in queue...", this->_defaultFont16, {300, 74});
					c->name.setSize({
						c->name.texture.getSize().x,
						c->name.texture.getSize().y
					});
					c->name.rect.width = c->name.texture.getSize().x;
					c->name.rect.height = c->name.texture.getSize().y;
					c->name.tint = SokuLib::Color{0xA0, 0xA0, 0xA0, 0xFF};
				} else
					this->_connectionsMutex.unlock();
			}
		} catch (std::exception &e) {
			if (dynamic_cast<EOFException *>(&e))
				this->_mainServer.disconnect();
			this->_loadingText.texture.createFromText(("<color FF0000>Error when communicating with master server:</color><br>" + std::string(e.what())).c_str(), this->_defaultFont16, {600, 74});
			this->_lastError = e.what();
			if (_locked)
				this->_connectionsMutex.unlock();
		}
	fail:
		for (int i = 0; i < 100 && this->_open; i++)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void LobbyMenu::_connectLoop()
{
	int i = 0;

	while (this->_open) {
		for (int j = 0; ; j++) {
			this->_connectionsMutex.lock();
			if (j >= this->_connections.size()) {
				this->_connectionsMutex.unlock();
				break;
			}

			auto connection = this->_connections[j];

			this->_connectionsMutex.unlock();

			auto weak = std::weak_ptr(connection);

			try {
				if (!connection->c || !connection->c->isConnected()) {
					if (i != 0 && !connection->first)
						continue;

					connection->first = false;
					connection->name.texture.createFromText("Connecting to lobby...", this->_defaultFont16, {300, 74});
					connection->name.setSize({
						connection->name.texture.getSize().x,
						connection->name.texture.getSize().y
					});
					connection->name.rect.width = connection->name.texture.getSize().x;
					connection->name.rect.height = connection->name.texture.getSize().y;
					connection->name.tint = SokuLib::Color{0xFF, 0xFF, 0x00, 0xFF};

					connection->c = std::make_shared<Connection>(connection->ip, connection->port, this->_loadedSettings);
					connection->c->onError = [weak, this](const std::string &msg) {
						auto c = weak.lock();

						c->lastName = msg;
						c->name.texture.createFromText(c->lastName.c_str(), this->_defaultFont16, {300, 74});
						c->name.setSize({
							c->name.texture.getSize().x,
							c->name.texture.getSize().y
						});
						c->name.rect.width = c->name.texture.getSize().x;
						c->name.rect.height = c->name.texture.getSize().y;
						c->name.tint = SokuLib::Color::Red;
						SokuLib::playSEWaveBuffer(38);
						std::cerr << "Error:" << msg << std::endl;
					};
					connection->c->onImpMsg = [weak, this](const std::string &msg) {
						auto c = weak.lock();

						c->lastName = msg;
						c->name.texture.createFromText(c->lastName.c_str(), this->_defaultFont16, {300, 74});
						c->name.setSize({
							c->name.texture.getSize().x,
							c->name.texture.getSize().y
						});
						c->name.rect.width = c->name.texture.getSize().x;
						c->name.rect.height = c->name.texture.getSize().y;
						c->name.tint = SokuLib::Color{0xFF, 0x80, 0x00};
						SokuLib::playSEWaveBuffer(23);
						std::cerr << "Broadcast: " << msg << std::endl;
					};
					connection->c->startThread();
				}

				auto info = connection->c->getLobbyInfo();

				if (connection->lastName != info.name) {
					connection->lastName = info.name;
					connection->name.texture.createFromText(connection->lastName.c_str(), this->_defaultFont16, {300, 74});
					connection->name.setSize({
						connection->name.texture.getSize().x,
						connection->name.texture.getSize().y
					});
					connection->name.rect.width = connection->name.texture.getSize().x;
					connection->name.rect.height = connection->name.texture.getSize().y;
					connection->name.tint = SokuLib::Color::White;
				}
				if (
					connection->lastPlayerCount.first != info.currentPlayers ||
					connection->lastPlayerCount.second != info.maxPlayers
				) {
					SokuLib::Vector2i size;

					connection->lastPlayerCount = {info.currentPlayers, info.maxPlayers};
					connection->playerCount.texture.createFromText((std::to_string(info.currentPlayers) + "/" + std::to_string(info.maxPlayers)).c_str(), this->_defaultFont16, {300, 74}, &size);
					connection->playerCount.setSize(size.to<unsigned>());
					connection->playerCount.rect.width = connection->playerCount.getSize().x;
					connection->playerCount.rect.height = connection->playerCount.getSize().y;
					connection->playerCount.tint = SokuLib::Color::White;
				}
			} catch (std::exception &e) {
				connection->lastName.clear();
				connection->lastPlayerCount = {0, 0};
				connection->name.texture.createFromText(e.what(), this->_defaultFont16, {300, 74});
				connection->name.setSize({
					connection->name.texture.getSize().x,
					connection->name.texture.getSize().y
				});
				connection->name.rect.width = connection->name.texture.getSize().x;
				connection->name.rect.height = connection->name.texture.getSize().y;
				connection->name.tint = SokuLib::Color::Red;
				connection->playerCount.texture.destroy();
				connection->playerCount.setSize({0, 0});
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		i = (i + 1) % 200;
	}
}