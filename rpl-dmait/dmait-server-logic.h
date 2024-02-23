/*
 * DMAIT - Deteccao e Mitigacao de Ataques de Interrupcao de Trafego Extension
 *
 * This code is based on the rpl-udp example from Contiki operating system.
 *
 * Created by Daniel Francis Soriano
 *
 */

#ifndef DMAIT_SERVER_LOGIC_H
#define DMAIT_SERVER_LOGIC_H


/********** Includes **********/
#include "dmait-types.h"

#include "lib/list.h"
#include "sys/node-id.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/routing/routing.h"
#include "net/routing/rpl-lite/rpl-neighbor.h"


/********** Public symbols **********/

/* DMAIT RV */

/**
 * INIT MEMB STRUCTURES
 */
void dmait_init_memb();

/**
 * Initializes server lists
 */
void dmait_init_server_lists();

/**
 * Initializes the dmaitRvValidRouteNodes (list of nodes that it's route were validated)
 */
void dmait_init_rv_valid_route_nodes();

/**
 * Initializes the dmaitArSuspectNodes (list of suspect nodes instances of dmait_suspect_node_t)
 */
void dmait_init_ar_suspect_nodes();

/**
 * Initializes the dmaitFlAckedNodeRouteList (fault location acked nodes route list)
 */
//void dmait_init_fl_acked_route_list();


/**
 * Gets the lenght of the dmaitFlAckedNodeRouteList
 * @return
 */
//int dmait_fl_acked_route_list_length();

/**
 * Add a new node to the end of dmaitFlAckedNodeRouteList
 * @param node
 */
//void dmait_fl_acked_route_list_add_node_route(dmait_fl_acked_node_route_t *nodeRoute);

/**
 * Build (instantiates) a new dmait_fl_acked_node_route_t and initializes it's internal list (acked_node_list);
 * @param hostAddr
 * @return
 */
//dmait_fl_acked_node_route_t * dmait_fl_build_acked_node_route_structure(dmait_host_addr_t *hostAddr);


/**
 * Build (instantiates) a new dmait_fl_acked_node_t from an dmait_host_addr_t
 * @param hostAddr
 * @return
 */
//dmait_fl_acked_node_t * dmait_fl_build_acked_node_structure(dmait_host_addr_t *hostAddr);

/**
 * Find a node in a dmaitFlAckedNodeRouteList and set it as valid
 * Used when an ack with HMAC is received from that node, and then just set as valid
 * @param baseNode The node that has that route
 * @param validNode The node that belongs to base node's route, and just sent an ACK
 * @return true if the route and node were found and set, false otherwise
 */
//bool dmait_fl_acked_route_list_set_node_valid(dmait_host_addr_t *baseNode, dmait_host_addr_t *validNode);

/**
 * Removes an entire route from dmaitFlAckedNodeRouteList by base node host addr
 * @param hostAddr
 * @return
 */
//bool dmait_fl_acked_route_list_remove_node_route_by_base_node_addr(dmait_host_addr_t *baseNode);

/**
 * Checks if all the nodes in a route from dmaitFlAckedNodeRouteList are valid
 * @param baseNode The route that has this route
 * @return true if all nodes in route are valid, false otherwise
 */
//bool dmait_fl_acked_route_list_validation_check(dmait_host_addr_t *baseNode);

/**
 * Returns the amount of suspect nodes
 * @return
 */
int dmait_ar_suspect_nodes_amount();

/**
 * Increment suspicion for a node
 * If node was not in the list, creates it and it's score starts at 1
 * @param nodeAddr
 * @return true if the node was already a suspect and false if it had to be created
 */
bool dmait_ar_increment_node_suspicion(dmait_host_addr_t *nodeAddr);

/**
 * Decrement node suspicion by 1
 * (Removes the node from the list if the suspicion is less than or equal 0 (zero).
 * @param nodeAddr
 * @return true if the node was already a suspect, and false if it was not found on the list
 */
bool dmait_ar_decrement_node_suspicion(dmait_host_addr_t *nodeAddr);

/**
 * Checks if node is in the dmaitArSuspectNodes
 * @param nodeAddr
 * @return
 */
bool dmait_ar_is_node_a_suspect(dmait_host_addr_t *nodeAddr);

/**
 * Gets the node suspicion value
 * @param nodeAddr
 * @return a positive score if the node is in dmaitArSuspectNodes, and 0 (zero) if the node is not in dmaitArSuspectNodes.
 */
uint8_t dmait_ar_get_node_suspicion(dmait_host_addr_t *nodeAddr);

/**
 * Print the list of suspect nodes with each score
 */
void dmait_print_suspect_nodes();

/**
 * Returns the length of the valid route nodes list
 * @return
 */
int dmait_rv_valid_route_node_list_length();

/**
 * Add a node address to valid route nodes list
 * @param node
 */
void dmait_rv_valid_route_nodes_add_node_by_addr(dmait_host_addr_t *nodeAddr);

/**
 * Removes a node address from valid route nodes list
 * @param node
 * @return true if the nodeAddr was found and removed, false otherwise
 */
bool dmait_rv_valid_route_nodes_remove_node_by_addr(dmait_host_addr_t *nodeAddr);

/**
 * Check if a node address is in valid route nodes list
 * @param node
 * @return true if the address is in the list, false otherwise
 */
bool is_node_addr_in_dmait_rv_valid_route_nodes(dmait_host_addr_t *nodeAddr);

/**
 * Process a route validation message
 * Checks if all HMACs are valid (using each node key) and checks if there's is continuity in the blocks.
 * If it's all good, send the ack back to the originating node, using the connection "c".
 * @param sender_addr
 * @param data
 * @param datalen
 * @param c
 * @return
 */
bool dmait_process_route_validation_message(const uip_ipaddr_t *sender_addr, char *data, uint16_t datalen, struct simple_udp_connection *c);

/**
//Add a new node by passing only a host address
void dmait_rv_list_add_node_by_host_addr(dmait_host_addr_t *node_host_addr);

//Removes a node from dmait_rv_list
void dmait_rv_list_remove_node(dmait_rv_node_validation_t *node);

//Removes a node from dmait_rv_list by it's host address
void dmait_rv_list_remove_node_by_host_addr(dmait_host_addr_t *node_host_addr);


//Change the acked value of a node, by it's host address
void dmait_rv_list_set_acked_by_host_addr(dmait_host_addr_t *node_host_addr, bool acked);

//Run through the list checking if all the nodes are acked
bool is_dmait_rv_list_validated();
*/

/**
 * Process DMAIT-AR message
 * @param sender_addr
 * @param data
 * @param datalen
 */
void dmait_process_alternate_route_message(const uip_ipaddr_t *sender_addr, char *data, uint16_t datalen, struct simple_udp_connection *c);

/**
 * Transmits an FL-PROBE message
 * @param hostAddr
 * @param c
 */
void dmait_fl_send_probe_msg(dmait_host_addr_t *hostAddr, struct simple_udp_connection *c);

/**
 * Process an FL-ACK message
 * If the HMAC is valid decreases hostAddr suspicion score
 * @param sender_addr
 * @param data
 * @param datalen
 */
void dmait_process_fl_ack_message(const uip_ipaddr_t *sender_addr, char *data, uint16_t datalen);

/**
 * Process DATA MESSAGE and sends an ACK back
 * @param sender_addr
 * @param data
 * @param datalen
 */
void dmait_process_data_message(const uip_ipaddr_t *sender_addr, char *data, uint16_t datalen, struct simple_udp_connection *c);

/** @} */

#endif /* DMAIT_SERVER_LOGIC_H */
