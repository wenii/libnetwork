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
#include "IOBuffer.h"
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
using namespace libnetwork;


int TcpServer::secondTime = 0;
long long TcpServer::millisTime = 0;

static const char* CONFIG_FILED_BIND = "bind";
static const char* CONFIG_FILED_PORT = "port";
static const char* CONFIG_FILED_BACKLOG = "backlog";
static const char* CONFIG_FILED_HZ = "hz";
static const char* CONFIG_FILED_TIMEOUT = "timeout";

// ΢��
static long long ustime() {
	struct timeval tv;
	long long ust;

	gettimeofday(&tv, NULL);
	ust = ((long long)tv.tv_sec) * 1000000;
	ust += tv.tv_usec;
	return ust;
}

// ����
static long long mstime() {
	return ustime() / 1000;
}

// ��
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
	signal(SIGPIPE, SIG_IGN);			// ����SIGPIPE��Ϣ

	_eventLoop->addSignal(SIGINT);		// ֹͣ
	_eventLoop->addSignal(SIGABRT);		// ����
	_eventLoop->addSignal(SIGFPE);		// �������
	_eventLoop->addSignal(SIGSEGV);		// �ڴ�δ���
	_eventLoop->createSignalEvent(TcpServer::signalHandler);

	// ��������
	loadConfig("server.cfg");

	// �����˿�
	if (!listen()) exit(0);

	// �������տͻ��������¼�
	if (!createAcceptEvent()) exit(0);

	// ����ʱ���¼�, ��ʼʱ���趨Ϊ1���ڵ�һ�����м�ִ�У������Թ̶�Ƶ��ִ��
	_eventLoop->createTimeEven(1, serverCron, this, nullptr);

	// ����ʱ���¼�����1sʱ��������,������ʱ��
	_eventLoop->createTimeEven(1000, timerHandler, this, nullptr);

	// ѭ���¼�
	_eventLoop->run();
}

int TcpServer::onRecv(ConnID connID, const char* buf, int size)
{
	Log::info("waning the function TcpServer::onRecv() should be overwrite.");
	return 0;
}

void TcpServer::onAccept(ConnID connID)
{
	Log::info("warning the function TcpServer::onAccept() shuld be overwrite.");
}

void TcpServer::onDisconnect(ConnID connID)
{
	Log::info("warning the function TcpServer::onDisconnect() shuld be overwrite.");
}

void TcpServer::update()
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

	// ͨ��onRecv�ӿڴ������ݵ��ϲ�
	IOBuffer* readBuffer = conn->getReadBuffer();
	const int readableSize = readBuffer->getReadableSize();
	if (readableSize > 0)
	{
		const int size = self->onRecv(conn->getConnectID(), readBuffer->getHead(), readableSize);
		assert(size <= readableSize);

		self->_recvBytes += size;

		readBuffer->alignToLeft(size);
	}

	conn->setLastTime(TcpServer::getSecondTime());
}

void TcpServer::disconnectHandler(Connection* conn, void* target)
{
	TcpServer* self = (TcpServer*)target;

	// �ӻ�ͻ����������Ƴ��ͻ�������
	self->_conns->removeConnection(conn);

	// �����ѹر����ӵ����Ƴ�������
	conn->next = self->_connsToRemoveHead;
	self->_connsToRemoveHead = conn;
}

int TcpServer::serverCron(long long id, void* clientData)
{
	TcpServer* self = (TcpServer*)clientData;

	// ���»���ʱ��
	self->updateCacheTime();

	// �����رյĿͻ�������
	self->clearDisconnect();

	// ����
	self->update();

	self->_cronLoops++;

	return 1000 / self->_hz;
}

int TcpServer::timerHandler(long long id, void* clientData)
{
	TcpServer* self = (TcpServer*)clientData;
	self->_timingWheel->tick();
	self->_timingWheel->processTimerEvent();

	return 1000;			// ���1�뱻����
}

void TcpServer::beforeSleepHandler(void* clientData)
{
	TcpServer* self = (TcpServer*)clientData;

	// ����Ƿ���Ҫ����
	if (self->_conns->getConnectionCount() >= self->_eventLoop->setsize)
	{
		self->_eventLoop->resize();
	}

	// ���ͻ������ӳ�ʱ
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
		self->_eventLoop->stop = true;
		break;
	case SIGABRT:
	case SIGFPE:
	case SIGSEGV:
		self->fatalErrorHandler(sig);
		break;
	default:
		Log::info("TcpServer::unkonwn sig:%d", sig);
	}
}

// �ж�һ���ַ����Ƿ����ת������
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
			// ��δָ���󶨵�ַʱ����IPv6 0::0 ��IPv4 0.0.0.0
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
			// ��IPv6��ַ
			_ipFD[_ipFDCount] = Socket::listenToPortWithIPv6(bindAddr, strPort, _backLog);
		}
		else
		{
			// ��IPv4��ַ
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

		// ֪ͨ�ӿڶϿ�����
		onDisconnect(head->getConnectID());

		Connection* next = head->next;
		Connection::ObjectPool::free(head);
		head = next;
	}
	_connsToRemoveHead = nullptr;
}

void TcpServer::checkClientConnectionAlive()
{
	int count = _conns->getSize() / _hz;	// ��֤1���ڼ������������
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
					conn->close();
				}
			}
			conn = next;
		}
	}
}


static std::string getCurrentSystemTime()
{
	auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm* ptm = localtime(&tt);
	char date[60] = { 0 };
	sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
		(int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
		(int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
	return std::string(date);
}

void TcpServer::fatalErrorHandler(int sig)
{
	static const char szSigMsg[][256] = {
	"Received	SIGHUP",			// 1	/* Hangup (POSIX).  */
	"Received	SIGINT",			// 2	/* Interrupt (ANSI).  */
	"Received	SIGQUIT",		    // 3	/* Quit (POSIX).  */
	"Received	SIGILL",		    // 4	/* Illegal instruction (ANSI).  */
	"Received	SIGTRAP",		    // 5	/* Trace trap (POSIX).  */
	"Received	SIGABRT",		    // 6	/* Abort (ANSI).  */
	"Received	SIGBUS",		    // 7	/* BUS error (4.2 BSD).  */
	"Received	SIGFPE",		    // 8	/* Floating-point exception (ANSI).  */
	"Received	SIGKILL",		    // 9	/* Kill, unblockable (POSIX).  */
	"Received	SIGUSR1",		    // 10	/* User-defined signal 1 (POSIX).  */
	"Received	SIGSEGV	",		    // 11	/* Segmentation violation (ANSI).  */
	"Received	SIGUSR2",		    // 12	/* User-defined signal 2 (POSIX).  */
	"Received	SIGPIPE",		    // 13	/* Broken pipe (POSIX).  */
	"Received	SIGALRM",		    // 14	/* Alarm clock (POSIX).  */
	"Received	SIGTERM",		    // 15	/* Termination (ANSI).  */
	"Received	SIGSTKFLT",		    // 16	/* Stack fault.  */
	"Received	SIGCHLD",		    // 17	/* Child status has changed (POSIX).  */
	"Received	SIGCONT",		    // 18	/* Continue (POSIX).  */
	"Received	SIGSTOP",		    // 19	/* Stop, unblockable (POSIX).  */
	"Received	SIGTSTP",		    // 20	/* Keyboard stop (POSIX).  */
	"Received	SIGTTIN",		    // 21	/* Background read from tty (POSIX).  */
	"Received	SIGTTOU",		    // 22	/* Background write to tty (POSIX).  */
	"Received	SIGURG",		    // 23	/* Urgent condition on socket (4.2 BSD).  */
	"Received	SIGXCPU",		    // 24	/* CPU limit exceeded (4.2 BSD).  */
	"Received	SIGXFSZ",		    // 25	/* File size limit exceeded (4.2 BSD).  */
	"Received	SIGVTALRM",		    // 26	/* Virtual alarm clock (4.2 BSD).  */
	"Received	SIGPROF",		    // 27	/* Profiling alarm clock (4.2 BSD).  */
	"Received	SIGWINCH",		    // 28	/* Window size change (4.3 BSD, Sun).  */
	"Received	SIGIO",		        // 29	/* I/O now possible (4.2 BSD).  */
	"Received	SIGPWR",		    // 30	/* Power failure restart (System V).  */
	"Received   SIGSYS"				// 31	/* Bad system call.  */ };
	};

	const char* traceFileName = "trace.txt";
	std::ofstream dumpFile;
	dumpFile.open(traceFileName, std::ios::app);
	if (!dumpFile.is_open())
	{
		Log::info("----------------------------------error------------------------------");
		Log::info("----------------ErrorHandle::signalHanle() open file failed--------------");
	}

	void* pTrace[256];
	char** ppszMsg = nullptr;
	size_t uTraceSize = 0;

	do {
		if (0 == (uTraceSize = backtrace(pTrace, sizeof(pTrace) / sizeof(void*)))) {
			break;
		}
		if (nullptr == (ppszMsg = backtrace_symbols(pTrace, uTraceSize))) {
			break;
		}
		std::string curTime = getCurrentSystemTime();
		char infobuffer[1024] = { 0 };
		sprintf(infobuffer, "[%s]%s. call stack:\n", curTime.c_str(), szSigMsg[sig - 1]);
		Log::info(infobuffer);
		printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
		if (dumpFile.is_open())
		{
			dumpFile << infobuffer;
			dumpFile << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
			for (size_t i = 0; i < uTraceSize; ++i) {
				dumpFile << ppszMsg[i] << std::endl;
			}
			dumpFile << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
		}

		for (size_t i = 0; i < uTraceSize; ++i) {
			Log::info("%s", ppszMsg[i]);
		}
		Log::info("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
	} while (0);

	if (nullptr != ppszMsg) {
		free(ppszMsg);
		ppszMsg = nullptr;
	}

	dumpFile.close();

	exit(0);
}
