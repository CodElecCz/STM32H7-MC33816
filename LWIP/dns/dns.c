#include "lwip/opt.h"
#include "dns.h"
#include "lwip/dns.h"

#include "main.h"

void ip_found(const char *name, const ip_addr_t *ipaddr, void *arg)
{
    uint8_t ip[4];

    ip[0] = (ipaddr->addr)>>24;
    ip[1] = (ipaddr->addr)>>16;
    ip[2] = (ipaddr->addr)>>8;
    ip[3] = (ipaddr->addr);

    MAIN_DEBUG_TRACE(DNS, ("DNS '%s' IP: %d.%d.%d.%d\n", name, ip[3], ip[2], ip[1], ip[0]));
}

bool
dns_ex_init()
{
	err_t err  = ERR_OK;

	//if static address
	ip_addr_t dnsserver;
	IP_ADDR4( &dnsserver, 1,1,1,1);
	dns_setserver(0, &dnsserver);

	IP_ADDR4( &dnsserver, 8,8,8,8);
	dns_setserver(1, &dnsserver);

	char hostname[] = "pool.ntp.org";

	ip_addr_t serverIp;
	err  = dns_gethostbyname((char*)(hostname), &(serverIp), ip_found, NULL);

	return ( err == ERR_OK);
}
