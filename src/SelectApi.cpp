#include "SelectApi.h"
#include "EventLoop.h"
#include <string.h>
#include <new>
using namespace libnetwork;


SelectApi* SelectApi::create()
{
	return new(std::nothrow) SelectApi();
}

bool SelectApi::apiCreate(EventLoop* eventLoop)
{
	FD_ZERO(&_rfdsBackup);
	FD_ZERO(&_wfdsBackup);
	return true;
}

bool SelectApi::apiAddEvent(EventLoop* eventLoop, int fd, int mask)
{
	if (mask & LN_READABLE) FD_SET(fd, &_rfdsBackup);
	if (mask & LN_WRITABLE) FD_SET(fd, &_wfdsBackup);
	return true;
}

bool SelectApi::apiDelEvent(EventLoop* eventLoop, int fd, int delMask)
{
	if (delMask & LN_READABLE) FD_CLR(fd, &_rfdsBackup);
	if (delMask & LN_WRITABLE) FD_CLR(fd, &_wfdsBackup);
	return true;
}

int SelectApi::apiPoll(EventLoop* eventLoop, struct timeval* tvp)
{
	memcpy(&_rfds, &_rfdsBackup, sizeof(fd_set));
	memcpy(&_wfds, &_wfdsBackup, sizeof(fd_set));

	int retval, j, numevents = 0;
	retval = select(eventLoop->maxfd + 1,
		&_rfds, &_wfds, NULL, tvp);
	if (retval > 0) {
		for (j = 0; j <= eventLoop->maxfd; j++) {
			int mask = 0;
			FileEvent* fe = &eventLoop->events[j];

			if (fe->mask == LN_NONE) continue;
			if (fe->mask & LN_READABLE && FD_ISSET(j, &_rfds))
				mask |= LN_READABLE;
			if (fe->mask & LN_WRITABLE && FD_ISSET(j, &_wfds))
				mask |= LN_WRITABLE;
			eventLoop->fired[numevents].fd = j;
			eventLoop->fired[numevents].mask = mask;
			numevents++;
		}
	}
	return numevents;
}

bool SelectApi::apiResize(EventLoop* eventLoop, int size)
{
	if (size <= FD_SETSIZE) return true;
	return false;
}

const char* SelectApi::apiName()
{
	return "select";
}

SelectApi::SelectApi()
{

}

SelectApi::~SelectApi()
{

}