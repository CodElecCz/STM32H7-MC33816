#include "ntp.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include <time.h>
#include <stdint.h>
#include <string.h>

#define NTP_PORT 123
#define NTP_TIMESTAMP_DELTA 2208988800ull // seconds between 1900 and 1970

static void ntp_server_recv(void *arg, struct udp_pcb *pcb,
                            struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    if (!p || p->len < 48) {
        if (p) pbuf_free(p);
        return; // NTP packets are 48 bytes minimum
    }

    // create NTP reply
    uint8_t reply[48] = {0};
    reply[0] = 0x24; // LI=0, VN=4, Mode=4 (server)
    
    // Get current time
    time_t now = time(NULL); // Make sure your system time is set!
    uint32_t seconds = (uint32_t)(now + NTP_TIMESTAMP_DELTA);

    // Write the transmit timestamp to the proper offset (40)
    reply[40] = (seconds >> 24) & 0xFF;
    reply[41] = (seconds >> 16) & 0xFF;
    reply[42] = (seconds >> 8) & 0xFF;
    reply[43] = seconds & 0xFF;

    // Normally, you'd set the fractional part too
    memset(&reply[44], 0, 4);

    struct pbuf *q = pbuf_alloc(PBUF_TRANSPORT, 48, PBUF_RAM);
    if (q) {
        memcpy(q->payload, reply, 48);
        udp_sendto(pcb, q, addr, port);
        pbuf_free(q);
    }
    pbuf_free(p);
}

void ntp_server_init(void)
{
    struct udp_pcb *pcb = udp_new();
    if (pcb) {
        if (udp_bind(pcb, IP_ADDR_ANY, NTP_PORT) == ERR_OK) {
            udp_recv(pcb, ntp_server_recv, NULL);
        } else {
            udp_remove(pcb);
        }
    }
}
