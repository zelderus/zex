#include "zex_types.h"
#include "responser.h"


#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>




struct zex_response_t resp_get_response( struct zex_serv_params prms )
{
	struct zex_response_t resp;
	char* response = malloc(1024);

	// content
	char* resp_content = "<html><div>reeeesp</div></html>";
	int resp_size = strlen(resp_content);
	
	
	// header
	zex_strcat(response, "HTTP/1.0 200 OK\n");
	zex_strcat(response, "Server: zex\n");
	zex_strcat(response, "Content-Type: text/html;charset=utf-8\n");
	zex_strcat(response, "Status: 200 OK\n");
	zex_strcat(response, "Content-Length: ");
	char str_cl[30] = "";
	sprintf(str_cl, "%d", resp_size);
	zex_strcat(response, str_cl);
	zex_strcat(response, "\n\n");
	zex_strcat(response, resp_content);
	zex_strcat(response, "\r\r");


	// send
	//p(response);
	resp.num = 97;
	resp.str = response;
	resp.size = strlen(response);
	return resp;
}



