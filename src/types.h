#include <string>
#include <vector>

#define ZEX_VER			"0.3a"

#define ZEX_RET_SERVSUCC	0
#define ZEX_RET_FRMCLIENT 	-3

namespace zex
{


	typedef struct zex_responser_head
	{
		int success;
		std::string status;
		std::string content_type;
	} ZexRespHead;


	

}
