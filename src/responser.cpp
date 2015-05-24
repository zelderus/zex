#include "types.h"
#include "responser.h"
#include "help.h"


#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>


using namespace std;

namespace zex
{
	namespace responser
	{
		// check request
		struct zex_responser_head check_request ( std::string& out, const struct zex_serv_params& prms)
		{
			struct zex_responser_head head;
			// TODO: check
			head.success = 1;	// 1=ok, 0=error
			head.status = "200 OK";
			head.content_type = "text/html;charset=utf-8";
			// content
			if (head.success)
			{
				out = "<html><div>reeeesp yeaaaahhhh</div></html>";
			}
			return head;
		}

		// header
		void get_header( std::string& out, const struct zex_responser_head& head, int content_size, const struct zex_serv_params& prms )
		{
			out += ("HTTP/1.0 " + head.status + "\n");
		    out += ("Server: zex/" + string(ZEX_VER) + "\n");
		    out += ("Content-Type: " + head.content_type + "\n");
		    out += ("Status: " + head.status + "\n");
		    out += ("Content-Length: " + inttostr(content_size) + "\n");
		    out += "\n";
		}
	}


    struct zex_response_t resp_get_response( std::string& outstr, struct zex_serv_params prms )
    {
        struct zex_response_t resp;
        string& response = outstr;	
		// request content
		string resp_content = "";
		struct zex_responser_head head = responser::check_request(resp_content, prms);
        int content_size = resp_content.size();
        // header
        responser::get_header(response, head, content_size, prms);
        response += resp_content;
        response += "\r\r";
        // send
        resp.num = 83;
        resp.str = &response[0];
        resp.size = response.size();
        return resp;
    }

}









