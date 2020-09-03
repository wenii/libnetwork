#ifndef __TEST_TIMER_H__
#define __TEST_TIMER_H__
#include "../src/Timer.h"
namespace libnetwork
{
	class TestTimer : public Timer
	{
	public:
		TestTimer(long  interval);
		~TestTimer();

	public:
		// 定时器周期性被调用
		virtual void onTimer(long  time);

		// 定时器终结时调用
		virtual void onTimerFinalize(long  time);

		int count;

	};
}	// namespace libnetwork


#endif // !__TEST_TIMER_H__
