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
		// 定时器周期性被调用
		virtual void onTimer(long time) = 0;

		// 定时器终结时调用
		virtual void onTimerFinalize(long time) = 0;

	public:
		// 获取定时器时间间隔
		long getInterval();

		// 重置定时器时间间隔
		void resetInterval(long time);

		// 移除定时器
		void removeTimer();

	private:
		long _interval;			// 定时器间隔时间（秒）
		long _triggerTime;		// 定时器触发时间（秒）
		Timer* _next;			// 下一个定时器
		bool _remove;			// 移除
		friend class TimingWheel;

	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_TIMER_H__
