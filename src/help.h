#include "types.h"
#include <string>
#include <vector>

namespace zex
{

    void p(const char* str);
	void p(const std::string& str);
    void pl (const char* str);
	void pl(const std::string& str);
    void pd (const int num);
    void p_ver(const char* str);

    void zex_strcat(char* dest, char* src);
    std::string inttostr (int num);
	std::string strtoupper (const std::string& str);



}

