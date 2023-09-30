//
// Created by PinkySmile on 04/11/2020.
//

#include "getPublicIp.hpp"
#include "Exceptions.hpp"
#include "Socket.hpp"
#include "data.hpp"
#include "LobbyData.hpp"
#include <cstdio>

static char *myIp = nullptr;
static char *myIpv6 = nullptr;
static char buffer[64];
static wchar_t buffer2[64];

const char *getMyIp()
{
	GetPrivateProfileStringW(L"Lobby", L"HostIP", L"", buffer2, sizeof(buffer2) / sizeof(*buffer2), profilePath);
	if (*buffer2) {
		for (int i = 0; i < sizeof(buffer); i++)
			buffer[i] = buffer2[i];
		printf("Using forced ip %s\n", buffer);
		return buffer;
	}
	if (myIp)
		return myIp;

	puts("Fetching public IP");
	try {
		Socket sock;
		Socket::HttpRequest request{
			/*.httpVer*/ "HTTP/1.1",
			/*.body   */ "",
			/*.method */ "GET",
			/*.host   */ "www.sfml-dev.org",
			/*.portno */ 80,
			/*.header */ {},
			/*.path   */ "/ip-provider.php",
		};
		auto response = sock.makeHttpRequest(request);

		if (response.returnCode != 200)
			throw HTTPErrorException(response);
		myIp = strdup(response.body.c_str());
		printf("My ip is %s\n", myIp);
		return myIp;
	} catch (NetworkException &e) {
		printf("Error: %s\n", e.what());
		throw;
	}
}

const char *getMyIpv6()
{
	if (myIpv6)
		return myIpv6;
	try {
		myIpv6 = strdup(lobbyData->httpRequest("https://api-ipv6.ip.sb/ip", "GET", "", 10000).c_str());
	} catch (std::exception &e) {
		printf("Error when getting ipv6: %s\n", e.what());
		return nullptr;
	}
	int len = strlen(myIpv6);
	if (len > 0 && myIpv6[len-1] == '\n')
		myIpv6[len-1] = '\0';
	printf("My ipv6 is %s\n", myIpv6);
	return myIpv6;
}

bool isIpv6Available() {
	return myIpv6;
}