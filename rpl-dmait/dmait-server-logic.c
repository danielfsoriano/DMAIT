/*
 * DMAIT - Deteccao e Mitigacao de Ataques de Interrupcao de Trafego Extension
 *
 * This code is based on the rpl-udp example from Contiki operating system.
 *
 * Created by Daniel Francis Soriano
 *
 */

#include <stdlib.h>

#include "contiki.h"
#include "lib/memb.h"
//#include "heapmem.h"

#include "dmait-types.h"
#include "dmait-server-logic.h"
#include "dmait-logic.h"

#include <stdio.h>
#include <string.h>

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "DMAIT"
#define LOG_LEVEL LOG_LEVEL_INFO

//#define HEAPMEM_CONF_ARENA_SIZE 1024000


//MEMB dynamic memory allocation definition for dmait_route_node_t structure
MEMB(dmait_route_node_memb, dmait_route_node_t, 100); //max 100 nodes in memory

//MEMB dynamic memory allocation definition for dmait_fl_acked_node_t structure
//MEMB(dmait_fl_acked_node_memb, dmait_fl_acked_node_t, 100); //max 1000 route nodes to validate (50 nodes times 2 depth)

//MEMB dynamic memory allocation definition for dmait_fl_acked_node_route_t structure
//MEMB(dmait_fl_acked_node_route_memb, dmait_fl_acked_node_route_t, 50); //validate max fifty nodes simultaneously (each's route)


/* Only instance of RV valid route nodes list per gateway - holds all nodes that it's route was validated */
LIST(dmaitRvValidRouteNodes); //List of dmait_route_node_t (basically a list of dmait_host_addr_t)



//MEMB dynamic memory allocation definition for dmait_route_node_t structure
MEMB(dmait_suspect_node_memb, dmait_suspect_node_t, 50); //max 50 suspect nodes in memory

/* Only instance of AR Suspect Node list */
LIST(dmaitArSuspectNodes); //List of dmait_suspect_node_t (basically a list of dmait_host_addr_t with a score for each)

/* Only instance of RV valid route nodes list per gateway - holds all nodes that it's route was validated */
//LIST(dmaitRvRouteValidationList); //List of dmait_rv_route_validation

/* Only instance of FL node validation list per gateway - holds all the routes that are in validation */
//LIST(dmaitFlAckedNodeRouteList); //List of dmait_fl_acked_node_route_t

//Used in FL PROBE only
uint8_t lastProbeMessageSequenceNumber = 0;


void dmait_init_memb() {
    memb_init(&dmait_route_node_memb);
    memb_init(&dmait_suspect_node_memb);
    //memb_init(&dmait_fl_acked_node_memb);
    //memb_init(&dmait_fl_acked_node_route_memb);
}

void dmait_init_server_lists() {
    //list_init(dmaitFlAckedNodeRouteList);
    list_init(dmaitRvValidRouteNodes);
    list_init(dmaitArSuspectNodes);
}

/*
int dmait_fl_acked_route_list_length() {
    return list_length(dmaitFlAckedNodeRouteList);
}

void dmait_fl_acked_route_list_add_node_route(dmait_fl_acked_node_route_t *nodeRoute) {
    list_add(dmaitFlAckedNodeRouteList, nodeRoute);
}

dmait_fl_acked_node_route_t * dmait_fl_build_acked_node_route_structure(dmait_host_addr_t *hostAddr) {
    dmait_fl_acked_node_route_t *nodeRoute;
    nodeRoute = memb_alloc(&dmait_fl_acked_node_route_memb);
    if (nodeRoute != NULL) {
        dmait_host_addr_copy(&nodeRoute->node_addr, hostAddr);
        LIST_STRUCT_INIT(nodeRoute, acked_node_list);
    }
    return nodeRoute;
}

dmait_fl_acked_node_t * dmait_fl_build_acked_node_structure(dmait_host_addr_t *hostAddr) {
    dmait_fl_acked_node_t *node;
    node = memb_alloc(&dmait_fl_acked_node_memb);
    if (node != NULL) {
        dmait_host_addr_copy(&node->node_addr, hostAddr);
    }
    return node;
}

bool dmait_fl_acked_route_list_set_node_valid(dmait_host_addr_t *baseNode, dmait_host_addr_t *validNode) {
    dmait_fl_acked_node_route_t *currRoute;
    currRoute = list_head(dmaitFlAckedNodeRouteList);
    while (currRoute != NULL) {
        if (dmait_host_addr_cmp(baseNode, &currRoute->node_addr)) {
            //found route
            break;
        }
        currRoute = list_item_next(currRoute);
    }
    if (currRoute != NULL) {
        //found route
        dmait_fl_acked_node_t *currNode;
        currNode = list_head(currRoute->acked_node_list);
        while (currNode != NULL) {
            if (dmait_host_addr_cmp(validNode, &currNode->node_addr)) {
                //found the node to set as valid
                currNode->acked = true;
                return true;
            }
            currNode = list_item_next(currNode);
        }
    }
    return false;
}

bool dmait_fl_acked_route_list_remove_node_route_by_base_node_addr(dmait_host_addr_t *baseNode) {
    dmait_fl_acked_node_route_t *currRoute;
    currRoute = list_head(dmaitFlAckedNodeRouteList);
    while (currRoute != NULL) {
        if (dmait_host_addr_cmp(baseNode, &currRoute->node_addr)) {
            //found route
            break;
        }
        currRoute = list_item_next(currRoute);
    }
    if (currRoute != NULL) {
        //found route
        dmait_fl_acked_node_t *currNode;
        currNode = list_pop(currRoute->acked_node_list);
        while (currNode != NULL) {
            memb_free(&dmait_fl_acked_node_memb, currNode);
            currNode = list_pop(currRoute->acked_node_list);
        }
        list_remove(dmaitFlAckedNodeRouteList, currRoute);
        memb_free(&dmait_fl_acked_node_route_memb, currRoute);
    }
    return false;
}

bool dmait_fl_acked_route_list_validation_check(dmait_host_addr_t *baseNode) {
    bool result = false;
    dmait_fl_acked_node_route_t *currRoute;
    currRoute = list_head(dmaitFlAckedNodeRouteList);
    while (currRoute != NULL) {
        if (dmait_host_addr_cmp(baseNode, &currRoute->node_addr)) {
            //found route
            break;
        }
        currRoute = list_item_next(currRoute);
    }
    if (currRoute != NULL) {
        //found route
        result = true; //if found the route, set the result as true...
        dmait_fl_acked_node_t *currNode;
        currNode = list_head(currRoute->acked_node_list);
        while (currNode != NULL) {
            //... and now check to see if any of it's nodes are not valid
            if (!currNode->acked) {
                //if found a node that is not valid, invalidate the answer and exit loop
                result = false;
                break;
            }
            currNode = list_item_next(currNode);
        }
    }
    return result;
}
*/

void dmait_print_node_suspicion_line(dmait_suspect_node_t *node) {
    dmait_print_host_addr(&node->node_addr);
    printf(" - NEGATIVE: %d, POSITIVE: %d, DIFFERENCE: %d, RATIO: %.2f\n",
           node->negative_score,
           node->positive_score,
           (node->negative_score - node->positive_score),
           ((float) node->positive_score / (float) node->negative_score));
}

void dmait_print_suspect_nodes() {
    LOG_INFO("**CURRENT SUSPECTS**\n");
    dmait_suspect_node_t *currNode;
    currNode = list_head(dmaitArSuspectNodes);
    while (currNode != NULL) {
        dmait_print_node_suspicion_line(currNode);
/*
        dmait_print_host_addr(&currNode->node_addr);
        printf(" - NEGATIVE: %d, POSITIVE: %d, DIFFERENCE: %d, RATIO: %.2f\n",
               currNode->negative_score,
               currNode->positive_score,
               (currNode->negative_score - currNode->positive_score),
               ((float) currNode->positive_score / (float) currNode->negative_score));
*/
        currNode = list_item_next(currNode);
    }
    LOG_INFO("****\n");
}

int dmait_ar_suspect_nodes_amount() {
    return list_length(dmaitArSuspectNodes);
}

bool dmait_ar_increment_node_suspicion(dmait_host_addr_t *nodeAddr) {
    dmait_suspect_node_t *currNode;
    currNode = list_head(dmaitArSuspectNodes);
    while (currNode != NULL) {
        dmait_suspect_node_t *nextNode;
        nextNode = list_item_next(currNode);
        if (dmait_host_addr_cmp(nodeAddr, &currNode->node_addr)) {
            currNode->negative_score++;

            //PRINT
            printf("NODE SUSPICION HAS BEEN INCREASED! - ");
            dmait_print_node_suspicion_line(currNode);
            return true;
        }
        currNode = nextNode;
    }
    //if got here, found no node.. so create one
    dmait_suspect_node_t *newNode;
    newNode = memb_alloc(&dmait_suspect_node_memb);
    if (newNode != NULL) {
        //LOG_INFO("MEMORY ALLOCATED IN dmait_suspect_node_memb \n");
        dmait_host_addr_copy(&newNode->node_addr, nodeAddr);
        newNode->negative_score=1; //initial value
        newNode->positive_score=0; //initial value
        list_add(dmaitArSuspectNodes, newNode);

        //PRINT
        printf("NODE SUSPICION HAS BEEN INCREASED! - ");
        dmait_print_node_suspicion_line(newNode);
    } else {
        LOG_ERR("ERRO-ALOCACAO dmait_route_node_memb \n");
    }
    return false;
}


bool dmait_ar_decrement_node_suspicion(dmait_host_addr_t *nodeAddr) {
    dmait_suspect_node_t *currNode;
    currNode = list_head(dmaitArSuspectNodes);
    while (currNode != NULL) {
        dmait_suspect_node_t *nextNode;
        nextNode = list_item_next(currNode);
        if (dmait_host_addr_cmp(nodeAddr, &currNode->node_addr)) {
            currNode->positive_score++;

            //PRINT
            printf("NODE SUSPICION HAS BEEN DECREASED! - ");
            dmait_print_node_suspicion_line(currNode);
/*
            if (currNode->score <= 0) {
                //removes the node from the list
                list_remove(dmaitArSuspectNodes, currNode);
                memb_free(&dmait_route_node_memb, currNode);
            }
*/
            return true;
        }
        currNode = nextNode;
    }
    return false;
}

bool dmait_ar_is_node_a_suspect(dmait_host_addr_t *nodeAddr) {
    dmait_suspect_node_t *currNode;
    currNode = list_head(dmaitArSuspectNodes);
    while (currNode != NULL) {
        if (dmait_host_addr_cmp(&currNode->node_addr, nodeAddr)) {
            return true;
        }
        currNode = list_item_next(currNode);
    }
    return false;
}

uint8_t dmait_ar_get_node_suspicion(dmait_host_addr_t *nodeAddr) {
    uint8_t result = 0;
    dmait_suspect_node_t *currNode;
    currNode = list_head(dmaitArSuspectNodes);
    while (currNode != NULL) {
        if (dmait_host_addr_cmp(&currNode->node_addr, nodeAddr)) {
            result = currNode->negative_score - currNode->positive_score;
            break;
        }
        currNode = list_item_next(currNode);
    }
    return result;
}





/*---------------------------------------------------------------------------*/
int dmait_rv_valid_route_node_list_length() {
    return list_length(dmaitRvValidRouteNodes);
}

void dmait_rv_valid_route_nodes_add_node_by_addr(dmait_host_addr_t *nodeAddr) {
    dmait_route_node_t *newNode;
    newNode = memb_alloc(&dmait_route_node_memb);
    //newNode = (dmait_route_node_t *) heapmem_alloc(sizeof(dmait_route_node_t));
    if (newNode != NULL) {
        //LOG_INFO("MEMORY ALLOCATED IN dmait_route_node_memb \n");
        dmait_host_addr_copy(&newNode->node_addr, nodeAddr);
        list_add(dmaitRvValidRouteNodes, newNode);
    } else {
        LOG_ERR("ERRO-ALOCACAO dmait_route_node_memb \n");
    }
}

bool dmait_rv_valid_route_nodes_remove_node_by_addr(dmait_host_addr_t *nodeAddr) {
    dmait_route_node_t *currNode;
    currNode = list_head(dmaitRvValidRouteNodes);
    while (currNode != NULL) {
        dmait_route_node_t *nextNode;
        nextNode = list_item_next(currNode);
        if (dmait_host_addr_cmp(nodeAddr, &currNode->node_addr)) {
            list_remove(dmaitRvValidRouteNodes, currNode);
            memb_free(&dmait_route_node_memb, currNode);
            //heapmem_free(currNode);
            return true;
        }
        currNode = nextNode;
    }
    return false;
}

bool is_node_addr_in_dmait_rv_valid_route_nodes(dmait_host_addr_t *nodeAddr) {
    dmait_route_node_t *currNode;
    currNode = list_head(dmaitRvValidRouteNodes);
    while (currNode != NULL) {
        if (dmait_host_addr_cmp(&currNode->node_addr, nodeAddr)) {
            return true;
        }
        currNode = list_item_next(currNode);
    }
    return false;
}

/*---------------------------------------------------------------------------*/

bool dmait_process_route_validation_message(const uip_ipaddr_t *sender_addr, char *data, uint16_t datalen, struct simple_udp_connection *c) {

    //LOG_INFO("PROCESSING-RV\n");

    bool validated = true; //initial value

    LIST(routeValidationList);  //list of dmait_route_node_t
    list_init(routeValidationList);

    //the null (0.0.0.0) address
    dmait_host_addr_t zeroNodeAddr;
    dmait_hostAddr(&zeroNodeAddr, 0,0,0,0);

    dmait_triple_node_block_t tnbAux;
    char *blockState;

    //first node address and key
    bool hasFirstNodeData = false;
    dmait_host_addr_t firstNodeAddr;
    char firstNodeKey[30];

    //previous data
    bool hasPrevious = false;
    dmait_host_addr_t prev_node_addr, prev_next_node_addr;

    int counter = 0;

    //LOG_INFO("PARSING ROUTE VALIDATION MESSAGE - BEGIN\n");

    bool hasNext = dmait_parse_next_block_from_rv_payload(data, &tnbAux, &blockState);


    while(hasNext && validated) {
        counter++;

//        dmait_print_triple_node_block(tnbAux);
//
//        //Debug block
//        if (hasPrevious) {
//            uip_ip6addr_t nodeAddrAux;
//
//            printf("HAS PREVIOUS: NODE: ");
//            buildLinkLocalUnicastAddr(&nodeAddrAux, &prev_node_addr);
//            LOG_INFO_6ADDR(&nodeAddrAux);
//            printf(", NEXT NODE: ");
//            buildLinkLocalUnicastAddr(&nodeAddrAux, &prev_next_node_addr);
//            LOG_INFO_6ADDR(&nodeAddrAux);
//            printf("\n");
//
//            printf("HAS PREVIOUS: (HOSTADDR) NODE: ");
//            dmait_print_host_addr(&prev_node_addr);
//            printf(", NEXT (HOSTADDR) NODE: ");
//            dmait_print_host_addr(&prev_next_node_addr);
//            printf("\n");
//        }

        dmait_rv_route_validation_t rv;
        dmait_host_addr_copy(&rv.tnb.node_addr, &tnbAux.node_addr);
        dmait_host_addr_copy(&rv.tnb.prev_node_addr, &tnbAux.prev_node_addr);
        dmait_host_addr_copy(&rv.tnb.next_node_addr, &tnbAux.next_node_addr);
        strcpy(rv.tnb.hmac_digest, tnbAux.hmac_digest);
        rv.hmacValidated = false; //default
        rv.previousValidated = false;

        //Check HMAC
        char nodeKey[30];
        dmait_get_node_key_for_host_addr(nodeKey, 30, &rv.tnb.node_addr);
        //printf("VALIDATING HMAC FOR BLOCK NODE: %s - LOOP COUNT: %d\n", nodeKey, counter);
        rv.hmacValidated = dmait_verify_hmac_in_triple_node_block(&rv.tnb, nodeKey);
        if (rv.hmacValidated) {
            //printf("HMAC VALIDATED FOR NODE: %s\n", nodeKey);

            //Check continuity - THIS <-> PREVIOUS NODE
            if (hasPrevious) {
                //printf("hasPrevious = true\n");

                if (dmait_host_addr_cmp(&prev_node_addr, &rv.tnb.prev_node_addr)
                    && dmait_host_addr_cmp(&prev_next_node_addr, &rv.tnb.node_addr)) {
                    rv.previousValidated = true;
                    //printf("previousValidated = true\n");
                } else {
                    validated = false;
                    //printf("previousValidated = false\n");
                }

            } else {
                //printf("hasPrevious = false\n");

                if (dmait_host_addr_cmp(&zeroNodeAddr, &rv.tnb.prev_node_addr)) {
                    rv.previousValidated = true;
                    //printf("previousValidated = true\n");
                } else {
                    validated = false;
                    //printf("previousValidated = false\n");
                }
            }

        } else {
            LOG_ERR("HMAC INVALID-NODE: %s\n", nodeKey);
            validated = false;
        }

        if (!hasFirstNodeData) {
            //set first node data (to be used later as the destination node)
            hasFirstNodeData = true;
            dmait_host_addr_copy(&firstNodeAddr, &rv.tnb.node_addr);
            strcpy(firstNodeKey, nodeKey);
        }

        //Copy current to previous
        hasPrevious = true;
        dmait_host_addr_copy(&prev_node_addr, &rv.tnb.node_addr);
        dmait_host_addr_copy(&prev_next_node_addr, &rv.tnb.next_node_addr);

        //Add the address to the route list
        dmait_route_node_t *node = memb_alloc(&dmait_route_node_memb);
        if (node != NULL) {
            dmait_host_addr_copy(&node->node_addr, &rv.tnb.node_addr);
            list_add(routeValidationList, node);
            //LOG_INFO("ADDED NODE TO LIST IN ROUTE VALIDATION\n");
        } else {
            LOG_ERR("ERRO-ALOCACAO dmait_route_node_memb-ABORTING RV\n");

            //Unallocate the list before returning.
            //Iterate routeValidationList and add remove and unallocate each
            if (list_length(routeValidationList) > 0) {
                dmait_route_node_t *node;
                node = list_head(routeValidationList);
                while (node != NULL) {
                    dmait_route_node_t *nextNode;
                    nextNode = list_item_next(node);

                    list_remove(routeValidationList, node);
                    memb_free(&dmait_route_node_memb, node);
                    node = nextNode;
                }
            }
            return false;
        }

        //get next
        hasNext = dmait_parse_next_block_from_rv_payload(NULL, &tnbAux, &blockState);
    }

    if (validated) {
        //LOG_INFO("CONTINUITY VALIDATED IN %d BLOCKS\n", counter);

        //Add to the valid route node list - dmaitRvValidRouteNodes
        if (!is_node_addr_in_dmait_rv_valid_route_nodes(&firstNodeAddr)) {
            dmait_rv_valid_route_nodes_add_node_by_addr(&firstNodeAddr);
            //LOG_INFO("ADDED NEW NODE TO THE VALID ROUTE NODES LIST\n");
        } else {
            //LOG_INFO("NODE ALREADY IN THE VALID ROUTE NODES LIST\n");
        }

        //Build ack payload msg
        int listLength = list_length(routeValidationList);
        //LOG_INFO("**************** LIST LENGTH - routeValidationList: %d\n", listLength);
        if (listLength > 0) {
            //Sums the header length, plus one address length per list entry (+1 separator for each entry),
            // +10 for the hmac, and +1 at the end to terminate the String
            int ackPayloadLength = strlen(DMAIT_ROUTE_VALIDATION_ACK_HEADER) + strlen(DMAIT_HEADER_SEPARATOR)
                    + 2 + strlen(DMAIT_HEADER_SEPARATOR)
                    + (listLength * (DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_FIELD_SEPARATOR)))
                    + DMAIT_HMAC_LENGTH;
            char ackPayload[ackPayloadLength + 1];
            ackPayload[0] = '\0'; //to start the concatenation from the beginning
            ackPayload[ackPayloadLength] = '\0'; //At the end.. as failsafe

            //concat the header and separator
            strcat(ackPayload, DMAIT_ROUTE_VALIDATION_ACK_HEADER);
            strcat(ackPayload, DMAIT_HEADER_SEPARATOR);

            char addrAmount[3];
            snprintf(addrAmount, sizeof(addrAmount), "%02d", listLength);
            strcat(ackPayload, addrAmount);
            strcat(ackPayload, DMAIT_HEADER_SEPARATOR);

            //Iterate routeValidationList and add each addr to the payload
            dmait_route_node_t *node;
            node = list_head(routeValidationList);
            while(node != NULL) {
                dmait_route_node_t *nextNode;
                nextNode = list_item_next(node);

                //Concatenates the address to the payload
                char addrAux[DMAIT_HOST_ADDR_LENGTH + 2]; //plus 2 for separator and string terminator
                dmait_snprint_host_addr(addrAux, DMAIT_HOST_ADDR_LENGTH + 2, &node->node_addr);
                addrAux[DMAIT_HOST_ADDR_LENGTH] = '\0'; //so the addr is terminated before concat the separator
                if (nextNode != NULL) {
                    strcat(addrAux, DMAIT_FIELD_SEPARATOR);
                } else {
                    strcat(addrAux, DMAIT_BLOCK_SEPARATOR);
                }
                //addrAux[DMAIT_HOST_ADDR_LENGTH + 1] = '\0';
                strcat(ackPayload, addrAux);

                list_remove(routeValidationList, node);
                memb_free(&dmait_route_node_memb, node);
                node = nextNode;
                //LOG_INFO("**************** LIST LENGTH - routeValidationList: %d\n", list_length(routeValidationList));
            }

            //generate hmac for the message using the destination node key
            char hmacStr[11];
            dmait_generate_hmac(ackPayload, firstNodeKey, hmacStr, sizeof(hmacStr));
            strcat(ackPayload, hmacStr);

            //printf("***************** ackPayload calculated length: %d, and real length: %d\n", (int) ackPayloadLength, (int) strlen(ackPayload));
            //LOG_INFO("***************** ackPayload: %s\n", ackPayload);

            //Transmits the RV ACK back to the destination
            uip_ipaddr_t destinationAddr;
            buildDS6DefaultPrefixAddr(&destinationAddr, &firstNodeAddr);
            //LOG_INFO("SENDING ACK TO NODE: ");
            LOG_INFO("ROUTE VALIDATED-NODE: ");
            LOG_INFO_6ADDR(&destinationAddr);
            printf(" - SENDING ACK: %s\n", ackPayload);
            //dmait_send_data(c, ackPayload, strlen(ackPayload), &destinationAddr);
            dmait_send_text_data(c, ackPayload, &destinationAddr);
        }

    } else { //not validated
        //LOG_INFO("CONTINUITY AND/OR HASH NOT VALIDATED! %d BLOCKS\n", counter);

        //Just in case removes the node from the validated route nodes list
        dmait_rv_valid_route_nodes_remove_node_by_addr(&firstNodeAddr);

        LOG_ERR("ROUTE NOT VALIDATED-NODE: ");
        uip_ipaddr_t destinationAddr;
        buildDS6DefaultPrefixAddr(&destinationAddr, &firstNodeAddr);
        LOG_INFO_6ADDR(&destinationAddr);
        LOG_INFO_("\n");
    }

    //LOG_INFO("PARSING ROUTE VALIDATION MESSAGE - END - %d BLOCKS\n", counter);

    //Just in case, unallocate the list before returning.
    //Iterate routeValidationList and add remove and unallocate each
    if (list_length(routeValidationList) > 0) {
        //LOG_INFO("UNNALLOCATING routeValidationList\n");
        dmait_route_node_t *node;
        node = list_head(routeValidationList);
        while (node != NULL) {
            dmait_route_node_t *nextNode;
            nextNode = list_item_next(node);

            list_remove(routeValidationList, node);
            memb_free(&dmait_route_node_memb, node);
            node = nextNode;
        }
    }

    return validated;
}



/*---------------------------------------------------------------------------*/
/*
void dmait_rv_list_add_node_by_host_addr(dmait_host_addr_t *node_host_addr) {
    dmait_rv_node_validation_t * new_node;
    //new_node = (dmait_rv_node_t *) malloc(sizeof(dmait_rv_node_t));
    new_node = memb_alloc(&dmait_rv_node_validation_memb);
    if (new_node != NULL) {
        new_node->acked = false; //default
        memcpy(&new_node->node_addr, node_host_addr, sizeof(dmait_host_addr_t));
        list_add(dmait_rv_list, new_node);
    } else {
        LOG_ERR("ERRO AO ALOCAR MEMORIA EM dmait_rv_list_add_node_by_host_addr");
    }
}



void dmait_rv_list_remove_node(dmait_rv_node_validation_t *node) {
    list_remove(dmait_rv_list, node);
    memb_free(&dmait_rv_node_validation_memb, node);
}



void dmait_rv_list_remove_node_by_host_addr(dmait_host_addr_t *node_host_addr) {
    dmait_rv_node_validation_t *curr;
    curr = list_head(dmait_rv_list);
    while (curr != NULL) {
        if (dmait_host_addr_cmp(&curr->node_addr, node_host_addr)) {
            list_remove(dmait_rv_list, curr);
            memb_free(&dmait_rv_node_validation_memb, curr);
            return;
        }
        curr = list_item_next(curr);
    }
}




void dmait_rv_list_set_acked_by_host_addr(dmait_host_addr_t *node_host_addr, bool acked) {
    dmait_rv_node_validation_t *curr;
    curr = list_head(dmait_rv_list);
    while (curr != NULL) {
        if (dmait_host_addr_cmp(&curr->node_addr, node_host_addr)) {
            curr->acked = acked;
            return;
        }
        curr = list_item_next(curr);
    }
}



bool is_dmait_rv_list_validated() {
    dmait_rv_node_validation_t *curr;
    curr = list_head(dmait_rv_list);
    while (curr != NULL) {
        if (!curr->acked) {
            return false;
        }
        curr = list_item_next(curr);
    }
    return true;
}
*/

/*----------------------------------------------------------------*/

void dmait_process_alternate_route_message(const uip_ipaddr_t *sender_addr, char *data, uint16_t datalen, struct simple_udp_connection *c) {

    //LOG_INFO("PROCESSANDO MENSAGEM AR\n");

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

    //Creates a list with all the nodes to avoid.
    dmait_host_addr_t avoidRouteArray[avoidNodesAmount];

    char currNodeHostAddr[DMAIT_HOST_ADDR_LENGTH + 1];
    int x;
    for (x = 0; x < avoidNodesAmount; x++) {
        getSubstring(currNodeHostAddr, data, avoidNodesStart + (x * (DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_FIELD_SEPARATOR))), DMAIT_HOST_ADDR_LENGTH);
        //LOG_PRINT("########### currAvoidNodeHostAddr-%d: %s\n", x, currNodeHostAddr);
        dmait_build_host_addr_from_block_field(currNodeHostAddr, &avoidRouteArray[x]);
    }

    //printf("avoidNodesAmount: %d\n", avoidNodesAmount);

    //calculates the amount of route nodes
    int routeNodesStart = avoidNodesStart + (avoidNodesAmount * (DMAIT_HOST_ADDR_LENGTH)) + ((avoidNodesAmount -1) * strlen(DMAIT_FIELD_SEPARATOR)) + strlen(DMAIT_BLOCK_SEPARATOR)
                          + DMAIT_HMAC_LENGTH + strlen(DMAIT_FIELD_SEPARATOR)
                          + strlen(DMAIT_ALTERNATE_ROUTE_ROUTE_HEADER) + strlen(DMAIT_BLOCK_SEPARATOR);

    int routeLength = 1;
    while ((routeNodesStart + (routeLength * (DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_FIELD_SEPARATOR)))) < datalen) {
        routeLength++;
    }

    //Creates a list with all the nodes to avoid.
    dmait_host_addr_t routeArray[routeLength];

    for (x = 0; x < routeLength; x++) {
        getSubstring(currNodeHostAddr, data, routeNodesStart + (x * (DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_FIELD_SEPARATOR))), DMAIT_HOST_ADDR_LENGTH);
        //LOG_PRINT("########### avoidNodesStart-%d: %s\n", routeLength, currNodeHostAddr);
        dmait_build_host_addr_from_block_field(currNodeHostAddr, &routeArray[x]);
    }
    //printf("routeLength: %d\n", routeLength);

    //Add all the suspect nodes to the suspect nodes list (except those that are in the route)
    for (x=0; x < avoidNodesAmount; x++) {
        bool isInRoute = false;
        int y;
        for (y=0; y < routeLength && !isInRoute; y++) {
            if (dmait_host_addr_cmp(&avoidRouteArray[x], &routeArray[y])) {
                //if the avoid node is in the route list, it means that the message got to the root through that node,
                //so that node is not a suspect anymore
                isInRoute = true;
            }
        }
        if (!isInRoute) {
            //if it's not in the route, it's still a suspect
            dmait_ar_increment_node_suspicion(&avoidRouteArray[x]);

            //Send a FL-PROBE message to each node to check if it is alive
            dmait_fl_send_probe_msg(&avoidRouteArray[x], c);

//            if (!is_node_addr_in_dmait_ar_suspect_nodes(&avoidRouteArray[x])) {
//                dmait_ar_suspect_nodes_add_node_by_addr(&avoidRouteArray[x]);
//            }

        }
    }

    //printf("LISTA DE NOS SUSPEITOS: %d\n", dmait_ar_suspect_nodes_amount());
    dmait_print_suspect_nodes();

//    printf("LIST OF AVOID NODES + ROUTE NODES\n");
//    for (x=0; x < avoidNodesAmount + routeLength; x++) {
//        dmait_print_host_addr(&avoidRouteArray[x]);
//        printf("\n");
//    }

}

void dmait_fl_send_probe_msg(dmait_host_addr_t *hostAddr, struct simple_udp_connection *c) {
    //build fl probe message
    int probeMessageSize = strlen(DMAIT_FAULT_LOCATION_PROBE_HEADER) + strlen(DMAIT_HEADER_SEPARATOR)
                         + DMAIT_SEQUENCE_NUMBER_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR)
                         + DMAIT_HMAC_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR); //message HMAC

    //The ack payload
    char probePayload[probeMessageSize + 1];
    probePayload[0] = '\0';
    probePayload[probeMessageSize] = '\0';

    //build the ack header...
    strcat(probePayload, DMAIT_FAULT_LOCATION_PROBE_HEADER);
    strcat(probePayload, DMAIT_HEADER_SEPARATOR);

    //... add the seq number...
    //concat the "SEQUENCE NUMBER" (up to 9999)
    int seqNumber = lastProbeMessageSequenceNumber + 1;
    if (seqNumber > DMAIT_MAX_SEQUENCE_NUMBER) {
        seqNumber = 0;
    }
    char seqNumberDigits[DMAIT_SEQUENCE_NUMBER_LENGTH + 1];
    snprintf(seqNumberDigits, sizeof(seqNumberDigits), "%04d", seqNumber);
    strcat(probePayload, seqNumberDigits);
    strcat(probePayload, DMAIT_BLOCK_SEPARATOR);

    //Get the sender node key
    char senderNodeKey[DMAIT_HOST_ADDR_LENGTH + 1];
    dmait_get_node_key_for_host_addr(senderNodeKey, sizeof(senderNodeKey), hostAddr);

    //generate the ack hmac
    char ackHmacStr[DMAIT_HMAC_LENGTH + 1];
    dmait_generate_hmac(probePayload, senderNodeKey, ackHmacStr, sizeof(ackHmacStr));

    strcat(probePayload, ackHmacStr);
    strcat(probePayload, DMAIT_BLOCK_SEPARATOR);

    //build ipaddress (routed using RPL) from hostAddr
    uip_ipaddr_t targetAddr;
    buildDS6DefaultPrefixAddr(&targetAddr, hostAddr);

    //Transmit the data
    //LOG_INFO("SENDING DMAIT-FL-PROBE: %s,  TO NODE: ", probePayload);
    //LOG_INFO_6ADDR(&targetAddr);
    //printf("\n");

    //dmait_send_data (c, probePayload, strlen(probePayload), &targetAddr);
    dmait_send_text_data (c, probePayload, &targetAddr);
}


void dmait_process_fl_ack_message(const uip_ipaddr_t *sender_addr, char *data, uint16_t datalen) {

    //LOG_INFO("PROCESSANDO MENSAGEM FL-ACK\n");


//        //FL-ACK MESSAGE
//        int flAckMessageSize = strlen(DMAIT_FAULT_LOCATION_ACK_HEADER) + strlen(DMAIT_HEADER_SEPARATOR)
//                               + DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR)
//                               + DMAIT_HMAC_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR); //message HMAC

    //pre calculate a few indexes (more indexes later on)
    int hostAddrIndex = strlen(DMAIT_FAULT_LOCATION_ACK_HEADER) + strlen(DMAIT_HEADER_SEPARATOR);
    int hmacIndex = hostAddrIndex + DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR);

    //Extracts the host address
    char hostAddrStr[DMAIT_HOST_ADDR_LENGTH + 1];
    getSubstring(hostAddrStr, data, hostAddrIndex, DMAIT_HOST_ADDR_LENGTH);
    hostAddrStr[DMAIT_HOST_ADDR_LENGTH] = '\0';

    //Extracts the HMAC
    char hmacStr[DMAIT_HMAC_LENGTH + 1];
    getSubstring(hmacStr, data, hmacIndex, DMAIT_HMAC_LENGTH);
    hmacStr[DMAIT_HMAC_LENGTH] = '\0';

    //printf("XXX hostAddrStr: %s, hmacStr: %s\n", hostAddrStr,  hmacStr);

    //Get the sender node key
    dmait_host_addr_t senderHostAddr;
    extractHostAddrFromIpv6(&senderHostAddr,  sender_addr);
    char senderNodeKey[DMAIT_HOST_ADDR_LENGTH + 1];
    dmait_get_node_key_for_host_addr(senderNodeKey, sizeof(senderNodeKey), &senderHostAddr);

    char paylodForHmac[hmacIndex + 1];
    getSubstring(paylodForHmac, data, 0, hmacIndex);
    paylodForHmac[hmacIndex] = '\0';

    //printf("XXX paylodForHmac: %s\n", paylodForHmac);

    //generate the ack hmac
    char calcHmacStr[DMAIT_HMAC_LENGTH + 1];
    dmait_generate_hmac(paylodForHmac, senderNodeKey, calcHmacStr, sizeof(calcHmacStr));

    if (strcmp(hmacStr, calcHmacStr) == 0) {
        //os hmacs sao iguais..
        //LOG_INFO("XXX HMAC RECEBIDO: %s, HMAC CALCULADO: %s, SAO IGUAIS!\n", hmacStr, calcHmacStr);
        LOG_INFO("HMAC VALIDADO-REDUZINDO SUSPEITA-");
        dmait_print_host_addr(&senderHostAddr);
        LOG_INFO_("\n");

        //decrease node suspicion
        dmait_ar_decrement_node_suspicion(&senderHostAddr);
        dmait_print_suspect_nodes();

    } else {
        LOG_ERR("FL-ACK HMAC INVALIDO-IGNORANDO FL-ACK!\n"); //mensagem forjada ou corrompida
    }

}

/*----------------------------------------------------------------*/

void dmait_process_data_message(const uip_ipaddr_t *sender_addr, char *data, uint16_t datalen, struct simple_udp_connection *c) {

    //LOG_INFO("PROCESSANDO MENSAGEM DE DADOS\n");

//    int dataMessageSize = strlen(DMAIT_DATA_MESSAGE_HEADER) + strlen(DMAIT_HEADER_SEPARATOR)
//            + DMAIT_SEQUENCE_NUMBER_LENGTH + strlen(DMAIT_HEADER_SEPARATOR) //4 chars for sequence number
//            + DMAIT_DATALEN_LENGHT + strlen(DMAIT_HEADER_SEPARATOR) //3 chars for data length, ie: 009 or 120, and another separator
//            + truncatedDataLenght + strlen(DMAIT_BLOCK_SEPARATOR) //the data length (not higher than 999)
//            //+ DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR)
//            + DMAIT_HMAC_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR);  //hmac + separator


    //pre calculate a few indexes (mode indexes later on)
    int seqNumberIndex = strlen(DMAIT_DATA_MESSAGE_HEADER) + strlen(DMAIT_HEADER_SEPARATOR);
    int dataLenghtIndex = seqNumberIndex + DMAIT_SEQUENCE_NUMBER_LENGTH + strlen(DMAIT_HEADER_SEPARATOR); //4 chars for sequence number
    int dataIndex = dataLenghtIndex + DMAIT_DATALEN_LENGHT + strlen(DMAIT_HEADER_SEPARATOR); //3 chars for data length, ie: 009 or 120, and another separator

    //Extracts sequence number
    char seqNumberStr[DMAIT_SEQUENCE_NUMBER_LENGTH + 1];
    getSubstring(seqNumberStr, data, seqNumberIndex, DMAIT_SEQUENCE_NUMBER_LENGTH);
    seqNumberStr[DMAIT_SEQUENCE_NUMBER_LENGTH] = '\0';

    char *endPtr;
    int seqNum = strtol(seqNumberStr, &endPtr, 10);
    if (seqNum < 0 || seqNum > DMAIT_MAX_SEQUENCE_NUMBER) {
        LOG_ERR("INVALID SEQUENCE NUMBER: %d\n", seqNum); //SEQUENCE NUMBER MUST BE IN BETWEEN 0 AND DMAIT_MAX_SEQUENCE_NUMBER
    }

    //Extracts the data lenght
    char dataLenghtStr[DMAIT_DATALEN_LENGHT + 1];
    getSubstring(dataLenghtStr, data, dataLenghtIndex, DMAIT_DATALEN_LENGHT);
    dataLenghtStr[DMAIT_DATALEN_LENGHT] = '\0';

    //converts dataLenghtStr to an int
    int dataLenght = strtol(dataLenghtStr, &endPtr, 10);
    //printf("### seqNumberStr: %s, dataLenghtStr: %s, dataLenght (int): %d\n", seqNumberStr, dataLenghtStr, dataLenght);

    char dataStr[dataLenght + 1];
    getSubstring(dataStr, data, dataIndex, dataLenght);
    dataStr[dataLenght] = '\0';

//    //Calculates a few more indexes
//    int hostAddrIndex = dataIndex + dataLenght + strlen(DMAIT_BLOCK_SEPARATOR);
//    int hmacIndex = hostAddrIndex + DMAIT_HOST_ADDR_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR);
//
//    //Extracts the host addr
//    char hostAddrStr[DMAIT_HOST_ADDR_LENGTH + 1];
//    getSubstring(hostAddrStr, data, hostAddrIndex, DMAIT_HOST_ADDR_LENGTH);
//    hostAddrStr[DMAIT_HOST_ADDR_LENGTH] = '\0';

    int hmacIndex = dataIndex + dataLenght + strlen(DMAIT_BLOCK_SEPARATOR);

    //Extracts the hmac
    char hmacStr[DMAIT_HMAC_LENGTH + 1];
    getSubstring(hmacStr, data, hmacIndex, DMAIT_HMAC_LENGTH);
    hmacStr[DMAIT_HMAC_LENGTH] = '\0';

    //printf("### dataStr: %s, dataIndex: %d, hmacStr: %s, hmacIndex: %d\n", dataStr, dataIndex, hmacStr, hmacIndex);

    //build ACK message
    int ackMessageSize = strlen(DMAIT_DATA_MESSAGE_ACK_HEADER) + strlen(DMAIT_HEADER_SEPARATOR)
            + DMAIT_SEQUENCE_NUMBER_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR)
            + DMAIT_HMAC_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR) //original HMAC
            + DMAIT_HMAC_LENGTH + strlen(DMAIT_BLOCK_SEPARATOR); //new HMAC for this message

    //The ack payload
    char ackPayload[ackMessageSize + 1];
    ackPayload[0] = '\0';
    ackPayload[ackMessageSize] = '\0';

    //build the ack header...
    strcat(ackPayload, DMAIT_DATA_MESSAGE_ACK_HEADER);
    strcat(ackPayload, DMAIT_HEADER_SEPARATOR);

    //... add the seq number...
    strcat(ackPayload, seqNumberStr);
    strcat(ackPayload, DMAIT_BLOCK_SEPARATOR);

    //... add original HMAC...
    strcat(ackPayload, hmacStr);
    strcat(ackPayload, DMAIT_BLOCK_SEPARATOR);

    //Get the sender node key
    dmait_host_addr_t senderHostAddr;
    extractHostAddrFromIpv6(&senderHostAddr,  sender_addr);
    char senderNodeKey[DMAIT_HOST_ADDR_LENGTH + 1];
    dmait_get_node_key_for_host_addr(senderNodeKey, sizeof(senderNodeKey), &senderHostAddr);

    //generate the ack hmac
    char ackHmacStr[DMAIT_HMAC_LENGTH + 1];
    dmait_generate_hmac(ackPayload, senderNodeKey, ackHmacStr, sizeof(ackHmacStr));

    strcat(ackPayload, ackHmacStr);
    strcat(ackPayload, DMAIT_BLOCK_SEPARATOR);

    //Transmit the data
    LOG_INFO("SENDING DMAIT-DM-ACK: %s,  TO NODE: ", ackPayload);
    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_("\n");

    //dmait_send_data(c, ackPayload, strlen(ackPayload), sender_addr);
    dmait_send_text_data(c, ackPayload, sender_addr);

}