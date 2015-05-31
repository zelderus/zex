
namespace zex
{

    int zex_serv(void);
    int zex_serv_child(int sock, std::string client_addr);

	int	zex_blocker(const std::string addr);
	int zex_zesap_do(const int sock, const std::string& reqstr, std::string& resp_out);
}
