/*
 * DMAIT - Deteccao e Mitigacao de Ataques de Interrupcao de Trafego Extension
 *
 * This code is based on the rpl-udp example from Contiki operating system.
 *
 * Created by Daniel Francis Soriano
 *
 */

#include "contiki.h"


#include "dmait-client-logic.h"
#include "dmait-logic.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Log configuration */
#include "sys/log.h"

#define LOG_MODULE "DMAIT"
#define LOG_LEVEL LOG_LEVEL_INFO

#define DATA_MESSAGE_TIMEOUT    (250 * CLOCK_SECOND/1000)

//How many data messages must be lost before starting DMAIT-AR
#define DATA_MESSAGE_LOSS_THRESHOLD 2

//MEMB dynamic memory allocation definition for dmait_route_node_t structure
MEMB(dmait_route_node_memb, dmait_route_node_t, 30); //40 route nodes max

/* Only instance of route validation list per node */
LIST(dmaitValidatedRoute); //for dmait_route_node_t

/* Only instance of node blacklist list per node */
//LIST(dmaitNodeBlacklist); //for dmait_route_node_t

/* Only instance of not acked message list */
//LIST(dmaitUnackedMessageList); //for dmait_route_node_t

//last data message metadata (for timeout reasons)
uint8_t lastDataMessageSequenceNumber = 0;
char lastDataMessageHmac[11];
//bool lastDataMessageAcked = false;
linkaddr_t lastDataMessageParentAddr;

//how many unacked data messages occur in a row
int unackedDataMessagesInARowCounter = 0;

static struct ctimer data_message_timeout_timer;


void init() {
    memb_init(&dmait_route_node_memb);
    list_init(dmaitValidatedRoute);
//    list_init(dmaitNodeBlacklist);
}



/*---------------------------------------------------------------------------*/

int dmait_valid_route_depth() {
    return list_length(dmaitValidatedRoute);
}

/*---------------------------------------------------------------------------*/

void dmait_print_valid_route() {
    LOG_INFO("******** CURRENT VALID ROUTE ********\n");
    dmait_route_node_t *currNode;
    currNode = list_head(dmaitValidatedRoute);
    while (currNode != NULL) {
        dmait_print_host_addr(&currNode->node_addr);
        LOG_INFO_("\n");
        currNode = list_item_next(currNode);
    }
    LOG_INFO("***************************************\n");
}


void dmait_valid_route_add_node_by_adddr(dmait_host_addr_t *nodeAddr) {
    //LOG_INFO("ADDING NODE TO CURRENT VALID ROUTE\n");
    dmait_route_node_t *newNode;
    newNode = memb_alloc(&dmait_route_node_memb);

    if (newNode != NULL) {
        //LOG_INFO("MEMORY ALLOCATED IN dmait_route_node_memb \n");
        dmait_host_addr_copy(&newNode->node_addr, nodeAddr);
        list_add(dmaitValidatedRoute, newNode);
    } else {
        LOG_ERR("CANNOT ALLOCATE MEMORY IN dmait_route_node_memb \n");
    }
    //dmait_print_valid_route();
}

/*---------------------------------------------------------------------------*/


bool dmait_route_remove_node_by_host_addr(dmait_host_addr_t *nodeAddr) {
    dmait_route_node_t *currNode;
    currNode = list_head(dmaitValidatedRoute);
    while (currNode != NULL) {
        dmait_route_node_t *nextNode;
        nextNode = list_item_next(currNode);
        if (dmait_host_addr_cmp(nodeAddr, &currNode->node_addr)) {
            list_remove(dmaitValidatedRoute, currNode);
            memb_free(&dmait_route_node_memb, currNode);
            return true;
        }
        currNode = nextNode;
    }
    return false;
}

/*---------------------------------------------------------------------------*/

bool is_node_addr_in_dmait_valid_route(dmait_host_addr_t *nodeAddr) {
    dmait_route_node_t *currNode;
    currNode = list_head(dmaitValidatedRoute);
    while (currNode != NULL) {
        if (dmait_host_addr_cmp(&currNode->node_addr, nodeAddr)) {
            return true;
        }
        currNode = list_item_next(currNode);
    }
    return false;
}

void dmait_clear_valid_route() {
    dmait_route_node_t *currNode;
    currNode = list_head(dmaitValidatedRoute);
    while (currNode != NULL) {
        dmait_route_node_t *nextNode;
        nextNode = list_item_next(currNode);
        list_remove(dmaitValidatedRoute, currNode);
        memb_free(&dmait_route_node_memb, currNode);
        currNode = nextNode;
    }
}


/*---------------------------------------------------------------------------*/

/*

int dmait_node_blacklist_length() {
    return list_length(dmaitNodeBlacklist);
}


void dmait_node_blacklist_add_node_by_adddr(dmait_host_addr_t *nodeAddr) {
    dmait_route_node_t *newNode;
    newNode = memb_alloc(&dmait_route_node_memb);

    if (newNode != NULL) {
        //LOG_INFO("MEMORY ALLOCATED IN dmait_route_node_memb \n");
        dmait_host_addr_copy(&newNode->node_addr, nodeAddr);
        list_add(dmaitNodeBlacklist, newNode);
    } else {
        LOG_ERR("CANNOT ALLOCATE MEMORY IN dmait_route_node_memb \n");
    }
}


bool dmait_blacklist_remove_node_by_host_addr(dmait_host_addr_t *nodeAddr) {
    dmait_route_node_t *currNode;
    currNode = list_head(dmaitNodeBlacklist);
    while (currNode != NULL) {
        dmait_route_node_t *nextNode;
        nextNode = list_item_next(currNode);
        if (dmait_host_addr_cmp(nodeAddr, &currNode->node_addr)) {
            list_remove(dmaitNodeBlacklist, currNode);
            memb_free(&dmait_route_node_memb, currNode);
            return true;
        }
        currNode = nextNode;
    }
    return false;
}


bool is_node_addr_in_dmait_node_blacklist(dmait_host_addr_t *nodeAddr) {
    dmait_route_node_t *currNode;
    currNode = list_head(dmaitNodeBlacklist);
    while (currNode != NULL) {
        if (dmait_host_addr_cmp(&currNode->node_addr, nodeAddr)) {
            return true;
        }
        currNode = list_item_next(currNode);
    }
    return false;
}

*/

/*---------------------------------------------------------------------------*/


void dmait_rv_start_route_validation(struct simple_udp_connection *c) {
    //TESTE DE INICIO DA VALIDACAO DE ROTA

    int arraySize = 1; //at this time only one
    dmait_triple_node_block_t dmaitTnbArray[arraySize];
    //PREV NODE IS NONE -> 0,0,0,0
    dmait_hostAddr(&dmaitTnbArray[0].prev_node_addr, 0,0,0,0); //prev node addr (all zeroes)

    //THIS NODE ADDRESS
    uip_ipaddr_t currIp;
    uip_ipaddr_copy(&currIp, rpl_get_global_address());
    extractHostAddrFromIpv6(&dmaitTnbArray[0].node_addr, &currIp); //curr node addr

    //PARENT ADDRESS
    uip_ipaddr_t parentIp;
    uip_ipaddr_copy(&parentIp, rpl_neighbor_get_ipaddr(curr_instance.dag.preferred_parent));
    extractHostAddrFromIpv6(&dmaitTnbArray[0].next_node_addr, &parentIp);

    //LINK LOCAL PARENT ADDRESS
    uip_create_linklocal_prefix(&parentIp);

    //CALCULATE HMAC
    char nodeKey[25];
    dmait_get_current_node_key(nodeKey, 25);
    dmait_calculate_hmac_from_triple_node_block(dmaitTnbArray[0].hmac_digest,
                                                sizeof(dmaitTnbArray[0].hmac_digest),
                                                &dmaitTnbArray[0],
                                                nodeKey);

    //dmait_print_triple_node_block(dmaitTnbArray[0]);

    //precalculate the block payload length
    int payloadLength = strlen(DMAIT_ROUTE_VALIDATION_HEADER) + strlen(DMAIT_HEADER_SEPARATOR)
            + arraySize * (3 * (DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_FIELD_SEPARATOR)) + DMAIT_HMAC_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR));

    char msgBuff[payloadLength + 1];
    dmait_route_validation_build_payload(msgBuff, sizeof(msgBuff), dmaitTnbArray, arraySize);

    //LOG_INFO("************* RV MSG PAYLOAD: %s\n", msgBuff);
/*
    char dummy[sizeof(msgBuff)];
    for (int x = 0; x < sizeof(msgBuff); x++) {
        dummy[x] = 'A';
    }
    dummy[sizeof(msgBuff)-1] = '\0';
    LOG_INFO("************* RV MSG DUMMY PAYLOAD: %s\n", dummy);
*/
    //SEND RV MSG TO PARENT
    //dmait_send_data(c, msgBuff, strlen(msgBuff), &parentIp);
    dmait_send_text_data(c, msgBuff, &parentIp);
//    dmait_send_text_data(c, dummy, &parentIp);

}

/*--------------------------------------------------------------------------------------*/

void dmait_ar_start_avoid_route(struct simple_udp_connection *c) {
    //INICIO DA ROTA ALTERNATIVA
    LOG_INFO("DMAIT-AR - INICIANDO O PROCESSO DE ROTA ALTERNATIVA\n");
    int avoidRouteLenght = dmait_valid_route_depth();
    //LOG_INFO("************* AVOID ROUTE LENGTH - dmait_valid_route_depth(): %d\n", avoidRouteLenght);

    if (avoidRouteLenght > 0) {
        //Sums the header length, plus the "AVOID" header and 2 chars for "avoid route" length (up to 99),
        //plus one address length per list entry (+1 separator for each entry),
        // + hmac length (10)
        // + the "ROUTE" header and this node host addr
        int payloadBuffLen = strlen(DMAIT_ALTERNATE_ROUTE_HEADER) + strlen(DMAIT_HEADER_SEPARATOR) //header
                + strlen(DMAIT_ALTERNATE_ROUTE_AVOID_HEADER) + strlen(DMAIT_HEADER_SEPARATOR) //avoid route header
                + 2 + strlen(DMAIT_HEADER_SEPARATOR)  //route length
                + (avoidRouteLenght * (DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_FIELD_SEPARATOR))) //hosts
                + strlen(DMAIT_BLOCK_SEPARATOR) + DMAIT_HMAC_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR)
                + strlen(DMAIT_ALTERNATE_ROUTE_ROUTE_HEADER) + DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR);

        char payload[payloadBuffLen + 1];
        payload[0] = '\0'; //to start the concatenation from the beginning
        payload[payloadBuffLen] = '\0'; //At the end.. as failsafe

        //concat the header and separator
        strcat(payload, DMAIT_ALTERNATE_ROUTE_HEADER);
        strcat(payload, DMAIT_HEADER_SEPARATOR);

        //concat the "AVOID" header and separator
        strcat(payload, DMAIT_ALTERNATE_ROUTE_AVOID_HEADER);
        strcat(payload, DMAIT_HEADER_SEPARATOR);

        //concat the "AVOID ROUTE NOTE AMOUNT" (up to 99)
        char addrAmount[3];
        snprintf(addrAmount, sizeof(addrAmount), "%02d", avoidRouteLenght);
        strcat(payload, addrAmount);
        strcat(payload, DMAIT_HEADER_SEPARATOR);

        //LOG_INFO("BUFFLEN: %d, REAL LEN: %d, PAYLOAD: %s\n", payloadBuffLen, strlen(payload), payload);


        dmait_host_addr_t avoidRouteArray[avoidRouteLenght];
        int avoidRouteArrayIndex = 0;

        //Iterate routeValidationList and add each addr to the payload
        dmait_route_node_t *currNode;
        currNode = list_head(dmaitValidatedRoute);
        while(currNode != NULL) {
            dmait_route_node_t *nextNode;
            nextNode = list_item_next(currNode);

            //build the array (to be used later)
            dmait_host_addr_copy(&avoidRouteArray[avoidRouteArrayIndex], &currNode->node_addr);
            avoidRouteArrayIndex++;

            //Concatenates the address to the payload
            char addrAux[DMAIT_HOST_ADDR_LENGTH + 2]; //plus 2 for separator and string terminator
            dmait_snprint_host_addr(addrAux, sizeof(addrAux), &currNode->node_addr);
            addrAux[DMAIT_HOST_ADDR_LENGTH] = '\0'; //so the addr is terminated before concat the separator
            if (nextNode != NULL) {
                strcat(addrAux, DMAIT_FIELD_SEPARATOR);
            } else {
                strcat(addrAux, DMAIT_BLOCK_SEPARATOR);
            }
            addrAux[DMAIT_HOST_ADDR_LENGTH + 1] = '\0';
            strcat(payload, addrAux);

            //list_remove(dmaitValidatedRoute, currNode);
            //memb_free(&dmait_route_node_memb, currNode);
            currNode = nextNode;
            //LOG_INFO("&&&&&&&&&&&&&& AVOID ROUTE LENGTH - dmait_valid_route_depth: %d\n", dmait_valid_route_depth());
        }

        //CALCULATE REAL HMAC
        char nodeKey[25];
        dmait_get_current_node_key(nodeKey, 25);

        //generate hmac for the message using current node key
        char hmacStr[11];
        dmait_generate_hmac(payload, nodeKey, hmacStr, sizeof(hmacStr));
        strcat(payload, hmacStr);
        strcat(payload, DMAIT_BLOCK_SEPARATOR);

        strcat(payload, DMAIT_ALTERNATE_ROUTE_ROUTE_HEADER);
        strcat(payload, DMAIT_HEADER_SEPARATOR);

        //LOG_INFO("ROTA A SER EVITADA: %s\n", payload);

        //Get current node addr
        char nodeHostAddr[25];
        dmait_snprint_current_host_addr(nodeHostAddr, sizeof(nodeHostAddr));
        strcat(payload, nodeHostAddr);
        strcat(payload, DMAIT_FIELD_SEPARATOR);

        //LOG_INFO("************* DMAIT-AR calculated length: %d, and real length: %d\n", payloadBuffLen, strlen(payload));
        //LOG_INFO("************* DMAIT-AR PAYLOAD: %s\n", payload);

        //Select a neighbor not in the validated route
        uip_ipaddr_t targetNeighborIpAddr;

        dmait_host_addr_t selectedNeighborHostAddr;
        if (dmait_select_neighbor_not_in_array(&selectedNeighborHostAddr, avoidRouteArray, avoidRouteLenght)) {
            LOG_INFO("DMAIT-AR - ENCONTRADO VIZINHO PARA ROTA ALTERNATIVA: ");
            dmait_print_host_addr(&selectedNeighborHostAddr);
            LOG_INFO_("\n");

            buildLinkLocalUnicastAddr(&targetNeighborIpAddr, &selectedNeighborHostAddr);

        } else {
            //if found no suitable neighbor, send it to the parent anyway
            LOG_INFO("DMAIT-AR - NAO FOI ENCONTRADO VIZINHO PARA ROTA ALTERNATIVA - ENVIANDO AO PAI ATUAL\n");

            //PARENT ADDRESS
            uip_ipaddr_copy(&targetNeighborIpAddr, rpl_neighbor_get_ipaddr(curr_instance.dag.preferred_parent));
            uip_create_linklocal_prefix(&targetNeighborIpAddr);
        }

        //Transmit the data
        LOG_INFO("DMAIT-AR - ENVIANDO DMAIT-AR PARA: ");
        LOG_INFO_6ADDR(&targetNeighborIpAddr);
        LOG_INFO_("\n");
        //dmait_send_data(c, payload, strlen(payload), &targetNeighborIpAddr);
        dmait_send_text_data(c, payload, &targetNeighborIpAddr);

    }
}

void dmait_send_data_message(struct simple_udp_connection *udp_conn,
                             const void *data,
                             const size_t datalen,
                             const uip_ipaddr_t *dest_ipaddr) {

    int truncatedDataLenght = datalen > DMAIT_MAX_DATA_LENGHT ? DMAIT_MAX_DATA_LENGHT : datalen;
    //printf("dmait_send_data_message - truncatedDataLenght: %d\n", truncatedDataLenght);

    //TESTE DE ENVIO DE MENSAGEM
    int dataMessageSize = strlen(DMAIT_DATA_MESSAGE_HEADER) + strlen(DMAIT_HEADER_SEPARATOR)
            + DMAIT_SEQUENCE_NUMBER_LENGTH + strlen(DMAIT_HEADER_SEPARATOR) //4 chars for sequence number
            + DMAIT_DATALEN_LENGHT + strlen(DMAIT_HEADER_SEPARATOR) //3 chars for data length, ie: 009 or 120, and another separator
            + truncatedDataLenght + strlen(DMAIT_BLOCK_SEPARATOR) //the data length (not higher than 999)
            //+ DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR)
            + DMAIT_HMAC_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR);  //hmac + separator

    //printf("dmait_send_data_message - dataMessageSize: %d\n", dataMessageSize);

    char msg[dataMessageSize + 1];
    msg[0] = '\0';
    msg[dataMessageSize] = '\0';

    //concat the header and separator
    strcat(msg, DMAIT_DATA_MESSAGE_HEADER);
    strcat(msg, DMAIT_HEADER_SEPARATOR);

    //concat the "SEQUENCE NUMBER" (up to 9999)
    int seqNumber = lastDataMessageSequenceNumber + 1;
    if (seqNumber > DMAIT_MAX_SEQUENCE_NUMBER) {
        seqNumber = 0;
    }
    char seqNumberDigits[DMAIT_SEQUENCE_NUMBER_LENGTH + 1];
    snprintf(seqNumberDigits, sizeof(seqNumberDigits), "%04d", seqNumber);
    strcat(msg, seqNumberDigits);
    strcat(msg, DMAIT_HEADER_SEPARATOR);

    //concat the "DATA LENGHT" (up to 999)
    char dataLenDigits[DMAIT_DATALEN_LENGHT + 1];
    snprintf(dataLenDigits, sizeof(dataLenDigits), "%03d", truncatedDataLenght);
    strcat(msg, dataLenDigits);
    strcat(msg, DMAIT_HEADER_SEPARATOR);

    //concat the DATA
    strncat(msg, data, truncatedDataLenght);
    strcat(msg, DMAIT_BLOCK_SEPARATOR);
/*
    //concat the sender host addr
    char hostAddrStr[DMAIT_HOST_ADDR_LENGTH + 1];
    dmait_snprint_current_host_addr(hostAddrStr, sizeof hostAddrStr);
    strncat(msg, hostAddrStr, DMAIT_HOST_ADDR_LENGTH);
    strcat(msg, DMAIT_BLOCK_SEPARATOR);
*/
    //CALCULATE REAL HMAC
    char nodeKey[25];
    dmait_get_current_node_key(nodeKey, sizeof(nodeKey));

    //generate hmac for the message using current node key
    char hmacStr[DMAIT_HMAC_LENGTH + 1];
    dmait_generate_hmac(msg, nodeKey, hmacStr, sizeof(hmacStr));

    strcat(msg, hmacStr);
    strcat(msg, DMAIT_BLOCK_SEPARATOR);


    LOG_INFO("ENVIANDO MENSAGEM DE DADOS: \"%s\", SEQ: %d, PARA: ", msg, seqNumber);
    LOG_INFO_6ADDR(dest_ipaddr);
    LOG_INFO_("\n");
    //printf("TAMANHO CALCULADO: %d, TAMANHO REAL: %d\n", dataMessageSize, strlen(msg));
    dmait_register_message_sent(seqNumber, hmacStr);
    //dmait_send_data(udp_conn, msg, strlen(msg), dest_ipaddr);
    dmait_send_text_data(udp_conn, msg, dest_ipaddr);

    //Start a timeout timer
    ctimer_set(&data_message_timeout_timer, DATA_MESSAGE_TIMEOUT, ctimer_data_message_timeout_callback, udp_conn);

}

void resetUnackedDataMessagesCounter() {
    unackedDataMessagesInARowCounter = 0;
}

void ctimer_data_message_timeout_callback(void *ptr) {

    if (unackedDataMessagesInARowCounter > 0) {
        LOG_INFO("TIMEOUT DE MENSAGEM DE DADOS!\n");
        unackedDataMessagesInARowCounter++;

        if (unackedDataMessagesInARowCounter > DATA_MESSAGE_LOSS_THRESHOLD) {
            //resets the unacked data message counter
            unackedDataMessagesInARowCounter = 0; //resets

            LOG_INFO("LIMITE DE MENSAGENS NAO CONFIRMADAS (%d) ULTRAPASSADO", DATA_MESSAGE_LOSS_THRESHOLD);
            LOG_INFO("ADICIONANDO LLADDR DO PAI A LISTA NEGRA: ");
            LOG_INFO_LLADDR(&lastDataMessageParentAddr);
            LOG_INFO_("\n");
            curr_instance.of->addNodeToBlackList(&lastDataMessageParentAddr);

            //starts the ALTERNATIVE ROUTE process
            LOG_INFO("INICIANDO DMAIT-AR\n");
            dmait_ar_start_avoid_route(ptr);
        }
    } else {
        //resets the unacked data message counter
        resetUnackedDataMessagesCounter();
    }

    /* rearm the ctimer */
    //ctimer_reset(&timer_ctimer);

}


void dmait_register_message_sent(uint8_t seqNumber, char * hmac) {
    lastDataMessageSequenceNumber = seqNumber;
    strcpy(lastDataMessageHmac, hmac);
    lladdr_copy(&lastDataMessageParentAddr, rpl_neighbor_get_lladdr(curr_instance.dag.preferred_parent));
    //lastDataMessageAcked = false;
    unackedDataMessagesInARowCounter++;
}

bool dmait_check_last_message_data(uint8_t seqNumber, char * hmac) {
    //printf("lastDataMessageSequenceNumber: %d, seqNumber: %d, lastDataMessageHmac: %s, hmac: %s\n", lastDataMessageSequenceNumber, seqNumber, lastDataMessageHmac, hmac);
    if (seqNumber == lastDataMessageSequenceNumber &&
            strcmp(hmac, lastDataMessageHmac) == 0) {
        return true;
    }
    return false;
}



void dmait_process_data_ack_message(const uip_ipaddr_t *sender_addr, char *data, uint16_t datalen) {

//    //ACK message
//    int ackMessageSize = strlen(DMAIT_DATA_MESSAGE_ACK_HEADER) + strlen(DMAIT_HEADER_SEPARATOR)
//            + DMAIT_SEQUENCE_NUMBER_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR)
//            + DMAIT_HMAC_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR) //original HMAC
//            + DMAIT_HMAC_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR); //new HMAC for this message

    LOG_INFO("PROCESSANDO ACK DE MENSAGEM DE DADOS\n");

    //pre calculate a few indexes (more indexes later on)
    int seqNumberIndex = strlen(DMAIT_DATA_MESSAGE_ACK_HEADER) + strlen(DMAIT_HEADER_SEPARATOR);
    int originalHmacIndex = seqNumberIndex + DMAIT_SEQUENCE_NUMBER_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR);
    int ackHmacIndex = originalHmacIndex + DMAIT_HMAC_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR);

    //Extracts sequence number
    char seqNumberStr[DMAIT_SEQUENCE_NUMBER_LENGTH + 1];
    getSubstring(seqNumberStr, data, seqNumberIndex, DMAIT_SEQUENCE_NUMBER_LENGTH);
    seqNumberStr[DMAIT_SEQUENCE_NUMBER_LENGTH] = '\0';

    char *endPtr;
    int seqNum = strtol(seqNumberStr, &endPtr, 10);
    if (seqNum < 0 || seqNum > DMAIT_MAX_SEQUENCE_NUMBER) {
        LOG_ERR("INVALID SEQUENCE NUMBER: %d. (SEQUENCE NUMBER MUST BE IN BETWEEN 0 AND %d)\n", seqNum, DMAIT_MAX_SEQUENCE_NUMBER);
    }
    
    //Extracts the original (message data) HMAC
    char originalHmacStr[DMAIT_HMAC_LENGTH + 1];
    getSubstring(originalHmacStr, data, originalHmacIndex, DMAIT_HMAC_LENGTH);
    originalHmacStr[DMAIT_HMAC_LENGTH] = '\0';

    //Extracts the ACK HMAC
    char ackHmacStr[DMAIT_HMAC_LENGTH + 1];
    getSubstring(ackHmacStr, data, ackHmacIndex, DMAIT_HMAC_LENGTH);
    ackHmacStr[DMAIT_HMAC_LENGTH] = '\0';

    //printf("XXX seqNumberStr: %s, originalHmacStr: %s, ackHmac: %s\n", seqNumberStr,  originalHmacStr, ackHmacStr);

    //Get this node key
    char nodeKey[DMAIT_HOST_ADDR_LENGTH + 1];
    dmait_get_current_node_key(nodeKey, sizeof(nodeKey));

    char paylodForHmac[ackHmacIndex + 1];
    getSubstring(paylodForHmac, data, 0, ackHmacIndex);
    paylodForHmac[ackHmacIndex] = '\0';

    //printf("XXX payloadForHmac: %s\n", paylodForHmac);

    //generate the ack hmac
    char calcHmacStr[DMAIT_HMAC_LENGTH + 1];
    dmait_generate_hmac(paylodForHmac, nodeKey, calcHmacStr, sizeof(calcHmacStr));

    if (strcmp(ackHmacStr, calcHmacStr) == 0) {
        //os hmacs sao iguais..
        //printf("XXX HMAC RECEBIDO: %s, HMAC CALCULADO: %s, SAO IGUAIS!\n", ackHmacStr, calcHmacStr);

//        if (strcmp(lastDataMessageHmac, ackHmacStr) == 0) {
        if (dmait_check_last_message_data(seqNum, originalHmacStr)) {
            LOG_INFO("MARCANDO ULTIMA MENSAGEM COMO ACKED!\n");
            //This ack if for the last message
            //lastDataMessageAcked = true; //mark it as acked
            resetUnackedDataMessagesCounter();

        } else {
            LOG_ERR("DMACK HMAC EH VALIDO, MAS NAO PARA A ULTIMA MSG (%s) - IGNORANDO DMACK!\n", lastDataMessageHmac);
        }
    } else {
        LOG_ERR("DMACK HMAC INVALIDO, MENSAGEM FORJADA OU CORROMPIDA (%s) - IGNORANDO DMACK!\n", lastDataMessageHmac);
    }

}

void dmait_process_fl_probe_message(const uip_ipaddr_t *sender_addr, char *data, uint16_t datalen, struct simple_udp_connection *c) {

//    //FL-PROBE MESSAGE
//    int probeMessageSize = strlen(DMAIT_FAULT_LOCATION_PROBE_HEADER) + strlen(DMAIT_HEADER_SEPARATOR)
//                         + DMAIT_SEQUENCE_NUMBER_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR)
//                         + DMAIT_HMAC_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR); //message HMAC

    LOG_INFO("PROCESSANDO MENSAGEM FL-PROBE\n");

    //pre calculate a few indexes (more indexes later on)
    int seqNumberIndex = strlen(DMAIT_FAULT_LOCATION_PROBE_HEADER) + strlen(DMAIT_HEADER_SEPARATOR);
    int hmacIndex = seqNumberIndex + DMAIT_SEQUENCE_NUMBER_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR);

    //Extracts sequence number
    char seqNumberStr[DMAIT_SEQUENCE_NUMBER_LENGTH + 1];
    getSubstring(seqNumberStr, data, seqNumberIndex, DMAIT_SEQUENCE_NUMBER_LENGTH);
    seqNumberStr[DMAIT_SEQUENCE_NUMBER_LENGTH] = '\0';

    //TODO: Check if the seqNumberStr is really a number
    //char *endPtr;
    //int seqNum = strtol(seqNumberStr, &endPtr, 10);

    //Extracts the HMAC
    char hmacStr[DMAIT_HMAC_LENGTH + 1];
    getSubstring(hmacStr, data, hmacIndex, DMAIT_HMAC_LENGTH);
    hmacStr[DMAIT_HMAC_LENGTH] = '\0';

    //printf("XXX seqNumberStr: %s, seqNum: %d, hmacStr: %s\n", seqNumberStr, seqNum,  hmacStr);

    //Get this node key
    char nodeKey[DMAIT_HOST_ADDR_LENGTH + 1];
    dmait_get_current_node_key(nodeKey, sizeof(nodeKey));

    char probePaylodForHmac[hmacIndex + 1];
    getSubstring(probePaylodForHmac, data, 0, hmacIndex);
    probePaylodForHmac[hmacIndex] = '\0';

    //printf("XXX probePaylodForHmac: %s\n", probePaylodForHmac);

    //generate the ack hmac
    char calcHmacStr[DMAIT_HMAC_LENGTH + 1];
    dmait_generate_hmac(probePaylodForHmac, nodeKey, calcHmacStr, sizeof(calcHmacStr));

    if (strcmp(hmacStr, calcHmacStr) == 0) {
        //os hmacs sao iguais..
        //printf("XXX HMAC RECEBIDO: %s, HMAC CALCULADO: %s, SAO IGUAIS!\n", hmacStr, calcHmacStr);

        //Transmit FL-ACK back
        //FL-ACK MESSAGE
        int flAckMessageSize = strlen(DMAIT_FAULT_LOCATION_ACK_HEADER) + strlen(DMAIT_HEADER_SEPARATOR)
                               + DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR)
                               + DMAIT_HMAC_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR); //message HMAC

        //The ack payload
        char flAckPayload[flAckMessageSize + 1];
        flAckPayload[0] = '\0';
        flAckPayload[flAckMessageSize] = '\0';

        //build the ack header...
        strcat(flAckPayload, DMAIT_FAULT_LOCATION_ACK_HEADER);
        strcat(flAckPayload, DMAIT_HEADER_SEPARATOR);

        //add this node host address
        char thisHostAddr[DMAIT_HOST_ADDR_LENGTH + 1];
        dmait_snprint_current_host_addr(thisHostAddr, sizeof(thisHostAddr));
        strcat(flAckPayload, thisHostAddr);
        strcat(flAckPayload, DMAIT_BLOCK_SEPARATOR);

        //generate the ack hmac
        char ackHmacStr[DMAIT_HMAC_LENGTH + 1];
        dmait_generate_hmac(flAckPayload, nodeKey, ackHmacStr, sizeof(ackHmacStr));

        strcat(flAckPayload, ackHmacStr);
        strcat(flAckPayload, DMAIT_BLOCK_SEPARATOR);

        //get root ip address
        uip_ipaddr_t root_ipaddr;
        NETSTACK_ROUTING.get_root_ipaddr(&root_ipaddr);

        LOG_INFO("TRANSMITINDO FL-ACK: %s\n", flAckPayload);

        //dmait_send_data(c, flAckPayload, strlen(flAckPayload), &root_ipaddr);
        dmait_send_text_data(c, flAckPayload, &root_ipaddr);

    } else {
        LOG_ERR("FL-PROBE HMAC INVALIDO (%s), MENSAGEM FORJADA OU CORROMPIDA - DESCARTANDO FL-PROBE!\n", hmacStr);
    }

}
