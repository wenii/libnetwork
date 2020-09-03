#ifndef __LIB_NETWORK_POLLER_INTERFACE_H__
#define __LIB_NETWORK_POLLER_INTERFACE_H__
struct timeval;
namespace libnetwork
{
	class EventLoop;
	class PollerInterface
	{
	public:
		// ����
		virtual ~PollerInterface(){}

		// ���ٶ���
		void destroy() { delete this; }
	public:
		// ������Ӧapi����
		virtual bool apiCreate(EventLoop* eventLoop) = 0;

		// ����¼�
		virtual bool apiAddEvent(EventLoop* eventLoop, int fd, int mask) = 0;

		// �Ƴ��¼�
		virtual bool apiDelEvent(EventLoop* eventLoop, int fd, int delMask) = 0;

		// �ȴ��¼�
		virtual int apiPoll(EventLoop* eventLoop, struct timeval* tvp) = 0;

		// ����
		virtual bool apiResize(EventLoop* eventLoop, int size) = 0;

		// ��ȡapi����
		virtual const char* apiName() = 0;
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_POLLER_INTERFACE_H__
