#ifndef __LIB_NETWORK_LOG_H__
#define __LIB_NETWORK_LOG_H__

namespace libnetwork
{
	class Log
	{
	public:
		// 输出非错误信息
		static void info(const char* fmt, ...);

		// 输出错误信息
		static void error(const char* fmt, ...);
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_LOG_H__
