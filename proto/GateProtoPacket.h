#ifndef __GATE_PROTO_PACKET_H__
#define __GATE_PROTO_PACKET_H__
#include <string>
class GateProtoPacket
{
public:
	//          Packet                 GateProtoPacket						RouterProtoPacket
	// Client------------>GateServer---------------------->RouterServer--------------------------->LogicServer
	// Э���ʽ
	/* GateProtoPacket
	+----------------+---------------+-----------------------+
	|  ��ͷ(����С)  |	�ͻ�������ID | 		   data          |
	+----------------+---------------+-----------------------+
	|     2�ֽ�      |	    4�ֽ�    |          Packet       |
	+----------------+---------------+-----------------------+
	*/

	/* RouterProtoPacket
	+----------------+---------------+-----------------------+
	|  ��ͷ(����С)  |	��������ID   | 		   data          |
	+----------------+---------------+-----------------------+
	|     2�ֽ�      |	    4�ֽ�    |    GatePotoPacket     |
	+----------------+---------------+-----------------------+
	*/

	static const int PACKET_SIZE_BYTES = 2;
	static const int PACKET_CLIENT_ID_BYTES = 4;
	static const int PACKET_HEAD_SIZE = PACKET_SIZE_BYTES + PACKET_CLIENT_ID_BYTES;

public:


public:
	// ���
	void unPacking(uint16_t packetSize, const char* body, int bodySize);

	// ���
	void packing(uint32_t clientConnID, const char* body, int bodySize);

	int getSize() const;

	// ��ȡbody
	const char* getBody() const;

	// ��ȡbody��С
	int getBodySize() const;

	// ��ȡconnID
	uint32_t getClientConnID() const;
private:
	uint16_t _size;
	uint32_t _clientConnID;
	const char* _data;
	
};

#endif // !__GATE_PROTO_PACKET_H__
