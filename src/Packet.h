#ifndef __LIB_NETWORK_PACKET_H__
#define __LIB_NETWORK_PACKET_H__
#include <string>
namespace libnetwork
{
	class Packet
	{
	public:
		// Э���ʽ
		/*
		+--------+-----------------------------------+
		|  ��ͷ  |			   ����                  |  
		+--------+-----------------------------------+
		| 2�ֽ�  |			��С���̶�               |
		+--------+-----------------------------------+
		*/

		static const int PACKET_SIZE_BYTES = 2;										// ����Сռ2�ֽ�
		static const int PACKET_HEAD_SIZE = PACKET_SIZE_BYTES;						// ��ͷ��С
		static const int PACKET_MAX_SIZE = 65535;									// ������С
			
	public:
		Packet();

		// ���
		bool Unpacking(const char* buf, int size);

		// ���
		void Packing(const char* data, int size);

	public:
		// ����С
		int getSize() const;

		// ��ȡbuffer
		const char* getBuffer() const;

		// ��ȡbody
		const char* getBody() const;
		
		// �����С
		int getBodySize() const;

	private:
		uint16_t _size;				// ����С
		const char* _buffer;		// ����ַ

	};	// namespace libnetwork
}


#endif // !__LIB_NETWORK_PACKET_H__
