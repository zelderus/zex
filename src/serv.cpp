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
#include <sys/epoll.h>
#include <fcntl.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>


#define EPOLL_QUEUE_LEN 	1
#define MAX_EVENTS 			64

namespace zex
{

	// TODO: to config
	int zex_port = ZEX_SRV_PORT;
	char zex_addr[] = ZEX_SRV_ADDR;
	char zesap_socket[] = ZEX_SRV_SOCK;



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
	//	Мультиплексирование ввода-вывода
	//
	int 
	zex_serv_do_epoll(void)
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

 		// Неблокирующий режим (EPOLL)
 		setnonblocking(listener);

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

		//+ начинаем слушать запросы
		listen(listener, 1);
		p("---------------------------");

		//- слушаем сигналы из вне (остановка приложения)
		signal(SIGINT, zex_onsignal);
		//- ждем сигнала от дочерних прцессов (чтобы обработать их и завершить)
		sigfillset(&sa.sa_mask);
		sa.sa_handler = zex_child_zombie;
		sa.sa_flags = 0;
		sigaction(SIGCHLD, &sa, NULL);


		//!+ EPOLL	-------------------------->
		int epfd, fd, ret;
		struct epoll_event event;
		//- create epoll
		epfd = epoll_create1(EPOLL_CLOEXEC);
		if (epfd < 0)
		{
			p("epoll error");
			return 5;
		}
		//- event epoll
		event.data.fd = listener;
		event.events = EPOLLIN | EPOLLPRI | EPOLLET; //EPOLLIN | EPOLLOUT;
		ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &event);
		if (ret)
		{
			p("epoll_ctl");
			return 6;
		}
		//struct epoll_event *events;
		struct epoll_event events[MAX_EVENTS];
		struct epoll_event connev;
		int nr_events, i;
		int events_cout = 1;
		// <------------------------ end of EPOLL


		socklen_t client;
		struct sockaddr_in cliaddr;

		// вечный цикл - слушаем соединения
		while(1)
		{
			if (serv_stopped) break;	// обаботали сигнал на остановку приложения
			
			/*
			*	===
			*	===== Мультиплексирование ввода-вывода =====
			*	===
			*
			*/
			std::string client_addr = "";
			// Блокирование до готовности одно или нескольких дескрипторов
  			int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
			for (int n = 0; n < nfds; ++n)
  			{
				// Готов слушающий дескриптор
				if (events[n].data.fd == listener)
				{
					client = sizeof(cliaddr);
					int connfd = accept(listener, (struct sockaddr*) &cliaddr, &client);
					if (connfd < 0)
					{
						p("accept err");
						continue;
					}
					// Недостаточно места в массиве ожидания
					if (events_cout == MAX_EVENTS-1)
					{
						p("err: MAX_EVENTS");
						close(connfd);
						continue;
					}
					// Добавление клиентского дескриптора в массив ожидания
					setnonblocking(connfd);

					connev.data.fd = connfd;
					connev.events = EPOLLIN;// | EPOLLOUT | EPOLLET | EPOLLRDHUP;
					if (!epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &connev) < 0)
					{
						p("Epoll fd add");
						close(connfd);
						continue;
					}
					++events_cout;
				}
				// Готов клиентский дескриптор
				else
				{
					// Выполням работу с дескриптором
					int fd = events[n].data.fd;

					if (events[n].events & EPOLLIN)
					{	
						p("child reading..");
						zex_servp_child_read(fd, client_addr);
						connev.events = EPOLLOUT;
						epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &connev);

						continue;	// не закрывает дескриптор
					}
					if (events[n].events & EPOLLOUT)
					{
						p("child writing..");
						setnonblocking(fd);
						zex_servp_child_write(fd, client_addr);
					}
					if (events[n].events & EPOLLRDHUP)
					{
						pl("serv procc err, fd=");
						pd(fd);
					}

					// В даннoм примере дескриптор просто закрывается и удаляется из массива ожидания.
					// В зависимости от логики работы можно не удалять дескриптор и подождать следующую порцию данных
					epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &connev);
					--events_cout;
					close(fd);
				}
			}

		}

		//+ close
		close(epfd);
		close(listener);

		return 0;
	}




	//
	// подключение и слушение адреса. основной цикл прослушки запросов
	//	Один процесс/поток на клиента
	//
	int 
	zex_serv_do_procs(void)
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

		//+ начинаем слушать запросы
		listen(listener, 1);
		p("---------------------------");

		//- слушаем сигналы из вне (остановка приложения)
		signal(SIGINT, zex_onsignal);
		//- ждем сигнала от дочерних прцессов (чтобы обработать их и завершить)
		sigfillset(&sa.sa_mask);
		sa.sa_handler = zex_child_zombie;
		sa.sa_flags = 0;
		sigaction(SIGCHLD, &sa, NULL);


		socklen_t client;
		struct sockaddr_in cliaddr;

		// вечный цикл - слушаем соединения
		while(1)
		{
			if (serv_stopped) break;	// обаботали сигнал на остановку приложения


			/*
			*	===
			*	===== Один процесс/поток на клиента =====
			*	===
			*
			*/
			// получаем входящий запрос
			errno = 0;
			std::string client_addr = "";	// TODO: получить IP клиента
			sock = accept(listener, (struct sockaddr*) &cliaddr, &client);
			if (sock < 0)
			{
				// было прерывание, завершился дочерний процесс
				if (errno == EINTR)
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
			if (pid == 0) // client proccess
			{
				serv_child = 1;
				close(listener);					//- у нового процесса продублировались дескрипторы
				zex_serv_child(sock, client_addr);	//- основная функция обработки
				return ZEX_RET_FRMCLIENT;			//- exit in client proccess
			}
			else
			{
				close(sock);
			}
		}

		//+ close
		//close(listener);

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
	void
	zex_send2(const int sock, const std::string& str)
	{
		send(sock, &str[0], str.size(), 0);
	}




	// читаем запрос
	int
	zex_servp_child_read(int sock, std::string client_addr)
	{
		int n, reqsize;
		reqsize = 1024;
		char buf[reqsize];
		// request (читаем в буфер из сокета)
		n = recv(sock, buf, reqsize, 0);
		if (n <= 0) { p("serv_child err: recv"); return 1; };
		std::string reqstr = std::string(buf);
		p("serv_child: recivied");
		// TODO: save reqstr

		
		return 0;
	}
	
	// пишем ответ
	int
	zex_servp_child_write(int sock, std::string client_addr)
	{
		// TODO: load reqstr
		std::string reqstr = "";

		// ответ
		std::string resp_out = "";
		//+ 1. zeblocker
		int blocker = zex_blocker(client_addr);
		if (blocker)
		{
			resp_getresponse_500(resp_out, "403 Forbidden", "stop it");
			zex_send2(sock, resp_out);
			return 1;
		}
		//+ 2. zesap
		int work = zex_zesap_do(sock, reqstr, resp_out);
		if (work > 0)
		{
			//close(sock);
			return work;
		}
		// response (пишем в буфер)
		zex_send2(sock, resp_out);
		p("serv_child: sended");
		return 0;
	}










	//
	// отдельный процесс. чтение запроса и ответ
	//
	int 
	zex_serv_child(int sock, std::string client_addr)
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



	// Неблокируемый поток
	int setnonblocking(int sock)
	{
		int opts;
		opts = fcntl(sock,F_GETFL);
		if (opts < 0)
		{
			p("fcntl(F_GETFL)");
			return -1;
		}
		opts = (opts | O_NONBLOCK);
		if (fcntl(sock,F_SETFL,opts) < 0)
		{
			p("fcntl(F_SETFL)");
			return -1;
		}
		return 0;
	}

	

}
