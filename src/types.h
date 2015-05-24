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


}
