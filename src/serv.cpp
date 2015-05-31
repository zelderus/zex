//#include "types.h"
#include "help.h"
#include "serv.h"
#include "responser.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>




namespace zex
{

	// TODO: to config
	int zex_port = 3542;
	char zex_addr[] = "127.0.0.1";
	char zesap_socket[] = "/home/zelder/cc/zesap/tmp/zesap.sock";



	static int serv_stopped = 0; 
	int serv_child = 0;
	int sock = 0;
	int listener = 0;
	int status = 0;

	//
	// пришло прерывание на выключение
	//
	void
	zex_onsignal( int signum )
	{
		if (serv_child)
		{
			close(sock);
		}
		else
		{
			pl("\nsignal:");
			pd(signum);

			close(listener);
			serv_stopped = 1;	//- flag to stop main proccess
		}
	}

	//
	// дочерний прцесс завершился, обрабатывем его ответ, чтобы он уничтожился
	//
	void
	zex_child_zombie( int num )
	{
		pid_t kidpid;
		int stat;
		// kill all zombies
		while ((kidpid = waitpid(-1, &stat, WNOHANG)) > 0)
		{
			pl("child zombie terminating ");
			pd(kidpid);
		}
	}


	//
	// подключение и слушение адреса. основной цикл прослушки запросов
	//
	int 
	zex_serv(void)
	{
		struct sockaddr_in addr;
		int pid;
		struct sigaction sa;

		// сокет для прослушки порта
		listener = socket(AF_INET, SOCK_STREAM, 0);
		if (listener < 0)
		{
			p("serv err: socket");
			return 1;
		}
		// подключаемся на адрес для прослушки
		addr.sin_family = AF_INET;
		addr.sin_port = htons(zex_port);
		addr.sin_addr.s_addr = inet_addr(zex_addr); // htonl(INADDR_ANY);
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

		//- слушаем сигналы из вне (остановка приложения)
		signal(SIGINT, zex_onsignal);
		//- ждем сигнала от дочерних прцессов (чтобы обработать их и завершить)
		sigfillset(&sa.sa_mask);
		sa.sa_handler = zex_child_zombie;
		sa.sa_flags = 0;
		sigaction(SIGCHLD, &sa, NULL);

		// вечный цикл - слушаем соединения
		while(1)
		{
			if (serv_stopped) break;	// обаботали сигнал на остановку приложения
			// получаем входящий запрос
			errno = 0;
			sock = accept(listener, 0, 0);
			if (sock < 0)
			{
				// было прерывание, завершился дочерний прцесс
				if (errno == EINTR) /* The system call was interrupted by a signal */
				{
					// dont worry, just killed a child
					continue;
				}
				pl("serv err: accept -> "); 
				p(strerror(errno)); 
				return 3;
			}
			//+ создание нового процесса с полной обработкой запроса
			//- способ с новым процессом
			//- альтернатива этому: применение select+poll или потоков
			pid = fork();
			if (pid < 0) { p("serv err: fork"); return 4; }

			if (pid == 0) /* client proccess  */
			{
				serv_child = 1;
				close(listener);			//- у нового процесса продублировались дескрипторы
				zex_serv_child(sock);		//- основная функция обработки
				return ZEX_RET_FRMCLIENT;	//- exit in client proccess
			}
			else
			{
				close(sock);
			}
		}
		return 0;
	}



	//
	//	помощник. отпраква сообщения и закрытие сокета
	//
	void
	zex_send(const int sock, const std::string& str)
	{
		send(sock, &str[0], str.size(), 0);
		close(sock);
	}

	//
	//	обращение к ZEBLOCKER
	//	возвращает:
	//		0 - есть доступ
	//		1 - нет доступа
	//
	int
	zex_blocker(const std::string addr)
	{
		// TODO: через трубу запрашиваем разрешение на текущего клиента (IP) у демона zeblocker
		

		return 0;
	}


	//
	//	запрос к ZESAP и ответ от него
	//	присоединяемся по UNIX-сокету к zesap, передаем поток запроса и получаем ответ
	//	return: 
	//		0 	- OK
	//		>0	- error
	//
	int
	zex_zesap_do(const int sock, const std::string& reqstr, std::string& resp_out)
	{
		int ressize = 2048;
		char bufcli[ressize];

		int unisock;
		struct sockaddr_un addr;
		unisock = socket(AF_UNIX, SOCK_STREAM, 0);
		errno = 0;
		if(unisock < 0)
		{
			pl("serv_child err: socket -> ");
			p(strerror(errno)); 
			resp_getresponse_500(resp_out, "502 Bad Gateway", "ZEX don't access to ZESAP");
			zex_send(sock, resp_out);
		    return 2;
		}
		addr.sun_family = AF_UNIX;
		strncpy(addr.sun_path, zesap_socket, sizeof(addr.sun_path)-1);
		errno = 0;
		if (connect(unisock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		{
			pl("serv_child err: connect -> ");
			p(strerror(errno)); 
			resp_getresponse_500(resp_out, "502 Bad Gateway", "ZEX don't access to ZESAP connecting");
			zex_send(sock, resp_out);
			close(unisock);
			return 3;
		}
		send(unisock, &reqstr[0], reqstr.size(), 0);
		recv(unisock, bufcli, ressize, 0);
		resp_out = std::string(bufcli);
		close(unisock);

		return 0;	// 0 - OK
	}



	//
	// отдельный процесс. чтение запроса и ответ
	//
	int 
	zex_serv_child(int sock)
	{
		int n, reqsize;
		reqsize = 1024;
		char buf[reqsize];
		// request (читаем в буфер из сокета)
		n = recv(sock, buf, reqsize, 0);
		if (n <= 0) { p("serv_child err: recv"); return 1; };
		std::string reqstr = std::string(buf);
		p("serv_child: recivied");
		// ответ
		std::string resp_out = "";
		//+ 1. zeblocker
		std::string client_addr = "";
		int blocker = zex_blocker(client_addr);
		if (blocker)
		{
			resp_getresponse_500(resp_out, "403 Forbidden", "stop it");
			zex_send(sock, resp_out);
		    return 1;
		}
		//+ 2. zesap
		int work = zex_zesap_do(sock, reqstr, resp_out);
		if (work > 0)
		{
			close(sock);
			return work;
		}
		// response (пишем в буфер)
		zex_send(sock, resp_out);
		p("serv_child: sended");
		return 0;
	}


	

}
