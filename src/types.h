#include <string>
#include <vector>

#define ZEX_VER			"0.4a"

#define ZEX_RET_SERVSUCC	0
#define ZEX_RET_FRMCLIENT 	-3

// TODO: to config
#define ZEX_SRV_PORT 		3542
#define ZEX_SRV_ADDR 		"127.0.0.1"
#define ZEX_SRV_SOCK 		"/home/zelder/cc/zesap/tmp/zesap.sock"


namespace zex
{


	typedef struct zex_responser_head
	{
		int success;
		std::string status;
		std::string content_type;
	} ZexRespHead;


	

}
