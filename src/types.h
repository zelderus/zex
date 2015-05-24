#include <string>

#define ZEX_VER 			"0.1"

#define ZEX_RET_SERVSUCC	0
#define ZEX_RET_FRMCLIENT 	-3

namespace zex
{

    struct zex_response_t
    {
        int num;
        char* str;
        int size;
    };

    struct zex_serv_params
    {
        char* url;
        // ..

    };

	struct zex_responser_head
	{
		int success;
		std::string status;
		std::string content_type;
	};


}
