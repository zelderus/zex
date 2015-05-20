#include "zex.h"


int main (void)
{
	
	p("zex working");
	
	int zs = zex_serv();
	if (zs < 0)
	{
		p("zex error");
	}

	


	p("zex end");
	return 0;
}
