/*
 * DMAIT - Deteccao e Mitigacao de Ataques de Interrupcao de Trafego Extension
 *
 * This code is based on the rpl-udp example from Contiki operating system.
 *
 * Created by Daniel Francis Soriano
 *
 */

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "contiki.h"
#include "dmait-logic.h"
#include "dmait-server-logic.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  0
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

#define SUSPECT_LIST_PRINT_INTERVAL          (60 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn_server;
static struct simple_udp_connection udp_conn_client;

PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);

/*---------------------------------------------------------------------------*/



static void
dmait_process_msg(const uip_ipaddr_t *sender_addr, const uint8_t *data, uint16_t datalen) {
    //LOG_INFO("PROCESSANDO MSG ");

    char stringHead[3];

    char msg[datalen+1];
    //strncpy(msg, (char *) data, datalen);
    memcpy(msg, data, datalen);
    msg[datalen] = '\0';

    //LOG_INFO_(" %s\n", msg);
    LOG_INFO("MENSAGEM RECEBIDA: \"%s\", DE: ", msg);
    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_("\n");

    getSubstring(stringHead, msg, 0, 2);
    stringHead[2] = '\0';

    if (stringHead != NULL) {

        //LOG_INFO("!!!!!!!!!!!!!!!!! CHEGOU NO PROCESSAMENTO NA RAIZ\n");

        if (strcmp(DMAIT_ROUTE_VALIDATION_HEADER, stringHead) == 0) {
            //LOG_INFO("PARSING MSG DMAIT-RV NA RAIZ\n");
            dmait_process_route_validation_message(sender_addr, msg, datalen, &udp_conn_client);

        } else if(strcmp(DMAIT_ALTERNATE_ROUTE_HEADER, stringHead) == 0) {
            //LOG_INFO("PARSING MSG DMAIT-AR NA RAIZ\n");
            dmait_process_alternate_route_message(sender_addr, (char *) data, datalen, &udp_conn_client);

        } else if(strcmp(DMAIT_DATA_MESSAGE_HEADER, stringHead) == 0) {
            //LOG_INFO("PARSING MSG DMAIT-DM NA RAIZ\n");
            dmait_process_data_message(sender_addr, (char *) data, datalen, &udp_conn_client);

        } else if(strcmp(DMAIT_FAULT_LOCATION_ACK_HEADER, stringHead) == 0) {
            //LOG_INFO("PARSING MSG DMAIT-FL-ACK NA RAIZ\n");
            dmait_process_fl_ack_message(sender_addr, (char *) data, datalen);

        } else {
            LOG_ERR("ERRO-CABECALHO DESCONHECIDO: %s\n", stringHead);
        }

    } else {
        LOG_ERR("ERRO-CABECALHO NULO");
    }

}



static void
udp_server_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  LOG_INFO("RECEBIDA NA RAIZ (SERVER) DE ");
  //LOG_INFO(" - MENSAGEM: '%s' from ", (char *) data);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");

  //LOG_INFO("DATALEN IS: %u\n", datalen);
#if WITH_SERVER_REPLY
  /* send back the same string to the client as an echo reply */
  LOG_INFO("Sending response.\n");
  simple_udp_sendto(&udp_conn, data, datalen, sender_addr);
#endif /* WITH_SERVER_REPLY */

  dmait_process_msg(sender_addr, data, datalen);

}

static void
udp_client_rx_callback(struct simple_udp_connection *c,
                       const uip_ipaddr_t *sender_addr,
                       uint16_t sender_port,
                       const uip_ipaddr_t *receiver_addr,
                       uint16_t receiver_port,
                       const uint8_t *data,
                       uint16_t datalen)
{
    //LOG_INFO("MENSAGEM RECEBIDA NA PORTA CLIENT DA RAIZ DE '%.*s' from ", datalen, (char *) data);
    LOG_INFO("RECEBIDA NA RAIZ (CLIENT) DE ");
    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_("\n");
    //LOG_INFO("DATALEN IS: %u\n", datalen);

    //dmait_process_msg(sender_addr, data, datalen);

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
    //Init memb structures
    dmait_init_memb();
    //Init lists
    dmait_init_server_lists();

    PROCESS_BEGIN();

    /* Initialize DAG root */
    NETSTACK_ROUTING.root_start();

    /* Initialize UDP server connection */
    simple_udp_register(&udp_conn_server, UDP_SERVER_PORT, NULL,
                  UDP_CLIENT_PORT, udp_server_rx_callback);

    /* Initialize UDP client connection */
    simple_udp_register(&udp_conn_client, UDP_CLIENT_PORT, NULL,
                        UDP_SERVER_PORT, udp_client_rx_callback);


    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
