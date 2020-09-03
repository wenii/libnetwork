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
		// ��ʱ�������Ա�����
		virtual void onTimer(long  time);

		// ��ʱ���ս�ʱ����
		virtual void onTimerFinalize(long  time);

		int count;

	};
}	// namespace libnetwork


#endif // !__TEST_TIMER_H__
