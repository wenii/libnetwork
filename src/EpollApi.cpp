#include "EpollApi.h"
#include "Log.h"
#include "EventLoop.h"
#include <new>
#include <unistd.h>
#include <time.h>
#include <sys/epoll.h>
using namespace libnetwork;

EpollApi* EpollApi::create()
{
	return new(std::nothrow) EpollApi();
}

EpollApi::EpollApi()
	: _epfd(-1)
	, _events(nullptr)
{}

EpollApi::~EpollApi()
{
	if (_epfd != -1)
	{
		::close(_epfd);
		_epfd = -1;
	}
	if (_events != nullptr)
	{
		delete[] _events;
		_events = nullptr;
	}
}

bool EpollApi::apiCreate(EventLoop* eventLoop)
{
	_events = new(std::nothrow) struct epoll_event[eventLoop->setsize];
	if (_events == nullptr) return false;
	
	_epfd = epoll_create(1024);

	if (_epfd == -1)
	{
		Log::error("epoll_create failed.");
		delete[] _events;
		return false;
	}

	return true;
}

bool EpollApi::apiAddEvent(EventLoop* eventLoop, int fd, int mask)
{
	struct epoll_event ee = { 0 }; 
	int op = eventLoop->events[fd].mask == LN_NONE ?
		EPOLL_CTL_ADD : EPOLL_CTL_MOD;

	ee.events = 0;
	mask |= eventLoop->events[fd].mask; 
	if (mask & LN_READABLE) ee.events |= EPOLLIN;
	if (mask & LN_WRITABLE) ee.events |= EPOLLOUT;
	ee.data.fd = fd;

	if (epoll_ctl(_epfd, op, fd, &ee) == -1) return false;
	
	return true;
}

bool EpollApi::apiDelEvent(EventLoop* eventLoop, int fd, int delMask)
{
	struct epoll_event ee = { 0 }; 
	int mask = eventLoop->events[fd].mask & (~delMask);

	ee.events = 0;
	if (mask & LN_READABLE) ee.events |= EPOLLIN;
	if (mask & LN_WRITABLE) ee.events |= EPOLLOUT;
	ee.data.fd = fd;
	if (mask != LN_NONE) {
		epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ee);
	}
	else {
		/* Note, Kernel < 2.6.9 requires a non null event pointer even for
		 * EPOLL_CTL_DEL. */
		epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &ee);
	}
	return true;
}

int EpollApi::apiPoll(EventLoop* eventLoop, struct timeval* tvp)
{
	int retval, numevents = 0;
	retval = epoll_wait(_epfd, _events, eventLoop->setsize,
		tvp ? (tvp->tv_sec * 1000 + tvp->tv_usec / 1000) : -1);
	if (retval > 0) {
		numevents = retval;
		for (int j = 0; j < numevents; j++) {
			int mask = 0;
			struct epoll_event* e = _events + j;

			if (e->events & EPOLLIN) mask |= LN_READABLE;
			if (e->events & EPOLLOUT) mask |= LN_WRITABLE;
			if (e->events & EPOLLERR) mask |= LN_WRITABLE;
			if (e->events & EPOLLHUP) mask |= LN_WRITABLE;
			eventLoop->fired[j].fd = e->data.fd;
			eventLoop->fired[j].mask = mask;
		}
	}
	return numevents;
}

bool EpollApi::apiResize(EventLoop* eventLoop, int size)
{
	struct epoll_event* events = new(std::nothrow) struct epoll_event[size];
	if (events != nullptr)
	{
		delete _events;
		_events = events;
		return true;
	}
	return false;
}

const char* EpollApi::apiName()
{
	return "epoll";
}