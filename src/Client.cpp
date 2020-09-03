#include "Client.h"
#include "EventLoop.h"
#include "Connection.h"
#include <thread>
#include "Socket.h"
#include <string.h>
#include "IOBuffer.h"
#include "Log.h"

using namespace libnetwork;


Client::Client()
	: _eventLoop(nullptr)
	, _serverConn(nullptr)
	, _hz(0)
	, isRuning(false)
{
	memset(_host, 0, HOST_LEN_MAX);
	memset(_port, 0, PORT_LEN_MAX);

	_hz = CLIENT_RUN_HZ_DEFAULT;

	_serverConn = Connection::create(-1);

	_eventLoop = EventLoop::create(16);

	// ����ʱ���¼�, ��ʼʱ���趨Ϊ1���ڵ�һ�����м�ִ�У������Թ̶�Ƶ��ִ��
	_eventLoop->createTimeEven(1, clientCron, this, timeEventFinalizerHandler);
}

Client::~Client()
{
	if (_eventLoop != nullptr)
	{
		_eventLoop->destory();
		_eventLoop = nullptr;
	}
	
	if (_serverConn)
	{
		_serverConn->destroy();
		_serverConn = nullptr;
	}
}

void Client::update()
{

}

void Client::onConnectSuccess()
{
	Log::info("warning the function Client::onConnectSuccess() shuld be overwirte.");
}

void Client::onConnectFailed()
{
	Log::info("warning the function Client::onConnectFailed() shuld be overwirte.");
}

void Client::onRecv(const char* buffer, int size)
{
	Log::info("warning the function Client::onRecv() shuld be overwirte.");
}

void Client::onDisconnect()
{
	Log::info("warning the function Client::onDisconnect() shuld be overwirte.");
}

void Client::connect(const char* host, const char* port)
{
	if (isRuning) return;

	isRuning = true;

	// ��¼�����ַ�˿�
	strcpy(_host, host);
	strcpy(_port, port);

	// �����߳̽���Զ������
	std::thread connThread(&Client::networkHandler, this, _host, _port);
	connThread.detach();
}

// ��������
bool Client::send(const char* buffer, int size)
{
	if (buffer == nullptr || size == 0)
		return false;

	if (_serverConn->getState() != Connection::CONNECT_STATE_ESTABLISHED)
		return false;

	IOBuffer* writeBuffer = _serverConn->getWriteBuffer();
	if (writeBuffer->getWritableSize() < size)
	{
		// buffer��д���������ӿ��ܳ����쳣���ر�����
		_serverConn->setState(Connection::CONNECT_STATE_DISCONNECTED);

		return false;
	}

	// д������
	writeBuffer->write(buffer, size);

	return true;
}

void Client::disconnect()
{
	_serverConn->shutdown();
	_serverConn->setState(Connection::CONNECT_STATE_WAIT_CLOSE);
}

void Client::networkHandler(const char* host, const char* port)
{
	const int fd = Socket::connect(host, port);
	if (fd != -1)
	{
		// ���ӳɹ�
		_serverConn->clearBuffer();
		_serverConn->setFD(fd);
		_serverConn->setState(Connection::CONNECT_STATE_ESTABLISHED);

		// �������ļ��¼�
		_eventLoop->createFileEvent(fd, LN_READABLE, readHandler, this);

		// ֪ͨ�ϲ����ӳɹ�
		onConnectSuccess();

		// ִ��ѭ���¼�
		_eventLoop->stop = false;
		_eventLoop->run();

		onDisconnect();
	}
	else
	{
		// ����ʧ��
		onConnectFailed();
	}
	
	isRuning = false;
}

int Client::clientCron(struct EventLoop* eventLoop, long long id, void* clientData)
{
	Client* self = (Client*)clientData;

	// ���Ͽ�����
	self->checkDisconnect();

	// ���д�¼�
	if (self->_serverConn->getState() == Connection::CONNECT_STATE_ESTABLISHED)
	{
		IOBuffer* writeBuffer = self->_serverConn->getWriteBuffer();
		if (writeBuffer->getUsedSize() > 0)
		{
			self->_eventLoop->createFileEvent(self->_serverConn->getFD(), LN_WRITABLE, writeHandler, self);
		}
	}

	// ����
	self->update();

	return 1000 / self->_hz;
}

void Client::timeEventFinalizerHandler(struct EventLoop* eventLoop, long long id, void* clientData)
{

}

void Client::readHandler(struct EventLoop* eventLoop, int fd, void* clientData, int mask)
{
	Client* self = (Client*)clientData;
	Connection* serverConn = self->_serverConn;
	
	// ��tcp���������
	self->_serverConn->readFromTcpBuffer();

	// ͨ��onRecv�ӿڴ������ݵ��ϲ�
	IOBuffer* readBuffer = self->_serverConn->getReadBuffer();
	while (readBuffer->getReadableSize())
	{
		self->onRecv(readBuffer->getHead(), readBuffer->getReadableSize());
		readBuffer->setReadOffset(readBuffer->getReadableSize());
	}
}

void Client::writeHandler(struct EventLoop* eventLoop, int fd, void* clientData, int mask)
{
	Client* self = (Client*)clientData;
	Connection* serverConn = self->_serverConn;

	// д���ݵ�tcp����
	serverConn->writeToTcpBuffer();

	if (!serverConn->haveDataToSend())
	{
		// ɾ��д�¼�
		self->_eventLoop->deleteFileEvent(serverConn->getFD(), LN_WRITABLE);
	}
}

void Client::checkDisconnect()
{
	if (_serverConn->getState() == Connection::CONNECT_STATE_DISCONNECTED)
	{
		Log::info("checked connection is disconnect.");

		// �Ƴ��¼�
		_eventLoop->deleteFileEvent(_serverConn->getFD(), LN_READABLE | LN_WRITABLE);

		_serverConn->close();
		_serverConn->setState(Connection::CONNECT_STATE_CLOSED);

		// ֹͣ�¼�ѭ��
		_eventLoop->stop = true;
	}
}

