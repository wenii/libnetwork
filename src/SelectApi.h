#ifndef __LIB_NETWORK_SELECT_API_H__
#define __LIB_NETWORK_SELECT_API_H__
#include "PollerInterface.h"
#include <sys/select.h>
namespace libnetwork
{
	class SelectApi : public PollerInterface
	{
	public:
		// ��������
		static SelectApi* create();

		// ���ٶ���
		//void destroy() { delete this; }

	public:
		virtual bool apiCreate(EventLoop* eventLoop);

		virtual bool apiAddEvent(EventLoop* eventLoop, int fd, int mask);

		virtual bool apiDelEvent(EventLoop* eventLoop, int fd, int delMask);

		virtual int apiPoll(EventLoop* eventLoop, struct timeval* tvp);

		virtual bool apiResize(EventLoop* eventLoop, int size);

		virtual const char* apiName();

	private:
		// ���캯��
		SelectApi();

		// ��������
		virtual ~SelectApi();

	private:
		fd_set _rfds;			// ��fd����
		fd_set _wfds;			// дfd����
		fd_set _rfdsBackup;		// ��fd���ϱ���
		fd_set _wfdsBackup;		// дfd���ϱ���

	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_SELECT_API_H__
