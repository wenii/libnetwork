#ifndef __LIB_NETWORK_PACKET_H__
#define __LIB_NETWORK_PACKET_H__
#include <string>
namespace libnetwork
{
	class Packet
	{
	public:
		// 协议格式
		/*
		+--------+-----------------------------------+
		|  包头  |			   包体                  |  
		+--------+-----------------------------------+
		| 2字节  |			大小不固定               |
		+--------+-----------------------------------+
		*/

		static const int PACKET_SIZE_BYTES = 2;										// 包大小占2字节
		static const int PACKET_HEAD_SIZE = PACKET_SIZE_BYTES;						// 包头大小
		static const int PACKET_MAX_SIZE = 65535;									// 包最大大小
			
	public:
		Packet();

		// 解包
		bool Unpacking(const char* buf, int size);

		// 打包
		void Packing(const char* data, int size);

	public:
		// 包大小
		int getSize() const;

		// 获取buffer
		const char* getBuffer() const;

		// 获取body
		const char* getBody() const;
		
		// 包体大小
		int getBodySize() const;

	private:
		uint16_t _size;				// 包大小
		const char* _buffer;		// 包地址

	};	// namespace libnetwork
}


#endif // !__LIB_NETWORK_PACKET_H__
