//#include "types.h"
#include "help.h"
#include "serv.h"
#include "responser.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


namespace zex
{


    int zex_port = 3542;
    char zex_addr[] = "127.0.0.1";

    // подключение и слушение адреса. основной цикл прослушки запросов
    int zex_serv(void)
    {
        int sock, listener;
        struct sockaddr_in addr;
        int pid;

        //p("serv: working..");
        listener = socket(AF_INET, SOCK_STREAM, 0);
        if (listener < 0)
        {
            p("serv err: socket");
            return 1;
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(zex_port);
        addr.sin_addr.s_addr = inet_addr(zex_addr);// htonl(INADDR_ANY);
        if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        {
            p("serv err: bind");
            p(strerror(errno));
            return 2;
        }

        // console
        pl("address: ");
        pl(zex_addr);
        pl(":");
        pd(zex_port);

        listen(listener, 1);
        p("---------------------------");

        while(1)
        {
            sock = accept(listener, 0, 0);
            if (sock < 0) { p("serv err: accept"); return 3; }
            //p("serv: accept success");

			//+ proccess
			//- способ с новым процессом
			//- альтернатива этому: применение select
            pid = fork();
            if (pid < 0) { p("serv err: fork"); return 4; }

            if (pid == 0) /* client proccess  */
            {
                close(listener);
                zex_serv_child(sock);
                return ZEX_RET_FRMCLIENT;	//- exit in client proccess
            }
            else
            {
                close(sock);
            }
        }

        return 0;
    }

    // отдельный процесс. чтение запроса и ответ
    int zex_serv_child(int sock)
    {
        int n;
        char buf[1024];

        // request
        n = recv(sock, buf, 1024, 0);
        if (n <= 0) { p("serv_child err: recv"); return 1; };
        p("serv_child: recivied");

        // params
        struct zex_serv_params params = zex_serv_getparams(buf);
        // create response
		std::string resp_out = "";	//- save scope
        struct zex_response_t zr = resp_get_response(resp_out, params);
        // response
        send(sock, zr.str, zr.size, 0);

        p("serv_child: sended");
        close(sock);	//!
        return 0;
    }

    // parse request
    struct zex_serv_params zex_serv_getparams(const char* buf)
    {
        struct zex_serv_params params;
        // parse request from buf
		std::vector<RequestParams> elms = parse_http(buf);
		// to model
		for (unsigned i=0; i<elms.size(); i++)
		{
			RequestParams* pr = &elms.at(i);
			params.params.push_back(pr);
			p(pr->name);
			// TODO: to model
			// check GET.. etc
		}
        return params;
    }


}
