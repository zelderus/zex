#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>


void p (const char* str)
{
	printf("%s\n", str);
}
void pl (const char* str)
{
	printf("%s", str);
}
void pd (const int num)
{
	printf("%d\n", num);
}
void p_ver (const char* ver)
{
	printf("version: %s\n", ver);
}


void zex_strcat(char* dest, char* src)
{
	strcat(dest, src);
	return;

	size_t len1 = strlen(dest);
	size_t len2 = strlen(src);

	//p("========");
	//pd(len1);

	char*s = malloc(len1 + len2 + 1);
	
	memcpy(s, dest, len1);
	memcpy(s + len1, src, len2 + 1);

	//dest = malloc(strlen(s));
	dest = s;
	//len1 = strlen(dest);
	//pd(len1);
	//p(dest);

//p(dest);
//p("--------");
}





void write_demo(const char* str)
{
	int fd;
	// file opts
	char* fname = "./kii.txt";
	int fflags = O_CREAT | O_APPEND | O_RDWR;
	mode_t fmode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
	// open
	errno = 0;
	fd = open(fname, fflags, fmode);
	if (fd == -1)
	{
		// err
		printf("E: file open err -> %s\n", strerror(errno));
		return;
	}
	printf("M: file opened with fd=%d\n", fd);
	// write
	ssize_t nr;
	errno = 0;
	nr = write(fd, str, strlen(str));
	if (nr == -1)
	{
		printf("E: file '%d' write err -> %s\n", fd, strerror(errno));
	}

	// close
	if (close(fd) == -1)
	{
		printf("E: file close err\n");
		return;
	}
}




