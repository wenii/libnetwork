#include "EventLoop.h"
#include "Log.h"
#include <new>
#include <sys/time.h>
#include "config.h"
#include <unistd.h>
#include <signal.h>
#include <cerrno>

using namespace libnetwork;


// 创建多路复用api接口
static PollerInterface* createApiInterface()
{
#ifdef LIB_NETWORK_USE_EPOLL
	return EpollApi::create();
#else
	#ifdef LIB_NETWORK_USE_SELECT
		return SelectApi::create();
	#endif
#endif // DEBUG
}

// 获取时间
static void getTimeSecAndMill(long* second, long* milliseconds)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	*second = tv.tv_sec;
	*milliseconds = tv.tv_usec / 1000;
}

// 设定时间
static void addMillisecondsToNow(long long milliseconds, long* sec, long* ms) {
	long cur_sec, cur_ms, when_sec, when_ms;

	getTimeSecAndMill(&cur_sec, &cur_ms);
	when_sec = cur_sec + milliseconds / 1000;
	when_ms = cur_ms + milliseconds % 1000;
	if (when_ms >= 1000) {
		when_sec++;
		when_ms -= 1000;
	}
	*sec = when_sec;
	*ms = when_ms;
}


int EventLoop::_pipedes[2] = {0};

EventLoop* EventLoop::create(int size)
{
	EventLoop* object = new(std::nothrow) EventLoop();
	if (object != nullptr)
	{
		object->maxfd = -1;
		object->setsize = size;
		object->lastTime = time(nullptr);
		object->events = new FileEvent[size];
		object->fired = new FiredEvent[size];
		object->pollerApi = createApiInterface();
		
		if (!(object->pollerApi != nullptr && 
			object->pollerApi->apiCreate(object) &&
			pipe(EventLoop::_pipedes) == 0))
		{
			delete object;
			return nullptr;
		}
	}
	return object;
}

FileEvent::FileEvent()
	: mask(LN_NONE)
	, rFileProc(nullptr)
	, wFileProc(nullptr)
	, clientData(nullptr)
{}

TimeEvent::TimeEvent()
	: id(0)
	, when_sec(0)
	, when_ms(0)
	, timeProc(nullptr)
	, finalizerProc(nullptr)
	, clientData(nullptr)
	, next(nullptr)
	, remove(nullptr)
{}

FiredEvent::FiredEvent()
	: fd(-1)
	, mask(0)
{}

EventLoop::EventLoop()
	: maxfd(0)
	, setsize(0)
	, timeEventNextId(0)
	, lastTime(0)
	, events(nullptr)
	, fired(nullptr)
	, timeEventHead(nullptr)
	, _stop(0)
	, beforesleep(nullptr)
	, pollerApi(nullptr)
	, clientData(nullptr)
	, _singalProc(nullptr)
{ 
	
}

EventLoop::~EventLoop()
{
	if(events != nullptr)
	delete [] events;

	if(fired != nullptr)
	delete [] fired;
	
	TimeEvent* head = timeEventHead;
	while (head != nullptr)
	{
		TimeEvent* next = head->next;
		delete head;
		head = next;
	}

	pollerApi->destroy();
}


bool EventLoop::createFileEvent(int fd, int mask, FileProc* proc, void* clientData)
{
	if (fd >= setsize)
		return false;

	FileEvent& fe = events[fd];
	if ((fe.mask & mask) && proc == proc)
		return true;

	if (!pollerApi->apiAddEvent(this, fd, mask))
		return false;

	fe.mask |= mask;
	if (mask & LN_READABLE) fe.rFileProc = proc;
	if (mask & LN_WRITABLE) fe.wFileProc = proc;
	fe.clientData = clientData;
	if (fd > maxfd)
		maxfd = fd;
	return true;
}

bool EventLoop::deleteFileEvent(int fd, int mask)
{
	if (fd >= setsize)
		return false;

	FileEvent& fe = events[fd];
	if (fe.mask == LN_NONE) 
		return false;

	pollerApi->apiDelEvent(this, fd, mask);

	fe.mask = fe.mask & (~mask);
	if (fd == maxfd && fe.mask == LN_NONE) {
		for (int i = maxfd - 1; i >= 0; i--)
		{
			if (events[i].mask != LN_NONE)
			{
				maxfd = i;
				break;
			}
		}
	}
	return true;
}

long long EventLoop::createTimeEven(long long milliseconds, TimeProc* proc, void* clientData, EventFinalizerProc* finalizerProc)
{
	TimeEvent* te = new(std::nothrow) TimeEvent();
	if (te == nullptr) return 0;

	const long long id = timeEventNextId++;
	te->id = id;
	te->timeProc = proc;
	te->finalizerProc = finalizerProc;
	te->clientData = clientData;
	te->next = timeEventHead;
	addMillisecondsToNow(milliseconds, &te->when_sec, &te->when_ms);
	timeEventHead = te;
	
	return id;
}

static void signalHandler(int signal)
{
	int save_errno = errno;
	char sig = signal;
	write(EventLoop::_pipedes[1], &sig, 1);
	Log::info("recv sigal:%d", signal);
	errno = save_errno;
}

void EventLoop::addSignal(int sig)
{
	Log::info("EventLoop::addSignal:%d", sig);
	signal(sig, signalHandler);
}

void EventLoop::createSignalEvent(SignalProc* proc)
{
	_singalProc = proc;
	createFileEvent(EventLoop::_pipedes[0], LN_READABLE, EventLoop::signalDesReadableCallback, this);
}

void EventLoop::run()
{
	while (!_stop)
	{
		if (beforesleep != nullptr)
		{
			beforesleep(clientData);
		}
		processEvents();
	}
	Log::info("EventLoop::run() loop exit.");
}

void EventLoop::stop()
{
	_stop = 1;
}

int EventLoop::processFileEvents(int eventCount)
{
	int processed = 0;
	for (int i = 0; i < eventCount; ++i)
	{
		const int fd = fired[i].fd;
		const int mask = fired[i].mask;

		FileEvent& fe = events[fd];
		if (fe.mask & mask & LN_WRITABLE)
		{
			if (fe.wFileProc)
				fe.wFileProc(fd, fe.clientData);
		}
		if (fe.mask & mask & LN_READABLE)
		{
			if(fe.rFileProc)
				fe.rFileProc(fd, fe.clientData);
		}
		
		processed++;
	}
	return processed;
}

int EventLoop::processTimeEvents()
{
	int processed = 0;
	time_t now = time(NULL);
	TimeEvent* te = nullptr;
	if (now < lastTime) {
		te = timeEventHead;
		while (te) {
			te->when_sec = 0;
			te = te->next;
		}
	}
	lastTime = now;

	TimeEvent* prev = nullptr;
	const long long maxID = timeEventNextId - 1;
	te = timeEventHead;
	while (te != nullptr)
	{
		// 移除需要删除的时间事件
		if (te->remove)
		{
			TimeEvent* next = te->next;
			if (prev == nullptr)
			{
				timeEventHead = next;
			}
			else
			{
				prev->next = next;
			}
			if (te->finalizerProc)
			{
				te->finalizerProc(te->id, te->clientData);
			}
			delete te;
			te = next;
			continue;
		}

		if (te->id > maxID)
		{
			// 防止在处理时间事件的时候添加新的时间事件，这些事件在本次时间事件处理期间不做处理
			te = te->next;
			continue;
		}

		long now_sec, now_ms;
		getTimeSecAndMill(&now_sec, &now_ms);
		if (now_sec > te->when_sec || (now_sec == te->when_sec && now_ms >= te->when_ms))
		{
			// 执行时间事件回调
			const int retval = te->timeProc(te->id, te->clientData);
			if (retval != LN_NOMORE) {
				addMillisecondsToNow(retval, &te->when_sec, &te->when_ms);
			}
			else {
				te->remove = true;
			}
			processed++;
		}
		prev = te;
		te = te->next;
	}
	return processed;
}


void EventLoop::resize()
{
	const int newSize = setsize * 2;
	if (pollerApi->apiResize(this, newSize))
	{
		if (events != nullptr)
			delete[] events;

		if (fired != nullptr)
			delete[] fired;

		events = new FileEvent[newSize];
		fired = new FiredEvent[newSize];
		setsize = newSize;
	}
	else
	{
		Log::error("EventLoop:resize failed.setsize:%d", setsize);
	}
}

void EventLoop::signalDesReadableCallback(int fd, void* clientData)
{
	EventLoop* self = (EventLoop*)clientData;
	char buf[2014] = { 0 };
	int n = ::read(fd, buf, 1024);
	if (self->_singalProc != nullptr)
	{
		for (int i = 0; i < n; ++i)
		{
			int signal = buf[i];
			self->_singalProc(signal, self->clientData);
		}
	}
}

int EventLoop::processEvents()
{
	int processed = 0;

	// 计算等待时间
	struct timeval tv, *ptv;
	ptv = &tv;
	calcPollWaitTime(&ptv);

	// 等待文件事件到来
	const int eventCount = pollerApi->apiPoll(this, ptv);

	// 处理文件事件
	processed += processFileEvents(eventCount);

	// 处理时间事件
	processed += processTimeEvents();

	return processed;
}

TimeEvent* EventLoop::getNearestTimeEvent()
{
	TimeEvent* nearest = nullptr;
	TimeEvent* head = timeEventHead;
	while (head != nullptr)
	{
		if (nearest == nullptr || nearest->when_sec > head->when_sec ||
			(nearest->when_sec == head->when_sec && nearest->when_ms > head->when_ms))
		{
			nearest = head;
		}
		head = head->next;
	}
	return nearest;
}

void EventLoop::calcPollWaitTime(timeval** ptv)
{
	TimeEvent* nearestTimeEvent = getNearestTimeEvent();
	if (nearestTimeEvent != nullptr)
	{
		long nowSecond, nowMilliSecond;
		getTimeSecAndMill(&nowSecond, &nowMilliSecond);
		long long dt = (nearestTimeEvent->when_sec - nowSecond) * 1000 + (nearestTimeEvent->when_ms - nowMilliSecond);
		if (dt > 0)
		{
			(**ptv).tv_sec = dt / 1000;
			(**ptv).tv_usec = (dt % 1000) * 1000;
		}
		else
		{
			// 事件等待函数立刻返回
			(**ptv).tv_sec = 0;
			(**ptv).tv_usec = 0;
		}
	}
	else
	{
		// 事件等待函数一直等待，直到有事件到来
		*ptv = nullptr;
	}
}
