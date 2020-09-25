#include "TcpServer.h"
#include "Log.h"
#include "Socket.h"
#include "EventLoop.h"
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <cerrno>
#include <stdlib.h>
#include "Connection.h"
#include "RingBuffer.h"
#include <assert.h>
#include <new>
#include "Timer.h"
#include "TimingWheel.h"
#include <functional>
#include <signal.h>
#include <execinfo.h>
#include <chrono>
#include <ostream>
#include <fstream>
#include "Packet.h"
using namespace libnetwork;


int TcpServer::secondTime = 0;
long long TcpServer::millisTime = 0;

static const char* CONFIG_FILED_BIND = "bind";
static const char* CONFIG_FILED_PORT = "port";
static const char* CONFIG_FILED_BACKLOG = "backlog";
static const char* CONFIG_FILED_HZ = "hz";
static const char* CONFIG_FILED_TIMEOUT = "timeout";

// 微秒
static long long ustime() {
	struct timeval tv;
	long long ust;

	gettimeofday(&tv, NULL);
	ust = ((long long)tv.tv_sec) * 1000000;
	ust += tv.tv_usec;
	return ust;
}

// 毫秒
static long long mstime() {
	return ustime() / 1000;
}

// 秒
static int sectime()
{
	return time(nullptr);
}

TcpServer::TcpServer()
	: _eventLoop(nullptr)
	, _bindaAddrCount(0)
	, _ipFDCount(0)
	, _port(LISTEN_PORT_DEFAULT)
	, _backLog(TCP_BACK_LOG_DEFAULT)
	, _hz(SEVER_RUN_HZ_DEFAULT)
	, _timeout(CONNECT_TIMEOUT_DEFAULT)
	, _cronLoops(0)
	, _recvBytes(0)
	, _sendBytes(0)
	, _conns(nullptr)
	, _connsToRemoveHead(nullptr)
	, _checkClientAliveIndex(0)
	, _timingWheel(nullptr)
{
	_eventLoop = EventLoop::create(CLIENT_LIST_COUNT);

	for (int i = 0; i < BINDADDR_MAX; ++i)
	{
		_bindAddr[i] = nullptr;
		_ipFD[i] = -1;
	}

	_eventLoop->beforesleep = beforeSleepHandler;
	_eventLoop->clientData = this;

	secondTime = sectime();
	millisTime = mstime();

	_conns = ConnectionDict::create(CLIENT_LIST_COUNT);

	_timingWheel = TimingWheel::create();
}

TcpServer::~TcpServer()
{
	if(_eventLoop) _eventLoop->destory();
	for (int i = 0; i < _bindaAddrCount; ++i)
	{
		char* addr = _bindAddr[i];
		if (addr != nullptr)
			delete [] addr;
	}

	Connection* head = _connsToRemoveHead;
	while (head)
	{
		Connection* next = head->next;
		Connection::ObjectPool::free(head, true);
		head = next;
	}

	if(_timingWheel)
	{
		_timingWheel->destory();
	}
}

void TcpServer::start()
{
	signal(SIGPIPE, SIG_IGN);			// 忽略SIGPIPE消息

	_eventLoop->addSignal(SIGINT);		// 停止
	_eventLoop->createSignalEvent(TcpServer::signalHandler);

	// 加载配置
	loadConfig("server.cfg");

	// 监听端口
	if (!listen()) exit(0);

	// 创建接收客户端连接事件
	if (!createAcceptEvent()) exit(0);

	// 创建时间事件, 初始时间设定为1，在第一次运行即执行，后面以固定频率执行
	_eventLoop->createTimeEven(1, serverCron, this, nullptr);

	// 创建时间事件，以1s时间间隔运行,驱动定时器
	_eventLoop->createTimeEven(1000, timerHandler, this, nullptr);

	// 调用初始化函数
	if (!onInit()) exit(0);

	// 循环事件
	_eventLoop->run();
}

ConnID TcpServer::connect(const char* host, const char* port)
{
	int sockfd = Socket::connect(host, port);
	if (sockfd != -1) 
	{
		Log::info("TcpServer::connect:connect to server:%s fd:%d success.", host, sockfd);

		Connection* serverConn = Connection::ObjectPool::create();
		serverConn->setFD(sockfd);
		serverConn->setEventLoop(_eventLoop);
		if (serverConn->enableRead())
		{
			serverConn->setTarget(this);
			serverConn->setRecvCompleteCallback(TcpServer::recvCompleteHandler);
			serverConn->setDisconnectCallback(TcpServer::disconnectHandler);
			serverConn->setConnectType(Connection::CONNECT_TYPE_SERVER);
			serverConn->setLastTime(TcpServer::getSecondTime());
			serverConn->established();

			_conns->saveConnection(serverConn);
			return serverConn->getConnectID();
		}
		else
		{
			Log::info("TcpServer::connect:new server connect while enable read, so close.:%lld fd:%d ip:%d", serverConn->getConnectID(), sockfd, host);

			serverConn->close();
			Connection::ObjectPool::free(serverConn);
		}
	}
	return INVALID_CONN;
}

ConnID TcpServer::connect(const char* hostPair)
{
	std::string addr = hostPair;
	std::string::size_type pos = addr.find_last_of(':');
	if (pos != std::string::npos) 
	{
		std::string host = addr.substr(0, pos);
		std::string port = addr.substr(pos + 1);
		return connect(host.c_str(), port.c_str());
	}
}

bool TcpServer::onInit()
{
	return true;
}

int TcpServer::onRecv(ConnID connID, const char* buf, int size)
{
	Log::info("waning the function TcpServer::onRecv() should be overwrite.");
	int handleSize = 0;
	while (size > Packet::PACKET_HEAD_SIZE)
	{
		Packet packet;
		if (packet.Unpacking(buf, size))
		{
			const int packetSize = packet.getSize();
			if (packetSize <= size)
			{
				onPacket(connID, packet);
				handleSize += packetSize;
				size -= packetSize;
				buf += handleSize;
			}
			else
			{
				break;
			}
		}
		else
		{
			Log::info("GameServer::onRecv Unpacking failed. connID:%lld", connID);
			// 主动关闭连接
			disconnect(connID);
			break;
		}
	}
	return handleSize;
}

int TcpServer::onRecvFromServer(ConnID connID, const char* buf, int size)
{
	Log::info("waning the function TcpServer::onRecv() should be overwrite.");
	int handleSize = 0;
	while (size > Packet::PACKET_HEAD_SIZE)
	{
		Packet packet;
		if (packet.Unpacking(buf, size))
		{
			const int packetSize = packet.getSize();
			if (packetSize <= size)
			{
				onPacketFromServer(connID, packet);
				handleSize += packetSize;
				size -= packetSize;
				buf += handleSize;
			}
			else
			{
				break;
			}
		}
		else
		{
			Log::info("GameServer::onRecv Unpacking failed. connID:%lld", connID);
			// 主动关闭连接
			disconnect(connID);
			break;
		}
	}
	return handleSize;
}

void TcpServer::onPacket(ConnID connID, const Packet& packet)
{

}

void TcpServer::onPacketFromServer(ConnID connID, const Packet& packet)
{

}

void TcpServer::onAccept(ConnID connID)
{
	Log::info("warning the function TcpServer::onAccept() shuld be overwrite.");
}

void TcpServer::onDisconnect(ConnID connID)
{
	Log::info("warning the function TcpServer::onDisconnect() shuld be overwrite.");
}

void TcpServer::update(int dt)
{
	Log::info("warning the function TcpServer::update() shuld be overwrite.");
}

void TcpServer::onExit()
{
	Log::info("warning the function TcpServer::onExit() shuld be overwrite.");
}

void TcpServer::send(ConnID connID, const char* buffer, int size)
{
	Connection* conn = _conns->findConnectionByID(connID);
	if (conn != nullptr)
	{
		conn->send(buffer, size);
		_sendBytes += size;
	}
}

void TcpServer::disconnect(ConnID connID)
{
	Connection* conn = _conns->findConnectionByID(connID);
	if (conn != nullptr)
	{
		conn->close();
	}
}

void TcpServer::addTimer(Timer* timer)
{
	_timingWheel->addTimer(timer);

	Log::info("add a timer, will exe after:%d", timer->getInterval());
}

int TcpServer::getSecondTime()
{
	return TcpServer::secondTime;
}

long long TcpServer::getMillisTiime()
{
	return TcpServer::millisTime;
}

void TcpServer::acceptHandler(int listenFD, void* clientData)
{
	TcpServer* self = (TcpServer*)clientData;

	char clientAddr[64] = { 0 };
	const int clientFD = Socket::accept(listenFD, clientAddr, 64);
	if (clientFD != -1)
	{
		Connection* clientConn = Connection::ObjectPool::create();
		clientConn->setFD(clientFD);
		clientConn->setEventLoop(self->_eventLoop);
		if (clientConn->enableRead())
		{
			Log::info("TcpServer::acceptHandler:new client:%lld fd:%d ip:%s", clientConn->getConnectID(), clientFD, clientAddr);

			clientConn->setTarget(self);
			clientConn->setRecvCompleteCallback(TcpServer::recvCompleteHandler);
			clientConn->setDisconnectCallback(TcpServer::disconnectHandler);
			clientConn->setConnectType(Connection::CONNECT_TYPE_CLIENT);
			clientConn->setLastTime(TcpServer::getSecondTime());
			clientConn->established();

			self->_conns->saveConnection(clientConn);
			self->onAccept(clientConn->getConnectID());
		}
		else
		{
			Log::info("TcpServer::acceptHandler:new client failed while enable read, so close.:%lld fd:%d ip:%d", clientConn->getConnectID(), clientFD, clientAddr);

			clientConn->close();
			Connection::ObjectPool::free(clientConn);
		}
	}
}

void TcpServer::recvCompleteHandler(Connection* conn, void* target)
{
	TcpServer* self = (TcpServer*)target;

	// 通过onRecv接口传递数据到上层
	RingBuffer* readBuffer = conn->getReadBuffer();
	const int readableSize = readBuffer->getReadableSize();
	if (readableSize > 0)
	{
		int size = 0;
		if (conn->getConnectType() == Connection::CONNECT_TYPE_CLIENT)
		{
			size = self->onRecv(conn->getConnectID(), readBuffer->getHead(), readableSize);
		}
		else if (conn->getConnectType() == Connection::CONNECT_TYPE_SERVER)
		{
			size = self->onRecvFromServer(conn->getConnectID(), readBuffer->getHead(), readableSize);
		}
		else
		{
			assert(false);
		}
		assert(size <= readableSize);

		self->_recvBytes += size;

		readBuffer->alignToLeft(size);
	}

	conn->setLastTime(TcpServer::getSecondTime());
}

void TcpServer::disconnectHandler(Connection* conn, void* target)
{
	TcpServer* self = (TcpServer*)target;

	// 从活动客户端数组中移除客户端连接
	self->_conns->removeConnection(conn);

	// 添加已关闭连接到待移除链表中
	conn->next = self->_connsToRemoveHead;
	self->_connsToRemoveHead = conn;
}

int TcpServer::serverCron(long long id, void* clientData)
{
	TcpServer* self = (TcpServer*)clientData;

	int lasteTime = TcpServer::millisTime;

	// 更新缓存时间
	self->updateCacheTime();

	// 清理关闭的客户端连接
	self->clearDisconnect();

	// 更新
	int dt = TcpServer::millisTime - lasteTime;
	self->update(dt);

	self->_cronLoops++;

	return 1000 / self->_hz;
}

int TcpServer::timerHandler(long long id, void* clientData)
{
	TcpServer* self = (TcpServer*)clientData;
	self->_timingWheel->tick();
	self->_timingWheel->processTimerEvent();

	return 1000;			// 间隔1秒被调用
}

void TcpServer::beforeSleepHandler(void* clientData)
{
	TcpServer* self = (TcpServer*)clientData;

	// 检查是否需要扩容
	if (self->_conns->getConnectionCount() >= self->_eventLoop->setsize)
	{
		self->_eventLoop->resize();
	}

	// 检查客户端连接超时
	if (self->_timeout)
	{
		self->checkClientConnectionAlive();
	}
}

void TcpServer::signalHandler(int sig, void* clientData)
{
	Log::info("TcpServer::signalHandler sig:%d", sig);

	TcpServer* self = (TcpServer*)clientData;
	switch (sig)
	{
	case SIGINT:
		self->onExit();
		self->_eventLoop->stop();
		break;
	default:
		Log::info("TcpServer::unkonwn sig:%d", sig);
	}
}

// 判断一个字符串是否可以转换整形
static bool isIntagerString(const char* str)
{
	const int len = strlen(str);
	for (int i = 0; i < len; ++i)
	{
		if (!(str[i] >= '0' && str[i] <= '9'))
			return false;
	}
	return true;
}

void TcpServer::loadConfig(const char* fileName)
{
	FILE* pf = fopen(fileName, "r");
	if (pf == nullptr)
	{
		Log::error("open config file:%s filed.", fileName);
		exit(0);
	}

	int lines = 0;
	char buf[CONFIG_MAX_LINE + 1] = {0};
	while (fgets(buf, CONFIG_MAX_LINE + 1, pf) != NULL)
	{
		if (buf[CONFIG_MAX_LINE] != '\0') goto err;

		lines++;

		if (strchr(buf, '#'))
		{
			memset(buf, 0, CONFIG_MAX_LINE + 1);
			continue;
		}

		if (buf[0] == '\r' || buf[0] == '\n')
		{
			memset(buf, 0, CONFIG_MAX_LINE + 1);
			continue;
		}

		for (int i = 0; i < CONFIG_MAX_LINE + 1; ++i)
		{
			if (buf[i] == ' ' || buf[i] == '\t')
				buf[i] = '\0';
			else if (buf[i] == '\r' || buf[i] == '\n')
			{
				buf[i] = '\0';
				if (i < CONFIG_MAX_LINE)
					buf[i + 1] = '\n';
				break;
			}
		}

		int indexArray[BINDADDR_MAX] = { 0 };
		int recordIndex = 0;
		for (int i = 0; i < CONFIG_MAX_LINE + 1;)
		{
			if (buf[i] == '\n')
			{
				break;
			}
			else if (buf[i] != '\0')
			{
				indexArray[recordIndex++] = i;
				i += strlen(buf + i);
			}
			else
			{
				i++;
			}
		}
		if (recordIndex >= 2)
		{
			const char* filed = buf + indexArray[0];
			if (!strcmp(filed, CONFIG_FILED_BIND))
			{
				for (int j = 1; j < recordIndex; ++j)
				{
					const char* value = buf + indexArray[j];
					const int valueLen = strlen(value);

					char* newValue = new char[valueLen + 1];
					newValue[valueLen] = '\0';
					strcpy(newValue, value);

					_bindAddr[_bindaAddrCount] = newValue;
					_bindaAddrCount += 1;
				}
			}
			else if (!strcmp(filed, CONFIG_FILED_PORT))
			{
				if (recordIndex > 2) goto err;

				const char* value = buf + indexArray[1];
				if (!isIntagerString(value)) goto err;
				
				_port = atoi(value);
			}
			else if (!strcmp(filed, CONFIG_FILED_BACKLOG))
			{
				if (recordIndex > 2) goto err;

				const char* value = buf + indexArray[1];
				if (!isIntagerString(value)) goto err;

				_backLog = atoi(value);
			}
			else if (!strcmp(filed, CONFIG_FILED_HZ))
			{
				if (recordIndex > 2) goto err;

				const char* value = buf + indexArray[1];
				if (!isIntagerString(value)) goto err;

				_hz = atoi(value);
			}
			else if (!strcmp(filed, CONFIG_FILED_TIMEOUT))
			{
				if (recordIndex > 2) goto err;

				const char* value = buf + indexArray[1];
				if (!isIntagerString(value)) goto err;

				_timeout = atoi(value);
			}
			else
			{
				goto err;
			}
		}
		
	
		memset(buf, CONFIG_MAX_LINE + 1, 0);
	}
	fclose(pf);
	return;

err:
	fclose(pf);
	Log::info("server.cfg error in line%d", lines);
	exit(0);
}

bool TcpServer::listen()
{
	char strPort[32] = { 0 };
	snprintf(strPort, 32, "%d", _port);
	for (int i = 0; i < _bindaAddrCount || i == 0; ++i)
	{
		const char* bindAddr = _bindAddr[i];
		if (bindAddr == nullptr)
		{
			// 当未指定绑定地址时，绑定IPv6 0::0 和IPv4 0.0.0.0
			int fd = -1;
			if ((fd = Socket::listenToPortWithIPv6(nullptr, strPort, _backLog)) != -1)
			{
				_ipFD[_ipFDCount++] = fd;
			}
			else if (errno == EAFNOSUPPORT)
			{
				Log::error("IPv6 Not Support.");
			}
			else
			{
				Log::error("ListenToPortWithIPv6 failed.");
			}
		
			if ((fd = Socket::listenToPortWithIPv4(nullptr, strPort, _backLog)) != -1)
			{
				_ipFD[_ipFDCount++] = fd;
			}
			else if (errno == EAFNOSUPPORT)
			{
				Log::error("IPv4 Not Support.");
			}
			else
			{
				Log::error("ListenToPortWithIPv4 failed.");
			}

			if (_ipFDCount == 0)
			{
				Log::info("fatal error! bind addr failed. exit!");
				return false;
			}
			break;
		}
		else if (strchr(_bindAddr[i], ':') != nullptr)
		{
			// 绑定IPv6地址
			_ipFD[_ipFDCount] = Socket::listenToPortWithIPv6(bindAddr, strPort, _backLog);
		}
		else
		{
			// 绑定IPv4地址
			_ipFD[_ipFDCount] = Socket::listenToPortWithIPv4(bindAddr, strPort, _backLog);

		}
		if (_ipFD[_ipFDCount] == -1)
		{
			Log::error("find bind addr:%s", bindAddr);
			return false;
		}
		_ipFDCount++;
	}
	return true;
}

bool TcpServer::createAcceptEvent()
{
	for (int i = 0; i < _ipFDCount; ++i)
	{
		const int fd = _ipFD[i];
		if (!_eventLoop->createFileEvent(fd, LN_READABLE, acceptHandler, this))
		{
			return false;
		}
	}
	return true;
}

void TcpServer::updateCacheTime()
{
	TcpServer::secondTime = sectime();
	TcpServer::millisTime = mstime();
}

bool TcpServer::runWithPeriod(int millisecond)
{
	return (millisecond < 1000 / _hz || !(_cronLoops % ((millisecond) / (1000 / _hz))));
}

void TcpServer::clearDisconnect()
{
	Connection* head = _connsToRemoveHead;
	while (head)
	{
		assert(head->getState() == Connection::CONNECT_STATE_CLOSED);

		// 通知接口断开连接
		onDisconnect(head->getConnectID());

		Connection* next = head->next;
		Connection::ObjectPool::free(head);
		head = next;
	}
	_connsToRemoveHead = nullptr;
}

void TcpServer::checkClientConnectionAlive()
{
	int count = _conns->getSize() / _hz;	// 保证1秒内检查完所有连接
	while (count--)
	{
		Connection* conn = _conns->getConnectionByIndex(_checkClientAliveIndex++);
		while (conn)
		{
			Connection* next = conn->next;
			if (conn->getConnectType() == Connection::CONNECT_TYPE_CLIENT)
			{
				if (TcpServer::getSecondTime() - conn->getLastTime() > _timeout)
				{
					Log::error("checkClientConnectionAlive client[%ld] timeout.", conn->getConnectID());
					conn->close();
				}
			}
			conn = next;
		}
	}
}
