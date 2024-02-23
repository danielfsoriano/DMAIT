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
#include <stdlib.h>
#include <string.h>

#include "contiki.h"
#include "dmait-logic.h"
#include "dmait-client-logic.h"
#include "net/routing/routing.h"
#include "net/routing/rpl-lite/rpl.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include "sys/log.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  0
#define UDP_CLIENT_PORT    8765
#define UDP_SERVER_PORT    5678

#define SEND_INTERVAL          (30 * CLOCK_SECOND)

#define DMAIT_ROUTE_VALIDATION_TIMEOUT (30 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn_client;
static struct simple_udp_connection udp_conn_server;


//Route Validation State
bool hasValidRoute = false;
bool validatingRoute = false;
//dmait_host_addr_t parentValidationHostAddr; //if "validatingRoute" == true, this holds the validating parent
linkaddr_t parentValidationLlAddr; //if "validatingRoute" == true, this holds the validating parent



/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);

/*---------------------------------------------------------------------------*/



static void
dmait_process_route_validation_message(const uip_ipaddr_t *sender_addr, char *data, uint16_t datalen) {

    LOG_PRINT("PROCESSANDO MENSAGEM DMAIT-RV\n");

    if (NETSTACK_ROUTING.node_is_reachable()) {

        dmait_triple_node_block_t dmaitTnb;

        uip_ipaddr_t prevIp;
        uip_ipaddr_copy(&prevIp, sender_addr);
        extractHostAddrFromIpv6(&dmaitTnb.prev_node_addr, &prevIp); //prev node addr

        //THIS NODE ADDRESS
        uip_ipaddr_t currIp;
        uip_ipaddr_copy(&currIp, rpl_get_global_address());
        extractHostAddrFromIpv6(&dmaitTnb.node_addr, &currIp); //curr node addr

        //PARENT ADDRESS
        uip_ipaddr_t parentIp;
        uip_ipaddr_copy(&parentIp, rpl_neighbor_get_ipaddr(curr_instance.dag.preferred_parent));
        extractHostAddrFromIpv6(&dmaitTnb.next_node_addr, &parentIp);

        //LINK LOCAL PARENT ADDRESS
        uip_ipaddr_t linkLocalParentIp;
        uip_ipaddr_copy(&linkLocalParentIp, rpl_neighbor_get_ipaddr(curr_instance.dag.preferred_parent));
        uip_create_linklocal_prefix(&linkLocalParentIp);

        //ELIMINA CICLO NO DMAIT-RV E ABORTA O PROCESSO
        if (dmait_host_addr_cmp(&dmaitTnb.prev_node_addr, &dmaitTnb.next_node_addr)) {
            //The next node (parent) is the same as the previous node.. so break the cicle and ignore process.
            LOG_ERR("CICLO DETECTADO AO REPASSAR DMAIT-RV - PROXIMO NO EH O ANTERIOR - ");
            LOG_ERR_6ADDR(&linkLocalParentIp);
            LOG_ERR_(" - ABORTANDO DMAIT-RV\n");
            return;
        }

        //CALCULATE HMAC (for real)
        char nodeKey[DMAIT_HOST_ADDR_LENGTH + 1];
        dmait_get_current_node_key(nodeKey, sizeof(nodeKey));
        dmait_calculate_hmac_from_triple_node_block(dmaitTnb.hmac_digest,
                                                    sizeof(dmaitTnb.hmac_digest),
                                                    &dmaitTnb,
                                                    nodeKey);

        dmait_print_triple_node_block(dmaitTnb);

        //uint16_t buffLen = datalen + 73;
        uint16_t buffLen = datalen + DMAIT_RV_BLOCK_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR) + 1;
        char buf[buffLen];
        memcpy(buf, data, datalen);
        //strncpy(buf, (char *) data, datalen);
        buf[datalen] = '\0';
        //strcat(buf, ";");

        //    LOG_INFO("@#@#@#@ strlen(data): %d\n", (int) strlen((char *) data));
        //    LOG_INFO("@#@#@#@ datalen: %d\n", (int) datalen);
        //    LOG_INFO("@#@#@#@ bufflen: %d\n", (int) buffLen);
        //    LOG_INFO("\n@#@#@#@ BUF APOS COPIA: %s\n", buf);

        dmait_route_validation_add_block(buf, buffLen, &dmaitTnb);

        //    LOG_INFO("\n@#@#@#@ BUF APOS ADICAO DE BLOCO: %s\n", buf);
        //    LOG_INFO("@#@#@#@ strlen(buf) apos adicao: %d\n", (int) strlen(buf));

        LOG_INFO("REPASSANDO MENSAGEM DMAIT-RV PARA O PAI: ");
        LOG_INFO_6ADDR(&linkLocalParentIp);
        LOG_INFO_("\n");

        //SEND RV MSG TO PARENT
        //dmait_send_data(&udp_conn_client, buf, strlen(buf), &linkLocalParentIp);
        dmait_send_text_data(&udp_conn_client, buf, &linkLocalParentIp);

    } else {
        //Node is not reacheable
        LOG_ERR("NAO CONSEGUE REPASSAR MENSAGEM DMAIT-RV PARA O PAI - NAO POSSUI PAI NO MOMENTO\n");
    }


}


static void
dmait_process_route_validation_ack_message(const uip_ipaddr_t *sender_addr, char *data, uint16_t datalen) {

    LOG_INFO("PROCESSANDO MENSAGEM RVACK\n");

    validatingRoute = false;

    //Validate RVACK HMAC
    char payload[datalen];
    getSubstring(payload, data, 0, datalen-10);  //payload without hmac
    payload[datalen-10] = '\0';

    char receivedHmac[11];
    getSubstring(receivedHmac, data, datalen-10, 10); //only hmac
    receivedHmac[10] = '\0';

    //printf("&&&&&&&&&&&&&&&&&&&&&&  RVACK - PAYLOAD: %s\n", payload);

    char nodeKey[25];
    dmait_get_current_node_key(nodeKey, sizeof(nodeKey));
    char hmacStr[11];
    dmait_generate_hmac(payload, nodeKey, hmacStr, sizeof(hmacStr));

    //printf("&&&&&&&&&&&&&&&&&&&&&&  RVACK - RECEIVED HMAC: %s, CALCULATED HMAC: %s\n", receivedHmac, hmacStr);
    if (strcmp(hmacStr, receivedHmac) == 0) {
        LOG_INFO("RVACK - HMAC VALIDADO\n");
        hasValidRoute = true;

        //Add each node from the valid route

        //Extracts the amount of nodes in the valid route list
        int amountStartPosition = strlen(DMAIT_ROUTE_VALIDATION_ACK_HEADER) + strlen(DMAIT_HEADER_SEPARATOR);
        char addrAmountStr[3];
        getSubstring(addrAmountStr, data, amountStartPosition, 2);
        addrAmountStr[2] = '\0';

        char *endPtr;
        int addrAmount = strtol(addrAmountStr, &endPtr, 10);

        //printf(")))))))))))))))))))))))))))  addrAmountStr: %s, int: %d\n", addrAmountStr, addrAmount);

        //THIS NODE ADDRESS
        uip_ipaddr_t currIp;
        uip_ipaddr_copy(&currIp, rpl_get_global_address());
        dmait_host_addr_t thisHostAddr;
        extractHostAddrFromIpv6(&thisHostAddr, &currIp); //curr node addr

        //clear the valid route list
        dmait_clear_valid_route();

        int addrStartPosition = strlen(DMAIT_ROUTE_VALIDATION_ACK_HEADER) + strlen(DMAIT_HEADER_SEPARATOR) + 2 + strlen(DMAIT_HEADER_SEPARATOR);
        int x;
        for (x = 0; x < addrAmount; x++) {
            char hostAddrStrAux[DMAIT_HOST_ADDR_LENGTH + 1];
            getSubstring(hostAddrStrAux, payload, addrStartPosition + (x * (DMAIT_HOST_ADDR_LENGTH + 1)), DMAIT_HOST_ADDR_LENGTH);
            hostAddrStrAux[DMAIT_HOST_ADDR_LENGTH] = '\0';
            //printf(">>>>>>>>>>>>>>  HOST %d: %s\n", x, hostAddrStrAux);
            dmait_host_addr_t hostAddrAux;
            dmait_build_host_addr_from_block_field(hostAddrStrAux, &hostAddrAux);
            //if (dmait_host_addr_cmp(&thisHostAddr, &hostAddrAux)) {
            if (x > 0) { //discards the first because it is this node
                if (x == 1) { //parent host addr
                    //Saves the validated parent host addr
                    //dmait_host_addr_copy(&parentValidationHostAddr, &hostAddrAux);

                    //Saves the validated parent lladdr
                    uip_ipaddr_t nodeIp;
                    //buildDS6DefaultPrefixAddr(&nodeIp, &hostAddrAux);
                    buildLinkLocalUnicastAddr(&nodeIp, &hostAddrAux);

                    rpl_nbr_t * nbr = rpl_neighbor_get_from_ipaddr(&nodeIp);
                    if (nbr != NULL) {
                        const linkaddr_t *parentLladdr = rpl_neighbor_get_lladdr(nbr);
                        if (parentLladdr != NULL) {
                            lladdr_copy(&parentValidationLlAddr, parentLladdr);
                            //LOG_INFO("PARENT LLADDR SAVED!\n");
                        } else {
                            LOG_ERR("PARENT LLADDR NULL\n");
                        }
                    } else {
                        LOG_ERR("NBR NULL\n");
                    }
                }
                //do not add the first node host addr to the validated route
                dmait_valid_route_add_node_by_adddr(&hostAddrAux);
            }
        }
        dmait_print_valid_route();
        //printf("TAMANHO DA LISTA DE ROTA VALIDA: %d\n", dmait_valid_route_depth());
    } else {
        LOG_ERR("RVACK - HMAC INVALIDO - DESCARTANDO RVACK!\n");
    }
}


static void
dmait_process_alternate_route_message(const uip_ipaddr_t *sender_addr, char *data, uint16_t datalen) {

    LOG_INFO("PROCESSANDO MENSAGEM AR\n");

    //Sums the header length, plus the "AVOID" header and 2 chars for "avoid route" length (up to 99),
    int headerEnd = strlen(DMAIT_ALTERNATE_ROUTE_HEADER) + strlen(DMAIT_HEADER_SEPARATOR) //header
                    + strlen(DMAIT_ALTERNATE_ROUTE_AVOID_HEADER) + strlen(DMAIT_HEADER_SEPARATOR);

    //Extracts the amount of nodes in the avoid route list
    char avoidNodesAmountStr[3];
    getSubstring(avoidNodesAmountStr, data, headerEnd, 2);
    avoidNodesAmountStr[2] = '\0';

    char *endPtr = NULL;
    int avoidNodesAmount = strtol(avoidNodesAmountStr, &endPtr, 10);

    int avoidNodesStart = headerEnd + 2 + strlen(DMAIT_HEADER_SEPARATOR);

    //printf("############ avoidNodesAmountStr: %s, int: %d\n", avoidNodesAmountStr, avoidNodesAmount);

    //calculates the amount of route nodes
    int routeNodesStart = avoidNodesStart + (avoidNodesAmount * (DMAIT_HOST_ADDR_LENGTH)) + ((avoidNodesAmount -1) * strlen(DMAIT_FIELD_SEPARATOR)) + strlen(DMAIT_BLOCK_SEPARATOR)
                          + DMAIT_HMAC_LENGTH + strlen(DMAIT_FIELD_SEPARATOR)
                          + strlen(DMAIT_ALTERNATE_ROUTE_ROUTE_HEADER) + strlen(DMAIT_BLOCK_SEPARATOR);

    int routeLength = 1;
    while ((routeNodesStart + (routeLength * (DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_FIELD_SEPARATOR)))) < datalen) {
        routeLength++;
    }

    //Creates a list with all the nodes to avoid (AVOID + ROUTE nodes).
    //The ROUTE nodes are added so there are no cycles in the route
    dmait_host_addr_t avoidRouteArray[avoidNodesAmount + routeLength];

    char currNodeHostAddr[DMAIT_HOST_ADDR_LENGTH + 1];
    int x;
    for (x = 0; x < avoidNodesAmount; x++) {
        getSubstring(currNodeHostAddr, data, avoidNodesStart + (x * (DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_FIELD_SEPARATOR))), DMAIT_HOST_ADDR_LENGTH);
        //LOG_INFO("########### currAvoidNodeHostAddr-%d: %s\n", x, currNodeHostAddr);
        dmait_build_host_addr_from_block_field(currNodeHostAddr, &avoidRouteArray[x]);
    }

    //printf("avoidNodesAmount: %d\n", avoidNodesAmount);

    //GET THIS NODE HOSTADDR
    uip_ipaddr_t currIp;
    uip_ipaddr_copy(&currIp, rpl_get_global_address());
    dmait_host_addr_t thisHostAddr;
    extractHostAddrFromIpv6(&thisHostAddr, &currIp);

    for (x = 0; x < routeLength; x++) {
        getSubstring(currNodeHostAddr, data, routeNodesStart + (x * (DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_FIELD_SEPARATOR))), DMAIT_HOST_ADDR_LENGTH);
        //LOG_INFO("########### avoidNodesStart-%d: %s\n", routeLength, currNodeHostAddr);
        dmait_build_host_addr_from_block_field(currNodeHostAddr, &avoidRouteArray[avoidNodesAmount + x]);

        if (dmait_host_addr_cmp(&thisHostAddr, &avoidRouteArray[avoidNodesAmount + x])) {
            //This node is already in the current route.. it is a cycle...
            //Discards this message
            LOG_ERR("DMAIT-AR - CICLO DETECTADO - DESCARTANDO MENSAGEM\n");
            return;
        }
    }
    //printf("routeLength: %d\n", routeLength);

//    printf("LIST OF AVOID NODES + ROUTE NODES\n");
//    for (x=0; x < avoidNodesAmount + routeLength; x++) {
//        dmait_print_host_addr(&avoidRouteArray[x]);
//        printf("\n");
//    }

    //Select a neighbor not in the validated route
    uip_ipaddr_t targetNeighborIpAddr;

    dmait_host_addr_t selectedNeighborHostAddr;
    if (dmait_select_neighbor_not_in_array(&selectedNeighborHostAddr, avoidRouteArray, avoidNodesAmount + routeLength)) {
        //printf("@@@@@@@@ DMAIT-AR FOUND NEIGHBOR: ");
        //dmait_print_host_addr(&selectedNeighborHostAddr);
        //printf("\n");

        buildLinkLocalUnicastAddr(&targetNeighborIpAddr, &selectedNeighborHostAddr);

    } else {
        //if found no suitable neighbor, send it to the parent anyway
        //PARENT ADDRESS
        uip_ipaddr_copy(&targetNeighborIpAddr, rpl_neighbor_get_ipaddr(curr_instance.dag.preferred_parent));
        uip_create_linklocal_prefix(&targetNeighborIpAddr);

        LOG_INFO("DMAIT-AR - NENHUM VIZINHO APROPRIADO - ENVIANDO AO PAI: ");
        LOG_INFO_6ADDR(rpl_neighbor_get_ipaddr(curr_instance.dag.preferred_parent));
        printf("\n");

    }

    int addedHostLenght = DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_FIELD_SEPARATOR) + 1;

    char payload[datalen + addedHostLenght];
    strncpy(payload, data, datalen);
    payload[datalen] = '\0'; //just in case

    //THIS NODE ADDRESS
    char thisNodeAddr[addedHostLenght];
    dmait_snprint_host_addr(thisNodeAddr, sizeof(thisNodeAddr), &thisHostAddr);
    //dmait_snprint_current_host_addr(thisNodeAddr, sizeof(thisNodeAddr));
    strcat(thisNodeAddr, DMAIT_FIELD_SEPARATOR);
    payload[datalen + addedHostLenght - 1] = '\0';
    strcat(payload, thisNodeAddr);

    //printf("@@@@@@ PAYLOAD: %s\n", payload);

    //Transmit the data
    LOG_INFO("ENVIANDO DMAIT-AR AO NO: ");
    LOG_INFO_6ADDR(&targetNeighborIpAddr);
    LOG_INFO_("\n");

    //dmait_send_data(&udp_conn_client, payload, strlen(payload), &targetNeighborIpAddr);
    dmait_send_text_data(&udp_conn_client, payload, &targetNeighborIpAddr);
}


static void
dmait_process_msg(const uip_ipaddr_t *sender_addr, const uint8_t *data, uint16_t datalen) {
    //LOG_INFO("PROCESSANDO MSG ");

//    char *stringHead;
//    char *headState;
//
//    char msg[datalen+1];
//    memcpy(msg, data, datalen);
//    msg[datalen] = '\0';
//
//    LOG_INFO_(" %s\n", msg);
//
//    stringHead = strtok_r(msg, DMAIT_HEADER_SEPARATOR, &headState);
//    LOG_INFO("HEADER: %s\n", stringHead);

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
        //LOG_INFO("!!!!!!!!!!!!!!!!! CHEGOU NO PROCESSAMENTO\n");

        if (strcmp(DMAIT_ROUTE_VALIDATION_HEADER, stringHead) == 0) {
            //LOG_INFO("PARSING MSG DMAIT - RV\n");
            dmait_process_route_validation_message(sender_addr, msg, sizeof(msg));

        } else if(strcmp(DMAIT_ROUTE_VALIDATION_ACK_HEADER, stringHead) == 0) {
            //LOG_INFO("PARSING MSG DMAIT - RV-ACK\n");
            dmait_process_route_validation_ack_message(sender_addr, (char *) data, datalen);

         } else if(strcmp(DMAIT_ALTERNATE_ROUTE_HEADER, stringHead) == 0) {
            //LOG_INFO("PARSING MSG DMAIT - AR\n");
            dmait_process_alternate_route_message(sender_addr, (char *) data, datalen);

        } else if(strcmp(DMAIT_DATA_MESSAGE_ACK_HEADER, stringHead) == 0) {
            //LOG_INFO("PARSING MSG DMAIT - DM-ACK \n");
            dmait_process_data_ack_message(sender_addr, (char *) data, datalen);

        } else if(strcmp(DMAIT_FAULT_LOCATION_PROBE_HEADER, stringHead) == 0) {
            //LOG_INFO("PARSING MSG DMAIT - FL-PROBE \n");
            dmait_process_fl_probe_message(sender_addr, (char *) data, datalen, &udp_conn_client);

        } else {
            LOG_ERR("ERRO AO PROCESSAR MSG DMAIT - CABECALHO DESCONHECIDO: %s\n", stringHead);
            //LOG_INFO("MENSAGEM: %s\n", msg);
        }

    } else {
        LOG_ERR("ERRO AO PROCESSAR MSG DMAIT - CABECALHO NULO");
    }
}

static void
udp_client_rx_callback(struct simple_udp_connection *c,
                       const uip_ipaddr_t *sender_addr,
                       uint16_t sender_port,
                       const uip_ipaddr_t *receiver_addr,
                       uint16_t receiver_port,
                       const uint8_t *data,
                       uint16_t datalen) {

    //LOG_INFO("MENSAGEM RECEBIDA COMO CLIENT '%.*s' DE ", datalen, (char *) data);
    LOG_INFO("MENSAGEM RECEBIDA NA PORTA CLIENT DE ");
    LOG_INFO_6ADDR(sender_addr);
#if LLSEC802154_CONF_ENABLED
    LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
    LOG_INFO_("\n");

}


static void
udp_server_rx_callback(struct simple_udp_connection *c,
                       const uip_ipaddr_t *sender_addr,
                       uint16_t sender_port,
                       const uip_ipaddr_t *receiver_addr,
                       uint16_t receiver_port,
                       const uint8_t *data,
                       uint16_t datalen) {

    LOG_INFO("MENSAGEM RECEBIDA NA PORTA SERVER DE: ");
    //LOG_INFO(" '%.*s' DE ", datalen, (char *) data);
    LOG_INFO_6ADDR(sender_addr);
#if LLSEC802154_CONF_ENABLED
    LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
    LOG_INFO_("\n");

    dmait_process_msg(sender_addr, data, datalen);
}


void
ctimer_route_validation_timeout_callback(void *ptr) {

    if (!hasValidRoute && validatingRoute) {

        printf("VALIDACAO DE ROTA TIMEOUT - ADICIONANDO PAI NA LISTA NEGRA\n");

        validatingRoute = false; //diz que nao esta mais validando rota
/*
        //Adds the parent host addr to the blacklist
        if (!is_node_addr_in_dmait_node_blacklist(&parentValidationHostAddr)) {
            dmait_node_blacklist_add_node_by_adddr( &parentValidationHostAddr);
        }
*/
        //Add node to the RPL OF dmait-blacklist, so the blacklisted node cannot parent this node again
        //printf("@@@@@@@@@@@@@ Adicionando lladdr do no a lista negra do RPL-OF\n");
        //uip_ip6addr_t nbrIp;
        //buildLinkLocalUnicastAddr(&nbrIp, &parentValidationHostAddr);

        //CAUSAVA OS TRAVAMENTOS POIS ENVIAVA O ENDERECO DO PAI NULO
        //curr_instance.of->addNodeToBlackList(rpl_neighbor_get_lladdr(rpl_neighbor_get_from_ipaddr(&nbrIp)));
        curr_instance.of->addNodeToBlackList(&parentValidationLlAddr);

    } // else... (has a valid route)... just discard this timer

    /* rearm the ctimer */
    //ctimer_reset(&timer_ctimer);

}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data) {
    static struct etimer periodic_timer;
    static struct ctimer route_validation_timeout_timer;
    static int unreachableCounter = 0;

    //static unsigned count;
    uip_ipaddr_t root_ipaddr;
    //uip_ipaddr_t another_ipaddr;
    //static bool print = true;


    PROCESS_BEGIN();

/*
                dmait_host_addr_t dmaitHostAddr;
                dmait_hostAddr(&dmaitHostAddr, 0x333, 0x2,0x2,0x2);
                buildLinkLocalUnicastAddr(&another_ipaddr, &dmaitHostAddr);
*/

                /* Initialize UDP client connection */
                simple_udp_register(&udp_conn_client, UDP_CLIENT_PORT, NULL,
                                    UDP_SERVER_PORT, udp_client_rx_callback);

                /* Initialize UDP server connection */
                simple_udp_register(&udp_conn_server, UDP_SERVER_PORT, NULL,
                                    UDP_CLIENT_PORT, udp_server_rx_callback);

                etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);


                //LOG_INFO("TAMANHO DE \"COOJA_RADIO_CONF_BUFSIZE\": %d\n", (int) COOJA_RADIO_CONF_BUFSIZE);
                //LOG_INFO("TAMANHO DE \"PACKETBUF_CONF_SIZE\": %d\n", (int) PACKETBUF_CONF_SIZE);

                while (1) {
                    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

                    if (NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&root_ipaddr)) {
                        //resets the unreacheable counter
                        unreachableCounter = 0;

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

                        //Check if has a valid route, and start route validation if not.
                        if (!hasValidRoute && !validatingRoute) {
                            LOG_INFO("INICIANDO VALIDACAO DE ROTA\n");
                            validatingRoute = true;
                            resetUnackedDataMessagesCounter();

                            //Build the payload and transmit it
                            dmait_rv_start_route_validation(&udp_conn_client);

                            //Saves parent host addr in the validation variable
                            lladdr_copy(&parentValidationLlAddr, &parentLladdr);
                            //extractHostAddrFromIpv6(&parentValidationHostAddr, &parentIp);

                            //TODO Avaliar se os times estao sendo sobrescritos toda vez que manda uma msg
                            //Talvez o ideal seria ter uma lista de timers (route_validation_timeout_timer) e ir reciclando
                            //Start a timeout timer
                            ctimer_set(&route_validation_timeout_timer, 40 * CLOCK_SECOND, ctimer_route_validation_timeout_callback, NULL);

                        }

                        if (hasValidRoute) {

                            //Verifica se o pai foi alterado pelo RPL
                            //if (!dmait_is_host_addr_equal_ip(&parentValidationHostAddr, &parentIp)) {
                            if (!lladdr_cmp(&parentLladdr, &parentValidationLlAddr)) {

                                //se o pai foi alterado pede nova validacao
                                LOG_PRINT("PAI ALTERADO PELO RPL - INVALIDANDO ROTA ATUAL\n");

                                hasValidRoute = false;
                                validatingRoute = false; //just in case

                                //Sai da rede (avisa os filhos da mudanca) e inicia o processo de pedir a revalidacao de rota
                                NETSTACK_ROUTING.leave_network();
                                //Alternativa segura seria avisar a raiz que houve a mudanca (com hmac) e a raiz
                                //avisar aos nos que todos que tiverem esse no na sua rota validada devem revalidar

                            } else {
                                //Pai nao mudou, entao a rota continua valida

                                //Envia mensagem de dados
                                LOG_PRINT("POSSUI ROTA VALIDADA - ENVIANDO MENSAGEM DE DADOS\n");

                                char str[100];
                                snprintf(str, sizeof(str), "TESTE MSG DE DADOS");
                                //dmait_send_data(&udp_conn_client, str, strlen(str), &root_ipaddr);
                                dmait_send_data_message(&udp_conn_client, str, strlen(str), &root_ipaddr);

                            }
                        }

                    } else {
                        //increments the unreacheable counter
                        unreachableCounter++;

                        if (unreachableCounter > 300) {
                            //clears the blacklist
                            curr_instance.of->clearBlackList();
                        }

                        //triggers the validating route process
                        if (!validatingRoute) {
                            LOG_PRINT("NO FORA DA REDE - INVALIDANDO ROTA ATUAL (SE HOUVER)\n");
                            hasValidRoute = false;
                        } else {
                            LOG_INFO("NO FORA DA REDE\n");
                        }
                    }

                    /* Add some jitter */
                    etimer_set(&periodic_timer, SEND_INTERVAL - CLOCK_SECOND + (random_rand() % (5 * CLOCK_SECOND)));
                }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
