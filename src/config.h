#ifndef __LIB_NETWORK_CONFIG_H__
#define __LIB_NETWORK_CONFIG_H__

//#define NETWORK_SERVER
#define NETWORK_CLIENT

#ifdef NETWORK_SERVER
#define LIB_NETWORK_USE_EPOLL
#else
	#ifdef NETWORK_CLIENT
		#define LIB_NETWORK_USE_SELECT
	#else
		static_assert(false, "no mcro define, must set NETWORK_SERVER or NETWORK_CLIENT");
	#endif
#endif // 


#ifdef LIB_NETWORK_USE_EPOLL
	#include "EpollApi.h"
#else
	#ifdef LIB_NETWORK_USE_SELECT
		#include "SelectApi.h"
	#endif
#endif // LIB_NETWORK_USE_EPOLL



#endif // !__LIB_NETWORK_CONFIG_H__
