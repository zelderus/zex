#include "types.h"
#include "responser.h"
#include "help.h"


#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

using namespace std;

namespace zex
{


    struct zex_response_t resp_get_response( struct zex_serv_params prms )
    {
        struct zex_response_t resp;
        string response = "";//malloc(1024);

        // content
        string resp_content = "<html><div>reeeesp</div></html>";
        int resp_size = resp_content.size();


        // header
        response += "HTTP/1.0 200 OK\n";
        response += "Server: zex\n";
        response += "Content-Type: text/html;charset=utf-8\n";
        response += "Status: 200 OK\n";
        response += "Content-Length: ";
        response += inttostr(resp_size);
        response += "\n\n";
        response += resp_content;
        response += "\r\r";


        // send
        //p(response);
        resp.num = 97;
        resp.str = &response[0];
        resp.size = response.size();
        return resp;
    }


}
