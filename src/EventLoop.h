#ifndef __LIB_NETWORK_EVENT_LOOP_H__
#define __LIB_NETWORK_EVENT_LOOP_H__
#include <time.h>
namespace libnetwork
{
	class PollerInterface;
	class EventLoop;

	// 事件回调函数
	typedef void FileProc(int fd, void* clientData);
	typedef int TimeProc(long long id, void* clientData);
	typedef void EventFinalizerProc(long long id, void* clientData);
	typedef void BeforeSleepProc(void* clientData);
	typedef void SignalProc(int signal, void* clientData);

	static const int LN_NONE = 0;
	static const int LN_READABLE = 1;
	static const int LN_WRITABLE = 2;

	static const int LN_NOMORE = -1;					// 不再执行时间事件

	// 文件事件
	struct FileEvent
	{
		FileEvent();

		int mask;
		FileProc* rFileProc;
		FileProc* wFileProc;
		void* clientData;
	};

	// 时间事件
	struct TimeEvent
	{
		TimeEvent();

		long long id;
		long when_sec;
		long when_ms;
		TimeProc* timeProc;
		EventFinalizerProc* finalizerProc;
		void* clientData;
		struct TimeEvent* next;
		bool remove;
	};


	// 可用事件
	struct FiredEvent 
	{
		FiredEvent();

		int fd;
		int mask;
	};

	class EventLoop
	{
	public:
		// 创建对象
		static EventLoop* create(int size);

		// 销毁对象
		void destory() { delete this; }

	public:
		// 创建文件事件
		bool createFileEvent(int fd, int mask, FileProc* proc, void* clientData);

		// 移除文件事件
		bool deleteFileEvent(int fd, int mask);

		// 创建时间事件
		long long createTimeEven(long long milliseconds, TimeProc* proc, void* clientData, EventFinalizerProc* finalizerProc);

		// 添加信号
		void addSignal(int sig);
		void createSignalEvent(SignalProc* proc);

		// 执行事件循环
		void run();

		// 结束事件循环
		void stop();

		// 处理事件
		int processEvents();
		int processFileEvents(int eventCount);
		int processTimeEvents();

		// 扩容
		void resize();

	public:
		static void signalDesReadableCallback(int fd, void* clientData);


	private:
		// 获取最近时间的时间事件
		TimeEvent* getNearestTimeEvent();

		// 计算等待时间
		void calcPollWaitTime(struct timeval** ptv);

	private:
		// 构造函数
		EventLoop();

		// 析构函数
		~EventLoop();

	public:
		int maxfd; 
		int setsize;
		long long timeEventNextId;
		time_t lastTime;
		FileEvent* events;
		FiredEvent* fired;
		TimeEvent* timeEventHead;
		int _stop;
		BeforeSleepProc* beforesleep;
		PollerInterface* pollerApi;
		void* clientData;
		SignalProc* _singalProc;

		static int _pipedes[2];
	};
}	// libnetwork


#endif // !__LIB_NETWORK_EVENT_LOOP_H__
