#include "zex.h"
#include "serv.h"
#include <iostream>
#include <stdlib.h>


using namespace std;
using namespace zex;

int main()
{
	pl("zex/");
	pl(ZEX_VER);
	p(" started");


	int zs = 0;
	zs = zex_serv_do_procs();		/* Один процесс/поток на клиента */
	//zs = zex_serv_do_epoll();		/* Мультиплексирование ввода-вывода */

	if (zs > 0)
	{
		p("zex: err");
		return 1;
	}
	else if (zs == ZEX_RET_FRMCLIENT)	/* client proccess end */
	{
		exit(0);
		return 0;
	}
	else
	{
		p("zex end");
	}

	return 0;
}
