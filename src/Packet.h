#ifndef __LIB_NETWORK_PACKET_H__
#define __LIB_NETWORK_PACKET_H__
#include <string>
namespace libnetwork
{
	class Packet
	{
		// 协议格式
		/*
		+--------+------------+------------+-----------+
		| 包大小 |协议名称长度|  协议名称  | 协议数据  |
		+--------+------------+------------+-----------+
		| 4字节  |   2字节	  |	大小不固定 |大小不固定 |
		+--------+------------+------------+-----------+
		*/

		static const int PACKET_SIZE_BYTES = 4;										// 包大小占4字节
		static const int PROTO_NAME_BYTES = 2;										// 协议类型占2字节
		static const int PACKET_HEAD_SIZE = PACKET_SIZE_BYTES + PROTO_NAME_BYTES;	// 包头大小

	public:
		Packet();

		// 解析包大小
		static int parsePacketSize(const char* buf);

		// 验证包大小是否合法
		static bool isPacketLegal(int size);

		// 解包
		void Unpacking(const char* buf, int size);

		// 打包
		void Packing(const std::string& protoName, const std::string& protoData);

	public:
		// 包大小
		int getSize() const;

		// 协议名称
		const std::string& getProtoName() const;

		// 协议数据
		const std::string& getProtoData() const;

		// 协议数据
		const std::string& getData() const;

	private:
		int _size;					// 包大小
		std::string _protoName;		// 协议名称
		std::string _protoData;		// 协议数据
		std::string _packetData;	// 包数据

	};	// namespace libnetwork
}


#endif // !__LIB_NETWORK_PACKET_H__
