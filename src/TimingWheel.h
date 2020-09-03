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
		// 创建对象
		static TimingWheel* create();

		// 销毁
		void destory();

	public:
		// 经过一秒钟
		void tick();

		// 处理时间事件
		void processTimerEvent();

		// 添加事件
		void addTimer(Timer* timer);
		

	private:
		// 添加事件到指定索引
		void addToIndex(Timer* timer, int index);

	public:
		// 构造函数
		TimingWheel();

		// 析构函数
		~TimingWheel();

	private:
		Timer* _wheel[SECONDS_WHEEL];
		long long _seconds;
		Timer* _needAddTimers;
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_TIMING_WHEEL_H__
