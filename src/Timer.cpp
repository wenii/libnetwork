#include "Timer.h"
#include "TcpServer.h"
#include <time.h>

using namespace libnetwork;

Timer::Timer(long interval)
	: _interval(interval)
	, _triggerTime(0)
	, _next(nullptr)
	, _remove(false)
{
	const long curTime = time(nullptr);
	_triggerTime = curTime + interval;
}

Timer::~Timer()
{

}

long Timer::getInterval()
{
	return _interval;
}

void Timer::resetInterval(long time)
{
	_interval = time;
}

void Timer::removeTimer()
{
	_remove = true;
}