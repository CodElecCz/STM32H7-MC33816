/*
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Dirk Ziegelmeier <dziegel@gmx.de>
 *
 */

/*
Multicast DNS for lwIP

Author: Erik Ekman


Note! The MDNS responder does not have all features required by the standards.
See notes in src/apps/mdns/mdns.c for what is left. It is however usable in normal
cases - but watch out if many devices on the same network try to use the same
host/service instance names.


How to enable:
==============

MDNS support does not depend on DNS.
MDNS supports using IPv4 only, v6 only, or v4+v6.

To enable MDNS responder, set
  LWIP_MDNS_RESPONDER = 1
in lwipopts.h and add src/apps/mdns/mdns.c to your list of files to build.

The max number of services supported per netif is defined by MDNS_MAX_SERVICES,
default is 1.

Increase MEMP_NUM_UDP_PCB by 1. MDNS needs one PCB.
Increase LWIP_NUM_NETIF_CLIENT_DATA by 1 (MDNS needs one entry on netif).

MDNS with IPv4 requires LWIP_IGMP = 1, and preferably LWIP_AUTOIP = 1.
MDNS with IPv6 requires LWIP_IPV6_MLD = 1, and that a link-local address is
generated.

The MDNS code puts its structs on the stack where suitable to reduce dynamic
memory allocation. It may use up to 1kB of stack.

MDNS (like other apps) needs a strncasecmp() implementation. If you have one, define
'lwip_strnicmp' to it. Otherwise the code will provide an implementation
for you.


How to use:
===========

Call mdns_resp_init() during system initialization.
This opens UDP sockets on port 5353 for IPv4 and IPv6.


To start responding on a netif, run
  mdns_resp_add_netif(struct netif *netif, char *hostname, u32_t dns_ttl)

The hostname will be copied. If this returns successfully, the netif will join
the multicast groups and any MDNS/legacy DNS requests sent unicast or multicast
to port 5353 will be handled:
- <hostname>.local type A, AAAA or ANY returns relevant IP addresses
- Reverse lookups (PTR in-addr.arpa, ip6.arpa) of netif addresses
  returns <hostname>.local
Answers will use the supplied TTL (in seconds)
MDNS allows UTF-8 names, but it is recommended to stay within ASCII,
since the default case-insensitive comparison assumes this.

Call mdns_resp_announce() every time the IP address on the netif has changed.

Call mdns_resp_restart() every time the network interface comes up after being
down, for example cable connected after being disconnected, administrative
interface comes up after being down, or the device wakes up from sleep.

To stop responding on a netif, run
  mdns_resp_remove_netif(struct netif *netif)


Adding services:
================

The netif first needs to be registered. Then run
  mdns_resp_add_service(struct netif *netif, char *name, char *service,
      u16_t proto, u16_t port, u32_t dns_ttl,
      service_get_txt_fn_t txt_fn, void *txt_userdata);

The name and service pointers will be copied. Name refers to the name of the
service instance, and service is the type of service, like _http
proto can be DNSSD_PROTO_UDP or DNSSD_PROTO_TCP which represent _udp and _tcp.
If this call returns successfully, the following queries will be answered:
- _services._dns-sd._udp.local type PTR returns <service>.<proto>.local
- <service>.<proto>.local type PTR returns <name>.<service>.<proto>.local
- <name>.<service>.<proto>.local type SRV returns hostname and port of service
- <name>.<service>.<proto>.local type TXT builds text strings by calling txt_fn
  with the supplied userdata. The callback adds strings to the reply by calling
  mdns_resp_add_service_txtitem(struct mdns_service *service, char *txt,
   int txt_len). Example callback method:

   static void srv_txt(struct mdns_service *service, void *txt_userdata)
   {
     res = mdns_resp_add_service_txtitem(service, "path=/", 6);
     LWIP_ERROR("mdns add service txt failed\n", (res == ERR_OK), return);
   }

  Since a hostname struct is used for TXT storage each single item can be max
  63 bytes long, and  the total max length (including length bytes for each
  item) is 255 bytes.

If your device runs a webserver on port 80, an example call might be:

  mdns_resp_add_service(netif, "myweb", "_http"
      DNSSD_PROTO_TCP, 80, 3600, srv_txt, NULL);

which will publish myweb._http._tcp.local for any hosts looking for web servers,
and point them to <hostname>.local:80

Relevant information will be sent as additional records to reduce number of
requests required from a client.

To remove a service from a netif, run
  mdns_resp_del_service(struct netif *netif, s8_t slot)
 */

/*

PS C:\Users\Development> Resolve-DnsName

cmdlet Resolve-DnsName at command pipeline position 1
Supply values for the following parameters:
Name: AR420_FFFF

Name                                           Type   TTL   Section    IPAddress
----                                           ----   ---   -------    ---------
AR420_FFFF.local                               A      120   Answer     192.168.1.100

 */

#include "lwip/apps/mdns.h"
#include "mdns_app.h"

#if LWIP_MDNS_RESPONDER
static void
srv_txt(struct mdns_service *service, void *txt_userdata)
{
  err_t res;
  LWIP_UNUSED_ARG(txt_userdata);
  
  res = mdns_resp_add_service_txtitem(service, "path=/", 6);
  LWIP_ERROR("mdns add service txt failed\n", (res == ERR_OK), return);

  res = mdns_resp_add_service_txtitem(service, "MAC=6C:93:08:42:FF:FF", 21);
  LWIP_ERROR("mdns add service txt failed\n", (res == ERR_OK), return);
}
#endif

#if LWIP_MDNS_RESPONDER
static void
mdns_report(struct netif* netif, u8_t result, s8_t service)
{
  LWIP_PLATFORM_DIAG(("mdns status[netif %d][service %d]: %d\n", netif->num, service, result));
}
#endif

void
mdns_init(void)
{
#if LWIP_MDNS_RESPONDER
  mdns_resp_register_name_result_cb(mdns_report);
  mdns_resp_init();
  mdns_resp_add_netif(netif_default, "AR420_FFFF");
  mdns_resp_add_service(netif_default, "AR420 Web-based Configuration", "_http", DNSSD_PROTO_TCP, 80, srv_txt, NULL);
  mdns_resp_announce(netif_default);
#endif
}
