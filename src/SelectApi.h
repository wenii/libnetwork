#ifndef __LIB_NETWORK_SELECT_API_H__
#define __LIB_NETWORK_SELECT_API_H__
#include "PollerInterface.h"
#include <sys/select.h>
namespace libnetwork
{
	class SelectApi : public PollerInterface
	{
	public:
		// 创建对象
		static SelectApi* create();

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
		SelectApi();

		// 析构函数
		virtual ~SelectApi();

	private:
		fd_set _rfds;			// 读fd集合
		fd_set _wfds;			// 写fd集合
		fd_set _rfdsBackup;		// 读fd集合备份
		fd_set _wfdsBackup;		// 写fd集合备份

	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_SELECT_API_H__
