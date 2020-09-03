#include "IOBuffer.h"
#include <new>
#include <algorithm>
#include <string.h>
using namespace libnetwork;

#define MASK (_size - 1)

static bool comp(int a, int b)
{
	return a < b;
}

IOBuffer* IOBuffer::create(unsigned int size)
{
	if (size > 0)
	{
		IOBuffer* obj = new(std::nothrow) IOBuffer();
		if (obj != nullptr)
		{
			obj->_buf = new(std::nothrow) char[size];
			if (obj->_buf != nullptr)
			{
				obj->_size = size;
				return obj;
			}
			delete obj;
		}
	}
	return nullptr;
}

IOBuffer::IOBuffer()
	: _head(0)
	, _tail(0)
	, _size(0)
	, _buf(nullptr)
{
}

IOBuffer::~IOBuffer()
{
	if (_buf != nullptr)
	{
		delete[] _buf;
		_buf = nullptr;
	}
}

int IOBuffer::getUsedSize()
{
	return (_tail & MASK) - (_head & MASK);
}

int IOBuffer::getWritableSize()
{
	return _size - this->getUsedSize();
}

int IOBuffer::write(const char* buff, int size)
{
	// 缓存可写大小
	size = std::min(size, this->getWritableSize(), comp);

	// 右边可写大小
	const unsigned int len = std::min(size, (int)(_size - (_tail & MASK)), comp);

	// 写右边
	memcpy(_buf + (_tail & MASK), buff, len);
	
	// 写左边
	memcpy(_buf, buff + len, size - len);

	_tail += size;

	return size;
}

const char* IOBuffer::getHead()
{
	return _buf + (MASK & _head);
}

char* IOBuffer::getTail()
{
	return _buf + (MASK & _tail);
}


int IOBuffer::getTailWritableSize()
{
	return std::min(getWritableSize(), (int)(_size - (MASK & _tail)));
}

void IOBuffer::setWriteOffset(int offset)
{
	_tail += offset;
}

int IOBuffer::getReadableSize()
{
	return std::min(this->getUsedSize(), (int)(_size - (_head & MASK)), comp);
}

void IOBuffer::setReadOffset(int offset)
{
	_head += offset;
}

void IOBuffer::alignToLeft(int offset)
{
	memcpy(_buf, _buf + offset, getUsedSize() - offset);
	_tail = _tail - offset;
}

void IOBuffer::clear()
{
	_head = 0;
	_tail = 0;
}