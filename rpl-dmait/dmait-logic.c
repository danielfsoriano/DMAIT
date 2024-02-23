/*
 * DMAIT - Deteccao e Mitigacao de Ataques de Interrupcao de Trafego Extension
 *
 * This code is based on the rpl-udp example from Contiki operating system.
 *
 * Created by Daniel Francis Soriano
 *
 */

#include "contiki.h"
#include "dmait-logic.h"

#include "random.h"

#include "hmac_sha256.h"

#include "lib/memb.h"
#include "sys/node-id.h"
#include "net/netstack.h"
#include "net/ipv6/uiplib.h"
#include "net/ipv6/simple-udp.h"
#include "net/routing/routing.h"
#include "net/routing/rpl-lite/rpl-neighbor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "DMAIT"
#define LOG_LEVEL LOG_LEVEL_INFO


/* Only instance of route list per node */
//LIST(dmait_routes);

//MEMB dynamic memory allocation definition for dmait_triple_node_block structure
//MEMB(triplenodeblockmemb, struct dmait_triple_node_block, 21);



/*

void dmait_init_routes() {
    list_init(dmait_routes);
}



int dmait_routes_length() {
   return list_length(dmait_routes);
}



void dmait_add_route(dmait_route_t *route) {
    list_add(dmait_routes, route);
}



void dmait_remove_route(dmait_route_t *route) {
    list_remove(dmait_routes, route);
}



dmait_route_t * dmait_get_route_by_id(uint8_t route_id) {
    dmait_route_t *curr;
    curr = list_head(dmait_routes);
    while (curr != NULL) {
        if(curr->route_id == route_id) {
            return curr;
        }
        curr = list_item_next(curr);
    }
    return NULL;
}



dmait_route_t * dmait_get_first_route_by_ip_addr(uip_ip6addr_t *ip_addr) {
    dmait_route_t *curr;
    curr = list_head(dmait_routes);
    while (curr != NULL) {
        dmait_ipaddr_t *dmait_ip = list_head(curr->ip_list);
        while (dmait_ip != NULL) {
            if (uip_ipaddr_cmp(ip_addr, &dmait_ip->ip_addr)) {
                return curr;
            }
            dmait_ip = list_item_next(dmait_ip);
        }
        curr = list_item_next(curr);
    }
    return NULL;
}



void dmait_update_route_parents(uip_ip6addr_t *child_ip, uip_ip6addr_t *new_parent_ip) {
    LOG_INFO("ATUALIZANDO PAIS EM TODAS AS ROTAS\n");
    printf("FILHO: ");
    LOG_INFO_6ADDR(child_ip);
    printf("\nNOVO PAI: ");
    LOG_INFO_6ADDR(new_parent_ip);
    printf("\n");

    dmait_route_t *curr_route;
    curr_route = list_head(dmait_routes);
    while (curr_route != NULL) {
        printf("PRIMEIRO WHILE\n");
        dmait_ipaddr_t *dmait_ip = list_head(curr_route->ip_list);
        while (dmait_ip != NULL) {
            printf("SEGUNDO WHILE\n");
            //Se achou o ip do filho na lista...
            if (uip_ipaddr_cmp(child_ip, &dmait_ip->ip_addr)) {
                LOG_INFO("ENCONTRADO FILHO NA ROTA: ");
                LOG_INFO_6ADDR(child_ip);
                LOG_INFO("\n");

                //Pega o antigo ip do pai (proximo da lista)
                dmait_ipaddr_t *old_parent_dmait_ip = list_item_next(dmait_ip);
                if (old_parent_dmait_ip != NULL) {
                    //Se tem antigo pai...
                    LOG_INFO("POSSUI ANTIGO PAI\n");

                    if (!uip_ipaddr_cmp(new_parent_ip, &old_parent_dmait_ip->ip_addr)) {
                        LOG_INFO("ANTIGO PAI DIFERENTE DO NOVO\n");

                        //Se o antigo pai eh diferente do novo...
                        //apaga todos os pais
                        dmait_ipaddr_t * last = list_tail(curr_route->ip_list);
                        //while (!uip_ipaddr_cmp(&dmait_ip->ip_addr, &last->ip_addr)) { //Se ainda nao chegou no ip atual
                        while (last != dmait_ip) { //se ainda nao chegou no ip atual
                            list_chop(curr_route->ip_list);
                            last = list_tail(curr_route->ip_list);
                        }
                        //Adiciona o novo pai ao final da lista
                        dmait_ipaddr_t * new_parent_dmait_ip;
                        new_parent_dmait_ip = (dmait_ipaddr_t *) malloc(sizeof(dmait_ipaddr_t));
                        memcpy(&new_parent_dmait_ip->ip_addr, new_parent_ip, sizeof(uip_ip6addr_t));
                        LOG_INFO("A ROTA CONTEM: %i IPs\n", list_length(curr_route->ip_list));
                        list_add(curr_route->ip_list, new_parent_dmait_ip);
                        LOG_INFO("PAI ADICIONADO\n");
                        LOG_INFO("A ROTA CONTEM: %i IPs\n", list_length(curr_route->ip_list));

                        break;
                    } else {
                        //se tiver antigo pai e for igual ao novo, nao modifica os pais
                        LOG_INFO("ANTIGO PAI IGUAL AO NOVO\n");
                        break;
                    }
                } else {
                    LOG_INFO("NAO POSSUI ANTIGO PAI\n");
                    //nao tem antigo pai
                    //Adiciona o novo pai ao final da lista
                    dmait_ipaddr_t * new_parent_dmait_ip;
                    new_parent_dmait_ip = (dmait_ipaddr_t *) malloc(sizeof(dmait_ipaddr_t));
                    memcpy(&new_parent_dmait_ip->ip_addr, new_parent_ip, sizeof(uip_ip6addr_t));
                    LOG_INFO("A ROTA CONTEM: %i IPs\n", list_length(curr_route->ip_list));
                    list_add(curr_route->ip_list, new_parent_dmait_ip);
                    LOG_INFO("PAI ADICIONADO\n");
                    LOG_INFO("A ROTA CONTEM: %i IPs\n", list_length(curr_route->ip_list));
                    break;
                }
            }
            dmait_ip = list_item_next(dmait_ip);
        }
        curr_route = list_item_next(curr_route);
    }
}


*/


char *getSubstring(char *dst, const char *src, size_t start, size_t lenght) {
    return strncpy(dst, src + start, lenght);
}

int my_strlen(const char *s, int max) {
    int count;
    for (count = 0; count <= max; count++) {
        if (*s == '\0') {
            return count;
        }
        s++;
    }
    return -1;
}

void dmait_replace_parent(rpl_nbr_t *nbr) {
    //printf("Checking parent switch\n");
    if(curr_instance.dag.preferred_parent != nbr) {
        LOG_INFO("TROCANDO PAI: ");
        LOG_INFO_6ADDR(rpl_neighbor_get_ipaddr(curr_instance.dag.preferred_parent));
        LOG_INFO_(" -> ");
        LOG_INFO_6ADDR(rpl_neighbor_get_ipaddr(nbr));
        LOG_INFO_("\n");

#ifdef RPL_CALLBACK_PARENT_SWITCH
        RPL_CALLBACK_PARENT_SWITCH(curr_instance.dag.preferred_parent, nbr);
#endif /* RPL_CALLBACK_PARENT_SWITCH */

        /* Always keep the preferred parent locked, so it remains in the
         * neighbor table. */
        nbr_table_unlock(rpl_neighbors, curr_instance.dag.preferred_parent);
        nbr_table_lock(rpl_neighbors, nbr);
        LOG_INFO("Removing old parent\n");
        nbr_table_remove(rpl_neighbors, curr_instance.dag.preferred_parent);

        /* Update DS6 default route. Use an infinite lifetime */
        uip_ds6_defrt_rm(uip_ds6_defrt_lookup(
                rpl_neighbor_get_ipaddr(curr_instance.dag.preferred_parent)));
        uip_ds6_defrt_add(rpl_neighbor_get_ipaddr(nbr), 0);

        curr_instance.dag.preferred_parent = nbr;
        curr_instance.dag.unprocessed_parent_switch = true;
    }
}


void dmait_print_nbr_list() {
    if(curr_instance.used) {
        int curr_dio_interval = curr_instance.dag.dio_intcurrent;
        int curr_rank = curr_instance.dag.rank;
        rpl_nbr_t *nbr = nbr_table_head(rpl_neighbors);

        LOG_INFO("### LISTA DE VIZINHOS\n");
        LOG_INFO("NODE: %i, ", node_id);
        LOG_INFO_6ADDR(rpl_get_global_address());
        LOG_INFO_(",\t DAG state: %s,\t MOP %u \tOCP %u\t rank %u,\t dioint %u,\t nbr count %u\n",
                  rpl_dag_state_to_str(curr_instance.dag.state),
                  curr_instance.mop, curr_instance.of->ocp, curr_rank,
                  curr_dio_interval, rpl_neighbor_count());

        rpl_nbr_t *best_candidate = NULL;

        while(nbr != NULL) {
            char is_parent[50] = " ";
            if (rpl_neighbor_is_parent(nbr)) {
                strcpy(is_parent, "(PAI)");
            } else {
                if (rpl_neighbor_is_acceptable_parent(nbr)) {
                    strcpy(is_parent, "(CANDIDATO [OF])");
                } else {
                    strcpy(is_parent, "(NAO CANDIDATO [OF])");
                }
                if (nbr->rank <= curr_rank) {
                    strcat(is_parent, "(CANDIDATO [DMAIT])");
                    //if (best_candidate == NULL || (best_candidate->rank > nbr->rank && rpl_neighbor_is_reachable(nbr))) {
                    if (best_candidate == NULL || (best_candidate->rank > nbr->rank)) {
                        best_candidate = nbr;
                    }
                } else {
                    strcat(is_parent, "(NAO CANDIDATO [DMAIT])");
                }
            }

            //Gera String com o IP do vizinho
            char nbr_ip[UIPLIB_IPV6_MAX_STR_LEN];
            uiplib_ipaddr_snprint(nbr_ip, sizeof(nbr_ip), rpl_neighbor_get_ipaddr(nbr));


            char is_root[8] = " ";
            if (nbr->rank == ROOT_RANK) strcpy(is_root,"(RAIZ)");
            LOG_INFO("NBR IP: %s RANK: %i %s %s\n", nbr_ip ,nbr->rank, is_parent, is_root);

//            char buf[120];
//            rpl_neighbor_snprint(buf, sizeof(buf), nbr);
//            LOG_INFO("nbr: %s\n", buf);
            nbr = nbr_table_next(rpl_neighbors, nbr);
        }

        if (best_candidate != NULL) {
            //TROCA O PAI
            LOG_INFO("TROCANDO O PAI! - NOVO PAI: ");
            LOG_INFO_6ADDR(rpl_neighbor_get_ipaddr(best_candidate));
            LOG_INFO("\n");
            //rpl_neighbor_set_preferred_parent(best_candidate);
            dmait_replace_parent(best_candidate);
        }
        LOG_INFO("### SEM MAIS VIZINHOS\n");
    }
}
/*---------------------------------------------------------------------------*/
/*
void ipv6_expander(uip_ipaddr_t *addr) {
    char str[40];
    sprintf(str,"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
            (int)addr->u8[0], (int)addr->u8[1],
            (int)addr->u8[2], (int)addr->u8[3],
            (int)addr->u8[4], (int)addr->u8[5],
            (int)addr->u8[6], (int)addr->u8[7],
            (int)addr->u8[8], (int)addr->u8[9],
            (int)addr->u8[10], (int)addr->u8[11],
            (int)addr->u8[12], (int)addr->u8[13],
            (int)addr->u8[14], (int)addr->u8[15]);
    printf("$$$$$ EXPANDED ipv6 Addr %s\n",str);
}
*/

void dmait_snprint_host_addr(char *buf, size_t bufflen, dmait_host_addr_t *hostAddr) {
    snprintf(buf, bufflen, "%02X%02X:%02X%02X:%02X%02X:%02X%02X",
            (int)hostAddr->u8[0], (int)hostAddr->u8[1],
            (int)hostAddr->u8[2], (int)hostAddr->u8[3],
            (int)hostAddr->u8[4], (int)hostAddr->u8[5],
            (int)hostAddr->u8[6], (int)hostAddr->u8[7]);
    //printf("$$$$$ EXPANDED ipv6 Addr %s\n",str);
}

void dmait_print_host_addr(dmait_host_addr_t *hostAddr) {
    LOG_INFO_("%02X%02X:%02X%02X:%02X%02X:%02X%02X",
             (int)hostAddr->u8[0], (int)hostAddr->u8[1],
             (int)hostAddr->u8[2], (int)hostAddr->u8[3],
             (int)hostAddr->u8[4], (int)hostAddr->u8[5],
             (int)hostAddr->u8[6], (int)hostAddr->u8[7]);
    //printf("$$$$$ EXPANDED ipv6 Addr %s\n",str);
}

void dmait_get_node_key_for_host_addr(char *buf, size_t bufflen, dmait_host_addr_t *hostAddr) {
    dmait_snprint_host_addr(buf, bufflen, hostAddr);
}

char* dmait_snprint_current_host_addr(char *buf, size_t bufflen) {
    uip_ipaddr_t currIp;
    uip_ipaddr_copy(&currIp, rpl_get_global_address());
    dmait_host_addr_t hostAddr;
    extractHostAddrFromIpv6(&hostAddr, &currIp);
    dmait_get_node_key_for_host_addr(buf, bufflen, &hostAddr);
    return buf;
}

char* dmait_get_current_node_key(char *buf, size_t bufflen) {
    return dmait_snprint_current_host_addr(buf, bufflen);
}

void dmait_generate_hmac(char *str_data, char *key, char *outBuff, size_t outBuffLen) {
    //const char* str_data = "Hello World!";
    //const char* str_key = "super-secret-key";
    uint8_t out[SHA256_HASH_SIZE];
    char out_str[SHA256_HASH_SIZE * 2 + 1];
    unsigned i;

    // Call hmac-sha256 function
    hmac_sha256(key, strlen(key), str_data, strlen(str_data), &out, sizeof(out));

    // Convert `out` to string with printf
    memset(&out_str, 0, sizeof(out_str));
    for (i = 0; i < sizeof(out); i++) {
        snprintf(&out_str[i*2], 3, "%02x", out[i]);
    }
    //had to create a new buffer so i can add the end string char \0
    char truncatedHash[outBuffLen];
    strncpy(truncatedHash, out_str, outBuffLen-1);
    truncatedHash[outBuffLen-1] = '\0';

    strcpy(outBuff, truncatedHash);

    // Print out the result
    //printf("Message: %s\n", str_data);
    //printf("Key: %s\n", key);
    //printf("HMAC FULL: %s\n", out_str);
    //printf("HMAC: %s\n", outBuff);

}


void dmait_print_triple_node_block(dmait_triple_node_block_t tnb) {

    LOG_INFO("@@@ DMAIT - TRIPLE NODE BLOCK\n");
    LOG_INFO("@@@ NODE: %i, - ", node_id);
    LOG_INFO_6ADDR(rpl_get_global_address());
    LOG_INFO_("\n");

    uip_ip6addr_t nodeAddrAux;

    LOG_INFO("@@@ PREVIOUS NODE IP: ");
    buildLinkLocalUnicastAddr(&nodeAddrAux, &tnb.prev_node_addr);
    LOG_INFO_6ADDR(&nodeAddrAux);
    LOG_INFO_("\n");

    LOG_INFO("@@@ CURR NODE IP: ");
    buildLinkLocalUnicastAddr(&nodeAddrAux, &tnb.node_addr);
    LOG_INFO_6ADDR(&nodeAddrAux);
    LOG_INFO_("\n");
/*
    //&&&& TESTE DE CONVERSAO DE IPS
    uip_ipaddr_t convIp;
    uip_ipaddr_copy(&convIp, &tnb.node_ip);

    printf("&&&& ENDERECO IP (HEX) =%#010x - %#010x - %#010x - %#010x\n",
           UIP_HTONS(convIp.u16[4]), UIP_HTONS(convIp.u16[5]), UIP_HTONS(convIp.u16[6]), UIP_HTONS(convIp.u16[7]));

    printf("$$$$ HEX: %#04X - %#04X - %#04X - %#04X - %#04X - %#04X - %#04X - %#04X - %#04X - %#04X - %#04X - %#04X - %#04X - %#04X - %#04X - %#04X\n",
           convIp.u8[0],convIp.u8[1],convIp.u8[2],convIp.u8[3],convIp.u8[4],convIp.u8[5],convIp.u8[6],convIp.u8[7],
           convIp.u8[8],convIp.u8[9],convIp.u8[10],convIp.u8[11],convIp.u8[12],convIp.u8[13],convIp.u8[14],convIp.u8[15]);
           //ipv6_expander(&convIp);

    //&&&&
*/
    LOG_INFO("@@@ NEXT NODE IP: ");
    buildLinkLocalUnicastAddr(&nodeAddrAux, &tnb.next_node_addr);
    LOG_INFO_6ADDR(&nodeAddrAux);
    LOG_INFO_("\n");

    LOG_INFO("@@@ HMAC DIGEST: %s\n", tnb.hmac_digest);

}

void extractHostAddrFromIpv6(dmait_host_addr_t *hostAddr, const uip_ipaddr_t *addr) {
    hostAddr->u8[0] = addr->u8[8];
    hostAddr->u8[1] = addr->u8[9];
    hostAddr->u8[2] = addr->u8[10];
    hostAddr->u8[3] = addr->u8[11];
    hostAddr->u8[4] = addr->u8[12];
    hostAddr->u8[5] = addr->u8[13];
    hostAddr->u8[6] = addr->u8[14];
    hostAddr->u8[7] = addr->u8[15];
/*
    //DEBUG
    printf("DEBUGING HOST ADDR: %02X%02X:%02X%02X:%02X%02X:%02X%02X AND IPADDR: ",
             (int)hostAddr->u8[0], (int)hostAddr->u8[1],
             (int)hostAddr->u8[2], (int)hostAddr->u8[3],
             (int)hostAddr->u8[4], (int)hostAddr->u8[5],
             (int)hostAddr->u8[6], (int)hostAddr->u8[7]);
    LOG_INFO_6ADDR(addr);
    printf("\n");
*/
}

void buildLinkLocalUnicastAddr(uip_ip6addr_t *unicastAddr, dmait_host_addr_t *hostAddr) {
    //uip_create_unspecified(unicastAddr); //zera o unicastAddr
    uip_create_linklocal_prefix(unicastAddr); //configura os primeiros octetos como "FE80"
    unicastAddr->u8[8] = hostAddr->u8[0];
    unicastAddr->u8[9] = hostAddr->u8[1];
    unicastAddr->u8[10] = hostAddr->u8[2];
    unicastAddr->u8[11] = hostAddr->u8[3];
    unicastAddr->u8[12] = hostAddr->u8[4];
    unicastAddr->u8[13] = hostAddr->u8[5];
    unicastAddr->u8[14] = hostAddr->u8[6];
    unicastAddr->u8[15] = hostAddr->u8[7];
}

void buildDS6DefaultPrefixAddr(uip_ipaddr_t *unicastAddr, dmait_host_addr_t *hostAddr) {
    unicastAddr->u16[0] = UIP_HTONS(0xfd00); //UIP_DS6_DEFAULT_PREFIX
    unicastAddr->u16[1] = 0;
    unicastAddr->u16[2] = 0;
    unicastAddr->u16[3] = 0;

    unicastAddr->u8[8] = hostAddr->u8[0];
    unicastAddr->u8[9] = hostAddr->u8[1];
    unicastAddr->u8[10] = hostAddr->u8[2];
    unicastAddr->u8[11] = hostAddr->u8[3];
    unicastAddr->u8[12] = hostAddr->u8[4];
    unicastAddr->u8[13] = hostAddr->u8[5];
    unicastAddr->u8[14] = hostAddr->u8[6];
    unicastAddr->u8[15] = hostAddr->u8[7];
}

/*---------------------------------------------------------------------------*/

void dmait_build_host_addr_from_block_field(char *blockField, dmait_host_addr_t *dmaitHostAddr) {
    static char addrPrefix[] = "0000:0000:0000:0000:";
    char ipstr[UIPLIB_IPV6_MAX_STR_LEN+1];
    strcpy(ipstr, addrPrefix);
    strcat(ipstr, blockField);
    ipstr[UIPLIB_IPV6_MAX_STR_LEN] = '\0';
    uip_ip6addr_t addr;
    uiplib_ipaddrconv(ipstr, &addr);
    extractHostAddrFromIpv6(dmaitHostAddr, &addr);
}

bool dmait_is_host_addr_equal_ip(dmait_host_addr_t *hostAddr, uip_ipaddr_t *ipAddr) {
    dmait_host_addr_t hostAddrFromIp;
    extractHostAddrFromIpv6(&hostAddrFromIp, ipAddr);
    return dmait_host_addr_cmp(hostAddr, &hostAddrFromIp);
}



/*

dmait_ipaddr_t * build_dmait_ip(uip_ip6addr_t *addr) {
    dmait_ipaddr_t *dmait_ip;
    dmait_ip = (dmait_ipaddr_t *) malloc(sizeof(dmait_ipaddr_t));
    memcpy(&dmait_ip->ip_addr, addr, sizeof(uip_ip6addr_t));
    return dmait_ip;
}


uip_ip6addr_t * extract_ip(dmait_ipaddr_t *dmait_ip) {
    if (dmait_ip != NULL) {
        return &dmait_ip->ip_addr;
    } else {
        return NULL;
    }
}

*/

/*
int dmait_ip_to_str(char *buf, size_t size, const uip_ip6addr_t *addr) {
    return uiplib_ipaddr_snprint(buf, size, addr);
}
*/
/*---------------------------------------------------------------------------*/
/*
int dmait_str_to_ip(const char *addrstr, uip_ip6addr_t *addr) {
    return uiplib_ip6addrconv(addrstr, addr);
}
*/
/*---------------------------------------------------------------------------*/

void dmait_send_text_data(struct simple_udp_connection *udp_conn,
              const char *data,
              const uip_ipaddr_t *dest_ipaddr) {

    if(NETSTACK_ROUTING.node_is_reachable()) {
        // Send to DAG root
        LOG_INFO("ENVIANDO TEXTO: \"%s\", PARA: ", data);
        LOG_INFO_6ADDR(dest_ipaddr);
        LOG_INFO_("\n");
        //simple_udp_sendto(udp_conn, data, datalen, dest_ipaddr);
        simple_udp_sendto(udp_conn, data, strlen(data), dest_ipaddr);

        //LOG_INFO("MENSAGEM: \"%s\" ENVIADA PARA: ", data);
        //LOG_INFO_6ADDR(dest_ipaddr);
        //LOG_INFO_("\n");
        //LOG_INFO("ENVIADA\n");
    } else {
        LOG_ERR("ERRO-FORA DA REDE\n");
    }
}

void dmait_send_data(struct simple_udp_connection *udp_conn,
                     const void *data, uint16_t datalen,
                     const uip_ipaddr_t *dest_ipaddr) {

//    if(NETSTACK_ROUTING.node_is_reachable()) {
        // Send to DAG root
        LOG_INFO("ENVIANDO: \"%s\",  PARA: ", (char*) data);
        LOG_INFO_6ADDR(dest_ipaddr);
        LOG_INFO_("\n");

        simple_udp_sendto(udp_conn, data, datalen, dest_ipaddr);

        //LOG_INFO("MENSAGEM: \"%s\" ENVIADA PARA: ", (char*) data);
        //LOG_INFO_6ADDR(dest_ipaddr);
        //LOG_INFO_("\n");
        //LOG_INFO("ENVIADA\n");
//    } else {
//        LOG_ERR("ERRO - NAO EH POSSIVEL ENVIAR MENSAGEM - AINDA NAO INTEGRADO A REDE\n");
//    }
}

/*
void dmait_TEST_build_main_route(dmait_route_t *route) {
    if (route != NULL) {

        //Add a route for testing
        //dmait_route_t newroute;
        route->route_id = node_id;
        route->active = true;
        route->suspicion_level = 8;

        LIST_STRUCT_INIT(route, ip_list);

        uip_ipaddr_t root_ipaddr;
        NETSTACK_ROUTING.get_root_ipaddr(&root_ipaddr);
        dmait_ipaddr_t dmait_ip1;
        dmait_ip1.ip_addr = root_ipaddr;
        list_add(route->ip_list, &dmait_ip1);

    } else {
        LOG_INFO("ERROR - dmait_build_main_route: route is NULL");
    }
}


void dmait_build_route_payload(char *buf, size_t bufflen, dmait_route_t *route) {
    snprintf(buf, bufflen, "%u,%u", route->route_id, route->suspicion_level);

    char ipaddrstr[UIPLIB_IPV6_MAX_STR_LEN];
    dmait_ipaddr_t *dmait_ip = list_head(route->ip_list);
    printf("##### ITERATING DMAIT_IPs #####\n");
    while (dmait_ip != NULL) {
        printf("##### DMAIT_IP NOT NULL #####\n");
        //dmait_ip_to_str(ipaddrstr, UIPLIB_IPV6_MAX_STR_LEN, &dmait_ip->ip_addr);
        uiplib_ipaddr_snprint(ipaddrstr, UIPLIB_IPV6_MAX_STR_LEN, &dmait_ip->ip_addr);
        printf("##### EXTRACTING IP FROM DMAIT_IPs: %s #####\n", ipaddrstr);
        strcat(buf, ",");
        strcat(buf, ipaddrstr);
        dmait_ip = list_item_next(dmait_ip);
    }
}



void dmait_parse_route_payload(char *payload, dmait_route_t *route) {
    if (route != NULL) {
        char *token;
        //initialize the ip_list in the route
        LIST_STRUCT_INIT(route, ip_list);
        route->active = true; //default

        int field = 0;

        token = strtok(payload, DMAIT_FIELD_SEPARATOR);
        while (token != NULL) {
            printf("$$$$ PARSING PAYLOAD %i, TOKEN: %s\n", field, token);
            switch (field) {
                case 0: //route_id
                    printf("$$$$ CASE 0\n");
                    route->route_id = (uint8_t)atoi(token);
                    break;
                case 1: //suspicion_level
                    printf("$$$$ CASE 1\n");
                    route->suspicion_level = (uint8_t)atoi(token);
                    break;
                default: //ip addresses
                    printf("$$$$ CASE 2 or more\n");
                    //default case is an IP address
                    uip_ip6addr_t new_addr;

                    //dmait_str_to_ip(token, &new_addr);
                    uiplib_ip6addrconv(token, &new_addr);

                    dmait_ipaddr_t *dmait_ip = build_dmait_ip(&new_addr);

                    list_add(route->ip_list, dmait_ip);
            }
            field++;
            token = strtok(NULL, DMAIT_FIELD_SEPARATOR);
        }
    } else {
        LOG_INFO("ERROR - dmait_parse_route_payload: route not instantiated");
    }
}

*/

void dmait_calculate_hmac_from_triple_node_block(char *buf, size_t bufflen, dmait_triple_node_block_t *dmaitTnb, char *nodeKey) {
    char prevNodeAddr[DMAIT_HOST_ADDR_LENGTH + 1];
    dmait_snprint_host_addr(prevNodeAddr, sizeof(prevNodeAddr), &dmaitTnb->prev_node_addr);
    char nodeAddr[DMAIT_HOST_ADDR_LENGTH + 1];
    dmait_snprint_host_addr(nodeAddr, sizeof(nodeAddr), &dmaitTnb->node_addr);
    char nextNodeAddr[DMAIT_HOST_ADDR_LENGTH + 1];
    dmait_snprint_host_addr(nextNodeAddr, sizeof(nextNodeAddr), &dmaitTnb->next_node_addr);
    char blockStr[DMAIT_RV_BLOCK_LENGTH + 1];
    snprintf(blockStr, 62, "%s,%s,%s", prevNodeAddr, nodeAddr, nextNodeAddr);
    dmait_generate_hmac(blockStr, nodeKey, buf, bufflen);
}

bool dmait_verify_hmac_in_triple_node_block(dmait_triple_node_block_t *dmaitTnb, char *nodeKey) {
    bool result = false;
    char hmac[11];
    dmait_calculate_hmac_from_triple_node_block(hmac, 11, dmaitTnb, nodeKey);
    //printf("CHECKING HMAC - SENT: %s, CALCULATED: %s\n", dmaitTnb->hmac_digest, hmac);
    if (strcmp(hmac, dmaitTnb->hmac_digest) == 0) {
        result = true;
    }
    return result;
}

void dmait_route_validation_add_block(char *buf, size_t bufflen, dmait_triple_node_block_t *dmaitTnb) {
    size_t totalLen = bufflen;

    char prevNodeAddr[DMAIT_HOST_ADDR_LENGTH + 1];
    dmait_snprint_host_addr(prevNodeAddr, sizeof(prevNodeAddr), &dmaitTnb->prev_node_addr);

    char nodeAddr[DMAIT_HOST_ADDR_LENGTH + 1];
    dmait_snprint_host_addr(nodeAddr, sizeof(nodeAddr), &dmaitTnb->node_addr);

    char nextNodeAddr[DMAIT_HOST_ADDR_LENGTH + 1];
    dmait_snprint_host_addr(nextNodeAddr, sizeof(nextNodeAddr), &dmaitTnb->next_node_addr);

    char block[DMAIT_RV_BLOCK_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR) + 1];
    int  blockLen = snprintf(block, totalLen, "%s,%s,%s,%s;", prevNodeAddr, nodeAddr, nextNodeAddr, dmaitTnb->hmac_digest);

    if (blockLen < totalLen) {
        strncat(buf, block, totalLen);
        //totalLen -= blockLen;
    } else {
        LOG_ERR("ERRO-PAYLOAD PEQUENO\n");
        return;
    }

}

/*---------------------------------------------------------------------------*/

void dmait_route_validation_build_payload(char *buf, size_t bufflen, dmait_triple_node_block_t dmaitTnbArray[], int arraySize) {

    //precalculate the block payload length
    int headerLength = strlen(DMAIT_ROUTE_VALIDATION_HEADER) + strlen(DMAIT_HEADER_SEPARATOR);
    int blockLength = 3 * (DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_FIELD_SEPARATOR)) + DMAIT_HMAC_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR);
    int allBlocksLength = blockLength * arraySize;

    char payload[headerLength + allBlocksLength + 1];

    payload[0] = '\0'; //to start the concatenation from the beginning
    payload[sizeof(payload)] = '\0'; //At the end.. as failsafe

    //concat the header and separator
    strcat(payload, DMAIT_ROUTE_VALIDATION_HEADER);
    strcat(payload, DMAIT_HEADER_SEPARATOR);

    int x;
    for (x = 0; x < arraySize; x++) {
        char prevNodeAddr[DMAIT_HOST_ADDR_LENGTH + 1];
        dmait_snprint_host_addr(prevNodeAddr, sizeof(prevNodeAddr), &dmaitTnbArray[x].prev_node_addr);
        strcat(payload, prevNodeAddr);
        strcat(payload, DMAIT_FIELD_SEPARATOR);

        char nodeAddr[DMAIT_HOST_ADDR_LENGTH + 1];
        dmait_snprint_host_addr(nodeAddr, sizeof(nodeAddr), &dmaitTnbArray[x].node_addr);
        strcat(payload, nodeAddr);
        strcat(payload, DMAIT_FIELD_SEPARATOR);

        char nextNodeAddr[DMAIT_HOST_ADDR_LENGTH + 1];
        dmait_snprint_host_addr(nextNodeAddr, sizeof(nextNodeAddr), &dmaitTnbArray[x].next_node_addr);
        strcat(payload, nextNodeAddr);
        strcat(payload, DMAIT_FIELD_SEPARATOR);

        strcat(payload, dmaitTnbArray[x].hmac_digest);
        strcat(payload, DMAIT_BLOCK_SEPARATOR);
    }
    strncpy(buf, payload, bufflen);
    buf[bufflen - 1] = '\0'; //force null termination
}

/*---------------------------------------------------------------------------*/

bool dmait_parse_next_block_from_rv_payload(char *payload, dmait_triple_node_block_t *tnb, char **blockState) {
    bool result = false;
    char *stringBlock;

    if (payload != NULL) {
        char *stringHead, *stringBody;
        char *headState;

        stringHead = strtok_r(payload, DMAIT_HEADER_SEPARATOR, &headState);
        if (stringHead != NULL && strcmp(DMAIT_ROUTE_VALIDATION_HEADER, stringHead) == 0) {
            //printf("CABECALHO CORRETO - DMAITRV\n");

            //cabecalho identificado.
            //printf("HEAD: %s\n", stringHead);

            //Descartando cabecalho
            stringBody = strtok_r(NULL, DMAIT_HEADER_SEPARATOR, &headState);
            if (stringBody != NULL) {

                stringBlock = strtok_r(stringBody, DMAIT_BLOCK_SEPARATOR, blockState);
                result = parseBlock(stringBlock, tnb);
            } else {
                LOG_ERR("ERRO-NAO CONTEM BLOCOS\n");
            }
        } else {
            LOG_ERR("ERRO-CABECALHO INCORETO\n");
        }
    } else {
        stringBlock = strtok_r(NULL, DMAIT_BLOCK_SEPARATOR, blockState);
        result = parseBlock(stringBlock, tnb);
    }
    return result;
}

bool parseBlock(char *blockString, dmait_triple_node_block_t *tnb) {
    bool result = false;
    if (blockString != NULL) {
        //printf("~~~~~~~BLOCKSTRING: %s\n", blockString);
        //printf("~~~~~~~STRLEN: %d\n", (int) strlen(blockString));

        char stringField[20];

        getSubstring(stringField, blockString, 0, 19);
        stringField[19] = '\0';
        dmait_build_host_addr_from_block_field(stringField, &tnb->prev_node_addr);
        //printf("PREV NODE: %s\n", stringField);

        getSubstring(stringField, blockString, 20, 19);
        stringField[19] = '\0';
        dmait_build_host_addr_from_block_field(stringField, &tnb->node_addr);
        //printf("CURR NODE: %s\n", stringField);

        getSubstring(stringField, blockString, 40, 19);
        stringField[19] = '\0';
        dmait_build_host_addr_from_block_field(stringField, &tnb->next_node_addr);
        //printf("NEXT NODE: %s\n", stringField);

        getSubstring(stringField, blockString, 60, 10);
        stringField[10] = '\0';
        strcpy(tnb->hmac_digest, stringField);
        //printf("HMAC DIG: %s\n", stringField);

        result = true;
    }
    return result;
}


bool dmait_select_neighbor_not_in_array(dmait_host_addr_t *selectedNeighbor, dmait_host_addr_t hostAddrArray[], int arraySize) {
    static bool foundNbr = false;

    if(curr_instance.used) {
        foundNbr = false;

        //printf("SELECTING NEIGHBOR NOT IN ARRAY\n");
        rpl_nbr_t *nbr = nbr_table_head(rpl_neighbors);

        while(nbr != NULL && foundNbr == false) {
            //uip_ipaddr_t nbrIp;
            //uip_ipaddr_copy(&nbrIp, rpl_neighbor_get_ipaddr(nbr));
            //uip_create_linklocal_prefix(&nbrIp);
            dmait_host_addr_t nbrHostAddr;
            extractHostAddrFromIpv6(&nbrHostAddr, rpl_neighbor_get_ipaddr(nbr));

            //char msgBuff[200];
            //rpl_neighbor_snprint(msgBuff, sizeof(msgBuff), nbr);
            //LOG_INFO_("NBR DATA: %s\n", msgBuff);
            //LOG_INFO_6ADDR(rpl_neighbor_get_ipaddr(nbr));
            //LOG_INFO_("\n");

            //printf("NBR IS REACHEABLE: %s\n", rpl_neighbor_is_reachable(nbr) ? "true" : "false");
            //printf("NBR IS ACCEPTABLE PARENT: %s\n", rpl_neighbor_is_acceptable_parent(nbr) ? "true" : "false");

            bool suitableNeighbor = true;
            int x;
            for (x = 0; x < arraySize && suitableNeighbor; x++) {
                char nbrInListStr[30];
                dmait_snprint_host_addr(nbrInListStr, sizeof(nbrInListStr), &hostAddrArray[x]);
                char currNbrStr[30];
                dmait_snprint_host_addr(currNbrStr, sizeof(currNbrStr), &nbrHostAddr);
                //LOG_INFO("COMPARING NEIGHBOR: %s, WITH ADDR IN LIST: %s ...", currNbrStr, nbrInListStr);

                if (dmait_host_addr_cmp(&nbrHostAddr, &hostAddrArray[x])) {
                    //LOG_INFO_("EQUAL!\n");
                    suitableNeighbor = false;
                } else {
                    //LOG_INFO_("DIFFERENT!\n");
                }
            }
            if (suitableNeighbor) {
                LOG_INFO("VIZINHO ACEITAVEL: ");
                dmait_print_host_addr(&nbrHostAddr);
                LOG_INFO_("\n");

                foundNbr = true;
                dmait_host_addr_copy(selectedNeighbor, &nbrHostAddr);
            }
            nbr = nbr_table_next(rpl_neighbors, nbr);
        }
        if (!foundNbr) {
            //not found suitable neigbor
            LOG_ERR("ERRO-ABORTANDO DMAIT-AR\n");
        }
    } else {
        // curr_instance.used == false
        LOG_ERR("ERRO-\"curr_instante.used == false\"\n");
    }
    return foundNbr;
}


/*---------------------------------------------------------------------------*/




