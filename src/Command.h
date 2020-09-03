#ifndef __LIB_NETWORK_COMMAND_H__
#define __LIB_NETWORK_COMMAND_H__
#include "Types.h"
namespace libnetwork 
{
	class Command
	{
	public:
		bool onMessage(ConnID connID, const std::string& protoName, const std::string& protoData);

		virtual bool execute(ConnID connID, ) = 0;
	};
}	// namespace libnetwork

#endif // __LIB_NETWORK_COMMAND_H__