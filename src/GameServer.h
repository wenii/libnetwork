#ifndef __LIB_NETWORK_GAME_SERVER_H__
#define __LIB_NETWORK_GAME_SERVER_H__
#include "TcpServer.h"
#include <string>
namespace libnetwork
{
	class GameServer : public TcpServer
	{
	private:
		virtual int onRecv(ConnID connID, const char* buf, int size);

	public:
		virtual void onAccept(ConnID connID) = 0;
		virtual void onDisconnect(ConnID connID) = 0;
		virtual void update() = 0;
		virtual void onPacket(const std::string& protoName, const std::string& protoData) = 0;

	public:
		void sendProto(ConnID connID, const std::string& protoName, const std::string& protoData);
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_GAME_SERVER_H__
