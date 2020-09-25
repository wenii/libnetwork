#include "Connection.h"
#include <new>
#include "RingBuffer.h"
#include <algorithm>
#include "Socket.h"
#include <assert.h>
#include <string.h>
#include "EventLoop.h"
#include "Log.h"
using namespace libnetwork;

// 生成ConnectID
static ConnID genConnectID()
{
	static ConnID ID = 0;
	while (!ID++);
	return ID;
}

Connection* Connection::ObjectPool::create()
{
	Connection* conn = nullptr;
	if (_conns)
	{
		conn = _conns;
		_conns = _conns->next;
		_size--;

		conn->_connectID = genConnectID();
	}
	else
	{
		conn =  Connection::create();
	}
	return conn;
}

void Connection::ObjectPool::free(Connection* conn, bool destroy)
{
	if (conn && conn != _conns && !destroy)
	{
		if (_size < MAX_OBJECT_COUNT)
		{
			conn->clearBuffer();
			conn->setFD(-1);
			conn->setConnectType(CONNECT_TYPE_NONE);
			conn->setState(CONNECT_STATE_NONE);
			conn->setConnectID(0);

			conn->next = _conns;
			_conns = conn;
			_size++;
		}
		else
		{
			conn->destroy();
		}
	}
}

Connection* Connection::ObjectPool::_conns = nullptr;

int Connection::ObjectPool::_size = 0;

Connection* Connection::create()
{
	Connection* object = new(std::nothrow) Connection();
	if (object != nullptr)
	{
		RingBuffer* readBuffer = RingBuffer::create(RingBuffer::BUFFER_SIZE);
		RingBuffer* writeBuffer = RingBuffer::create(RingBuffer::BUFFER_SIZE_1M);
		if (readBuffer != nullptr && writeBuffer != nullptr)
		{
			object->_readBuffer = readBuffer;
			object->_writeBuffer = writeBuffer;
		}
		else
		{
			delete object;
			return nullptr;
		}
		
		object->_connectID = genConnectID();
	}
	return object;
}

Connection::Connection()
	: _eventLoop(nullptr)	
	, _connectID(0)
	, _fd(-1)
	, _readBuffer(nullptr)
	, _writeBuffer(nullptr)
	, _state(CONNECT_STATE_NONE)
	, _type(0)
	, _lastTime(0)
	, _onRecvComplete(nullptr)
	, _onDisconnect(nullptr)
	, _target(nullptr)
	, next(nullptr)
{

}

Connection::~Connection()
{

}

void Connection::setEventLoop(EventLoop* eventLoop)
{
	_eventLoop = eventLoop;
}

ConnID Connection::getConnectID()
{
	return _connectID;
}

void Connection::setConnectID(ConnID connID)
{
	_connectID = connID;
}

int Connection::getFD()
{
	return _fd;
}

void Connection::setFD(int fd)
{
	_fd = fd;
}

void Connection::setState(int state)
{
	_state = state;
}

int Connection::getState()
{
	return _state;
}

int Connection::getConnectType()
{
	return _type;
}

void Connection::setConnectType(int type)
{
	_type = type;
}

int Connection::getLastTime()
{
	return _lastTime;
}

void Connection::setLastTime(int time)
{
	_lastTime = time;
}

void Connection::setRecvCompleteCallback(RecvCompleteCallback* callback)
{
	_onRecvComplete = callback;
}

void Connection::setDisconnectCallback(DisconnectCallback* callback)
{
	_onDisconnect = callback;
}

void Connection::setTarget(void* target)
{
	_target = target;
}

void Connection::established()
{
	assert(_state == CONNECT_STATE_NONE);
	_state = CONNECT_STATE_ESTABLISHED;
}

void Connection::close()
{
	Log::info("Connection::close() close socket:%d connectID:%lld", _fd, _connectID);

	Socket::close(_fd);

	_eventLoop->deleteFileEvent(getFD(), LN_READABLE | LN_WRITABLE);

	_state = CONNECT_STATE_CLOSED;

	// 通知连接断开
	_onDisconnect(this, _target);
}

int Connection::readFromTcpBuffer()
{
	if (_state != CONNECT_STATE_ESTABLISHED)
		return 0;

	int totalReadSize = 0;
	for (int tailSize = _readBuffer->getTailWritableSize(); tailSize > 0; tailSize = _readBuffer->getTailWritableSize())
	{
		char* tail = _readBuffer->getTail();
		int recvSize = tailSize;
		if (Socket::recv(_fd, tail, &recvSize))
		{
			totalReadSize += recvSize;
			_readBuffer->setWriteOffset(recvSize);
			if (recvSize < tailSize)
			{
				break;
			}
		}
		else
		{
			close();
			break;
		}
	}

	return totalReadSize;
}

int Connection::writeToTcpBuffer()
{
	if (_state != CONNECT_STATE_ESTABLISHED)
		return 0;

	int totalWriteSize = 0;
	for (int headSize = _writeBuffer->getReadableSize(); headSize > 0; headSize = _writeBuffer->getReadableSize())
	{
		const char* head = _writeBuffer->getHead();
		int sendSize = headSize;
		if (Socket::send(_fd, head, &sendSize))
		{
			totalWriteSize += sendSize;
			_writeBuffer->setReadOffset(sendSize);
			if (sendSize < headSize)
			{
				break;
			}
		}
		else
		{
			close();
			break;
		}
	}

	return totalWriteSize;
}

RingBuffer* Connection::getReadBuffer()
{
	return _readBuffer;
}

RingBuffer* Connection::getWriteBuffer()
{
	return _writeBuffer;
}

bool Connection::enableRead()
{
	return _eventLoop->createFileEvent(_fd, LN_READABLE, readHandler, this);
}

void Connection::send(const char* buf, int size)
{
	if (!buf || !size)
		return;
	if (_state == CONNECT_STATE_CLOSED)
		return;

	const bool isBufferEmpty = _writeBuffer->isEmpty();

	
	if (!isBufferEmpty) 
	{
		// 将之前没有写完的写完
		writeToTcpBuffer();
	}
	int sendSize = size;
	if (_writeBuffer->isEmpty()) 
	{
		// 先往tcp buf里面写
		if (!Socket::send(_fd, buf, &sendSize))
		{
			Log::info("Connection::send error.");
			close();
			return;
		}
	}
	else
	{
		sendSize = 0;
	}
	if (sendSize < size)
	{
		// 剩下的写到缓存
		int leftSize = size - sendSize;
		if (_writeBuffer->getWritableSize() >= leftSize)
		{
			// 写入到应用层缓存
			int writeSize = _writeBuffer->write(buf + sendSize, leftSize);
			assert(writeSize == leftSize);
		}
		else
		{
			Log::info("Connection::send buf is full, not ehough space to write. so disconnect this connect.");

			// buffer已写满，该连接可能出现异常，关闭连接
			close();
		}
	}
	else
	{
		// 全部写完
	}
}

void Connection::readHandler(int fd, void* clientData)
{
	Connection* self = (Connection*)clientData;

	self->readFromTcpBuffer();

	if (self->_state == CONNECT_STATE_ESTABLISHED)
	{
		// 通知读取数据
		self->_onRecvComplete(self, self->_target);
	}
}

void Connection::writeHandler(int fd, void* clientData)
{
	Connection* self = (Connection*)clientData;

	self->writeToTcpBuffer();

	if (self->_state == CONNECT_STATE_ESTABLISHED && self->_writeBuffer->isEmpty())
	{
		// 删除写事件
		self->_eventLoop->deleteFileEvent(self->getFD(), LN_WRITABLE);
	}
}

void Connection::clearBuffer()
{
	if (_readBuffer)
		_readBuffer->clear();

	if (_writeBuffer)
		_writeBuffer->clear();
}

ConnectionDict* ConnectionDict::create(int size)
{
	return new(std::nothrow) ConnectionDict(size);
}

// 销毁对象
void ConnectionDict::destroy()
{
	delete this;
}

ConnectionDict::ConnectionDict(int size)
	: _conns(nullptr)
	, _size(size)
	, _used(0)

{
	_conns = new(std::nothrow) Connection * [size];
	assert(_conns);
	memset(_conns, 0, size * sizeof(Connection*));
}

ConnectionDict::~ConnectionDict()
{
	if (_conns != nullptr)
	{
		for (int i = 0; i < _size; ++i)
		{
			Connection* conn = _conns[i];
			while (conn != nullptr)
			{
				Connection* next = conn->next;
				Connection::ObjectPool::free(conn, true);
				conn = next;
			}
		}
	}
}

void ConnectionDict::saveConnection(Connection* conn)
{
	const ConnID connID = conn->getConnectID();
	const int index = connID % _size;

	conn->next = _conns[index];
	_conns[index] = conn;

	_used++;

	if (_used / _size > RESIZE_RATIO)
	{
		dictExpand();
	}
}

void ConnectionDict::removeConnection(Connection* conn)
{
	const int index = conn->getConnectID() % _size;
	Connection* head = _conns[index];
	Connection* prev = nullptr;
	while (head)
	{
		Connection* next = head->next;
		if (conn == head)
		{
			if (prev != nullptr)
				prev->next = next;
			else
				_conns[index] = next;
			_used--;
			break;
		}
		prev = head;
		head = next;
	}
}

Connection* ConnectionDict::findConnectionByID(ConnID connID)
{
	const int index = connID % _size;
	Connection* head = _conns[index];
	while (head)
	{
		if (head->getConnectID() == connID)
		{
			return head;
		}
		head = head->next;
	}
	return nullptr;
}

int ConnectionDict::getConnectionCount()
{
	return _used;
}

int ConnectionDict::getSize()
{
	return _size;
}

Connection* ConnectionDict::getConnectionByIndex(unsigned int index)
{
	return _conns[index % _size];
}

void ConnectionDict::dictExpand()
{
	const int rehashSize = _used * 2;
	Connection** rehashConns = new(std::nothrow) Connection * [rehashSize];
	if (rehashConns != nullptr)
	{
		memset(rehashConns, 0, rehashSize * sizeof(Connection*));
		for (int i = 0; i < _size; ++i)
		{
			Connection* head = _conns[i];
			while (head)
			{
				Connection* next = head->next;
				const int newIndex = head->getConnectID() % rehashSize;
				head->next = rehashConns[newIndex];
				rehashConns[newIndex] = head;
				head = next;
			}
		}
		delete[] _conns;
		_size = rehashSize;
		_conns = rehashConns;
	}
}
