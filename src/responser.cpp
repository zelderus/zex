//#include "types.h"
#include "responser.h"
#include "help.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>




using namespace std;

namespace zex
{
	namespace responser
	{
		void 
		get_header( std::string& out, const std::string status, int content_size )
		{
			out += ("HTTP/1.0 " + status + "\n");
			out += ("Server: zex/" + string(ZEX_VER) + "\n");
			out += ("Content-Type: text/html;charset=utf-8\n");
			out += ("Status: " + status + "\n");
			out += ("Content-Length: " + inttostr(content_size) + "\n");
			out += "\n";
		}
	}

	//
	// ответ ошибка сервера
	// статусы http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
	//
	void
	resp_getresponse_500( std::string& outstr, const std::string status, const std::string errmsg )
	{
		string& response = outstr;	
		// request content
		string resp_content = "<html><head><title>zex</title></head><body>";
		resp_content += "<div style='margin:0 auto; padding: 20px;padding: 10px !important;  border: 1px solid #732626;  background-color: #F8ECEC;  color: #732626;  margin: 0 !important;  -webkit-border-radius: 8px;  -moz-border-radius: 8px;  border-radius: 8px;'>";
		resp_content += ("<h1 style='text-align:center;'>" +status+ "</h1>");
		resp_content += ("<div style='text-align:center; font-size:10px;'>" +("(zex/"+string(ZEX_VER)+")")+ "</div>");
		resp_content += ("<div style='text-align:left;'>" +errmsg+ "</div>");
		resp_content += "</div></body></html>";
		int content_size = resp_content.size();
		// header
		responser::get_header(response, status, content_size);
		response += resp_content;
		response += "\r\r";
	}

}










