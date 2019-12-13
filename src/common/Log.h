#ifndef __LIB_NETWORK_LOG_H__
#define __LIB_NETWORK_LOG_H__

namespace libnetwork
{
	class Log
	{
	public:
		// ����Ǵ�����Ϣ
		static void info(const char* fmt, ...);

		// ���������Ϣ
		static void error(const char* fmt, ...);
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_LOG_H__
