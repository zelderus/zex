#include "serv.h"
#include "responser.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>


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
	
	// TODO: parse request from buf
	// TODO: reaction on Signals to terminate

	
	//char resp_content[] = "\n\n<div>reeeesp</div>";
	//int resp_size = sizeof(resp_content);
	char response[] = "HTTP/1.0 200 OK\nServer: zex\nContent-Type: text/html;charset=utf-8\nStatus: 200 OK\nConnection: close\n\n<!DOCTYPE html><html><div>reeeesp</div></html>\r\r";


	// response
	send(sock, response, sizeof(response), 0);
	p("serv_child: sended");	

	close(sock);	//! !!!!!!!!!!!!!!!!! <---------------------
	return 0;
}
