#ifndef __LIB_NETWORK_TIMING_WHEEL_H__
#define __LIB_NETWORK_TIMING_WHEEL_H__

namespace libnetwork
{
	struct TimeEvent;
	class Timer;
	class TimingWheel
	{
		static const int SECONDS_WHEEL = 60;

	public:
		// ��������
		static TimingWheel* create();

		// ����
		void destory();

	public:
		// ����һ����
		void tick();

		// ����ʱ���¼�
		void processTimerEvent();

		// ����¼�
		void addTimer(Timer* timer);
		

	private:
		// ����¼���ָ������
		void addToIndex(Timer* timer, int index);

	public:
		// ���캯��
		TimingWheel();

		// ��������
		~TimingWheel();

	private:
		Timer* _wheel[SECONDS_WHEEL];
		long long _seconds;
		Timer* _needAddTimers;
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_TIMING_WHEEL_H__
