#ifndef __LIB_NETWORK_POLLER_INTERFACE_H__
#define __LIB_NETWORK_POLLER_INTERFACE_H__
struct timeval;
namespace libnetwork
{
	class EventLoop;
	class PollerInterface
	{
	public:
		// 析构
		virtual ~PollerInterface(){}

		// 销毁对象
		void destroy() { delete this; }
	public:
		// 创建对应api数据
		virtual bool apiCreate(EventLoop* eventLoop) = 0;

		// 添加事件
		virtual bool apiAddEvent(EventLoop* eventLoop, int fd, int mask) = 0;

		// 移除事件
		virtual bool apiDelEvent(EventLoop* eventLoop, int fd, int delMask) = 0;

		// 等待事件
		virtual int apiPoll(EventLoop* eventLoop, struct timeval* tvp) = 0;

		// 扩容
		virtual bool apiResize(EventLoop* eventLoop, int size) = 0;

		// 获取api名称
		virtual const char* apiName() = 0;
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_POLLER_INTERFACE_H__
