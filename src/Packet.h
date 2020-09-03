#ifndef __LIB_NETWORK_PACKET_H__
#define __LIB_NETWORK_PACKET_H__
#include <string>
namespace libnetwork
{
	class Packet
	{
		// Э���ʽ
		/*
		+--------+------------+------------+-----------+
		| ����С |Э�����Ƴ���|  Э������  | Э������  |
		+--------+------------+------------+-----------+
		| 4�ֽ�  |   2�ֽ�	  |	��С���̶� |��С���̶� |
		+--------+------------+------------+-----------+
		*/

		static const int PACKET_SIZE_BYTES = 4;										// ����Сռ4�ֽ�
		static const int PROTO_NAME_BYTES = 2;										// Э������ռ2�ֽ�
		static const int PACKET_HEAD_SIZE = PACKET_SIZE_BYTES + PROTO_NAME_BYTES;	// ��ͷ��С

	public:
		Packet();

		// ��������С
		static int parsePacketSize(const char* buf);

		// ��֤����С�Ƿ�Ϸ�
		static bool isPacketLegal(int size);

		// ���
		void Unpacking(const char* buf, int size);

		// ���
		void Packing(const std::string& protoName, const std::string& protoData);

	public:
		// ����С
		int getSize() const;

		// Э������
		const std::string& getProtoName() const;

		// Э������
		const std::string& getProtoData() const;

		// Э������
		const std::string& getData() const;

	private:
		int _size;					// ����С
		std::string _protoName;		// Э������
		std::string _protoData;		// Э������
		std::string _packetData;	// ������

	};	// namespace libnetwork
}


#endif // !__LIB_NETWORK_PACKET_H__
