#ifndef __LIB_NETWORK_EPOLL_API_H__
#define __LIB_NETWORK_EPOLL_API_H__
#include "PollerInterface.h"

struct epoll_event;
namespace libnetwork
{
	class EpollApi : public PollerInterface
	{
	public:
		// 创建对象
		static EpollApi* create();

		// 销毁对象
		//void destroy() { delete this; }

	public:
		virtual bool apiCreate(EventLoop* eventLoop);

		virtual bool apiAddEvent(EventLoop* eventLoop, int fd, int mask);

		virtual bool apiDelEvent(EventLoop* eventLoop, int fd, int delMask);

		virtual int apiPoll(EventLoop* eventLoop, struct timeval* tvp);

		virtual bool apiResize(EventLoop* eventLoop, int size);

		virtual const char* apiName();
	
	private:
		// 构造函数
		EpollApi();

		// 析构函数
		virtual ~EpollApi();

	private:
		int _epfd;
		struct epoll_event* _events;
	};
}	// namespace libnetwork

#endif // !__LIB_NETWORK_EPOLL_API_H__
