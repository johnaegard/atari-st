#include <stdio.h>
#include <tos.h>
#include <stdlib.h>
#include <aes.h>

int main()
{
	char *cmdline, tail[2]={0};

	Pdomain(1);

	creat("u:\\etc\\fastboot");

	cmdline = getenv("SDMASTER");
	if (cmdline != NULL)
		shel_write(0, 1, 1, cmdline, tail);

	return 0;
}
