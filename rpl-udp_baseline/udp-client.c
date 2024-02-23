#include <stdio.h>

#include "contiki.h"
#include "net/routing/routing.h"
#include "net/routing/rpl-lite/rpl.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include "sys/log.h"
#include "node-id.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

#define SEND_INTERVAL		  (30 * CLOCK_SECOND)

/** linkaddr_t CMP MACRO */
#define lladdr_cmp(addr1, addr2) (memcmp(addr1, addr2, sizeof(linkaddr_t)) == 0)
/** linkaddr_t COPY MACRO */
#define lladdr_copy(dest, src) (*((linkaddr_t *)dest) = *((linkaddr_t *)src))




static struct simple_udp_connection udp_conn;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{

  LOG_INFO("RECEBIDA: \"%.*s\" from ", datalen, (char *) data);
  LOG_INFO_6ADDR(sender_addr);
#if LLSEC802154_CONF_ENABLED
  LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  LOG_INFO_("\n");

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic_timer;
  static unsigned count;
  static char str[43];
  uip_ipaddr_t dest_ipaddr;

  PROCESS_BEGIN();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);

  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
        snprintf(str, sizeof(str), "DM-%d-018-TESTE MSG DE DADOS;", count);


        //PARENT IP ADDRESS (to be used below)
        uip_ipaddr_t parentIp;
        uip_ipaddr_copy(&parentIp, rpl_neighbor_get_ipaddr(curr_instance.dag.preferred_parent));

        //PARENT LLADDR ADDRESS
        uip_lladdr_t parentLladdr;
        lladdr_copy(&parentLladdr, rpl_neighbor_get_lladdr(rpl_neighbor_get_from_ipaddr(&parentIp)));

        LOG_INFO("PAI ATUAL: ");
        LOG_INFO_6ADDR(&parentIp);
        LOG_INFO_(", (LLADDR): ");
        LOG_INFO_LLADDR(rpl_neighbor_get_lladdr(curr_instance.dag.preferred_parent));
        LOG_INFO_("\n");


        //[INFO: DMAIT     ] ENVIANDO TEXTO: "DM-0001-018-TESTE MSG DE DADOS;839e58fa11;", PARA: fd00::201:1:1:1


        //generating hash - 10 chars
        //format: <source node id-(2chars)> + <destination node id(2chars)> + count(4chars) + AA

        char senderStr[2 + 1];
        snprintf(senderStr, sizeof(senderStr), "%02d", node_id);
        strcat(str, senderStr);

        char destinationStr[2 + 1];
        snprintf(destinationStr, sizeof(destinationStr), "%02d", 1);
        strcat(str, destinationStr);

        char seqNumberDigits[4 + 1];
        snprintf(seqNumberDigits, sizeof(seqNumberDigits), "%04d", count);
        strcat(str, seqNumberDigits);

        strcat(str, "AA;");


        /* Send to DAG root */
        LOG_INFO("ENVIANDO TEXTO: \"%s\", PARA: ", str);
        LOG_INFO_6ADDR(&dest_ipaddr);
        LOG_INFO_("\n");

        simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
        count++;
    } else {
      LOG_INFO("Not reachable yet\n");
    }

    /* Add some jitter */
    etimer_set(&periodic_timer, SEND_INTERVAL
      - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
