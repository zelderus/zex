#include "zex.h"
#include "serv.h"
#include <iostream>

using namespace std;
using namespace zex;

int main()
{
    pl("zex/");
	pl(ZEX_VER);
	p(" started");

	// TODO: reaction on Signals to terminate

	int zs = zex_serv();
	if (zs > 0)
	{
		p("zex: err");
		return 1;
	}
	else if (zs == ZEX_RET_FRMCLIENT)	/* client proccess end */
	{

		return 0;
	}
	else
	{
		p("zex end");
	}

	return 0;
}
