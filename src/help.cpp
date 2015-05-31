#include "help.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <locale>
#include <errno.h>
#include <stdlib.h>
#include <sstream>


namespace zex
{

    void p (const char* str)
    {
        printf("%s\n", str);
    }
	void p(const std::string& str)
	{
		p(str.c_str());
	}
    void pl (const char* str)
    {
        printf("%s", str);
    }
	void pl(const std::string& str)
	{
		pl(str.c_str());
	}
    void pd (const int num)
    {
        printf("%d\n", num);
    }
    void p_ver (const char* ver)
    {
        printf("version: %s\n", ver);
    }

	std::string strtoupper (const std::string& str)
	{
		std::string out;
		std::locale loc;
		for (std::string::size_type i=0; i<str.length(); ++i)
		out += std::toupper(str[i],loc);

		return out;
	}

	std::vector<std::string> &split(const std::string& s, char delim, std::vector<std::string> &elems)
	{
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim))
		{
			elems.push_back(item);
		}
		return elems; 		
	}
	
	std::vector<std::string> split(const std::string &s, char delim)
	{
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}

    void zex_strcat(char* dest, char* src)
    {
        strcat(dest, src);
        return;
		/*
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
        */

    	//p(dest);
    	//p("--------");
    }


    std::string inttostr (int num)
    {
        char intStr[30] = "";
        sprintf(intStr, "%d", num);
        //itoa(num, intStr, 10);
        return std::string(intStr);
    }








}
