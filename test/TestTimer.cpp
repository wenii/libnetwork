#include "TestTimer.h"
#include "../src/Log.h"

using namespace libnetwork;


TestTimer::TestTimer(long  interval)
	: Timer(interval)
	, count(0)
{

}
TestTimer::~TestTimer()
{
	Log::info("TestTimer destroyed.%p",this);
}

void TestTimer::onTimer(long time)
{
	count++;
	if (count > 20)
	{
		removeTimer();
		Log::info("TestTimer::onTimer remove timer.");
	}

	Log::info("TestTimer::onTimer time:%lld, count:%d, interval:%d", time, count, getInterval());
}

void TestTimer::onTimerFinalize(long time)
{
	Log::info("TestTimer::onTimerFinalize time:%lld", time);
}