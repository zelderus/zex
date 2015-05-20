#include "zex.h"

int main (void)
{
	p("zex started");
	
	
	int zs = zex_serv();
	if (zs < 0)
	{
		p("serv: err");
		return 1;
	}



	p("zex end");
	return 0;
}
