
namespace zex
{

    int zex_serv_do_procs(void);
	int zex_serv_do_epoll(void);

    int zex_serv_child(int sock, std::string client_addr);

	int zex_servp_child_read(int sock, std::string client_addr);
	int zex_servp_child_write(int sock, std::string client_addr);

	int	zex_blocker(const std::string addr);
	int zex_zesap_do(const int sock, const std::string& reqstr, std::string& resp_out);
	int setnonblocking(int sock);
}
