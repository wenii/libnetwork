#ifndef __LIB_NETWORK_TIMER_H__
#define __LIB_NETWORK_TIMER_H__

namespace libnetwork
{
	class Timer
	{
	public:
		Timer(long interval);
		virtual ~Timer();

	public:
		// ��ʱ�������Ա�����
		virtual void onTimer(long time) = 0;

		// ��ʱ���ս�ʱ����
		virtual void onTimerFinalize(long time) = 0;

	public:
		// ��ȡ��ʱ��ʱ����
		long getInterval();

		// ���ö�ʱ��ʱ����
		void resetInterval(long time);

		// �Ƴ���ʱ��
		void removeTimer();

	private:
		long _interval;			// ��ʱ�����ʱ�䣨�룩
		long _triggerTime;		// ��ʱ������ʱ�䣨�룩
		Timer* _next;			// ��һ����ʱ��
		bool _remove;			// �Ƴ�
		friend class TimingWheel;

	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_TIMER_H__
