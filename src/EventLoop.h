#ifndef __LIB_NETWORK_EVENT_LOOP_H__
#define __LIB_NETWORK_EVENT_LOOP_H__
#include <time.h>
namespace libnetwork
{
	class PollerInterface;
	class EventLoop;

	// �¼��ص�����
	typedef void FileProc(int fd, void* clientData);
	typedef int TimeProc(long long id, void* clientData);
	typedef void EventFinalizerProc(long long id, void* clientData);
	typedef void BeforeSleepProc(void* clientData);
	typedef void SignalProc(int signal, void* clientData);

	static const int LN_NONE = 0;
	static const int LN_READABLE = 1;
	static const int LN_WRITABLE = 2;

	static const int LN_NOMORE = -1;					// ����ִ��ʱ���¼�

	// �ļ��¼�
	struct FileEvent
	{
		FileEvent();

		int mask;
		FileProc* rFileProc;
		FileProc* wFileProc;
		void* clientData;
	};

	// ʱ���¼�
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


	// �����¼�
	struct FiredEvent 
	{
		FiredEvent();

		int fd;
		int mask;
	};

	class EventLoop
	{
	public:
		// ��������
		static EventLoop* create(int size);

		// ���ٶ���
		void destory() { delete this; }

	public:
		// �����ļ��¼�
		bool createFileEvent(int fd, int mask, FileProc* proc, void* clientData);

		// �Ƴ��ļ��¼�
		bool deleteFileEvent(int fd, int mask);

		// ����ʱ���¼�
		long long createTimeEven(long long milliseconds, TimeProc* proc, void* clientData, EventFinalizerProc* finalizerProc);

		// ����ź�
		void addSignal(int sig);
		void createSignalEvent(SignalProc* proc);

		// ִ���¼�ѭ��
		void run();

		// �����¼�ѭ��
		void stop();

		// �����¼�
		int processEvents();
		int processFileEvents(int eventCount);
		int processTimeEvents();

		// ����
		void resize();

	public:
		static void signalDesReadableCallback(int fd, void* clientData);


	private:
		// ��ȡ���ʱ���ʱ���¼�
		TimeEvent* getNearestTimeEvent();

		// ����ȴ�ʱ��
		void calcPollWaitTime(struct timeval** ptv);

	private:
		// ���캯��
		EventLoop();

		// ��������
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
