/*
 * DMAIT - Deteccao e Mitigacao de Ataques de Interrupcao de Trafego Extension
 *
 * This code is based on the rpl-udp example from Contiki operating system.
 *
 * Created by Daniel Francis Soriano
 *
 */

#ifndef DMAIT_CLIENT_LOGIC_H
#define DMAIT_CLIENT_LOGIC_H


/********** Includes **********/
#include "dmait-types.h"
#include "lib/memb.h"
#include "lib/list.h"
#include "sys/node-id.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/routing/routing.h"
#include "net/routing/rpl-lite/rpl-neighbor.h"


/********** Public symbols **********/

/* DMAIT RV LIST */


/**
 * Initializes the MEMB and the lists
 */
//void dmait_init_valid_route();

/**
 * Returns the depth of the valid route list
 * @return
 */
int dmait_valid_route_depth();


/**
 * Print the current valid route
 */
void dmait_print_valid_route();

/**
 * Add a new node to the valid route by passing it's host address
 * @param nodeAddr
 */
void dmait_valid_route_add_node_by_adddr(dmait_host_addr_t *nodeAddr);

/**
 * Removes a node from dmait_route by it's host address
 * @param nodeAddr
 * @return true if found and removed, false otherwise
 */
bool dmait_route_remove_node_by_host_addr(dmait_host_addr_t *nodeAddr);

/**
 * Check if a node addr is in the valid route
 * @param nodeAddr
 * @return true if it is, false otherwise.
 */
bool is_node_addr_in_dmait_valid_route(dmait_host_addr_t *nodeAddr);


/**
 * Clear the valid route list
 */
void dmait_clear_valid_route();

/*
 * Gets the length of the node blacklist
 * @return
 */
//int dmait_node_blacklist_length();

/*
 * Add a node to the blacklist by passing a host address
 * @param nodeAddr
 */
//void dmait_node_blacklist_add_node_by_adddr(dmait_host_addr_t *nodeAddr);
/*---------------------------------------------------------------------------*/

/*
 * Removes a node from the blacklist by passing its address
 * @param nodeAddr
 * @return
 */
//bool dmait_blacklist_remove_node_by_host_addr(dmait_host_addr_t *nodeAddr);

/*---------------------------------------------------------------------------*/

/*
 * Checks if a node address is in the blacklist
 * @param nodeAddr
 * @return
 */
//bool is_node_addr_in_dmait_node_blacklist(dmait_host_addr_t *nodeAddr);



/**
 * Start the route validation (RV) process
 * @param c
 */
void dmait_rv_start_route_validation(struct simple_udp_connection *c);

/**
 * Start the alternate route (AR) process
 * @param c
 */
void dmait_ar_start_avoid_route(struct simple_udp_connection *c);

/**
 * Send a data message with hmac, and create a timer for timeout
 * @param udp_conn
 * @param data
 * @param datalen
 * @param dest_ipaddr
 */
void dmait_send_data_message(struct simple_udp_connection *udp_conn,
                             const void *data,
                             const size_t datalen,
                             const uip_ipaddr_t *dest_ipaddr);


/**
 * Resets the unackedDataMessagesInARowCounter to zero.
 */
void resetUnackedDataMessagesCounter();

/**
 * Data message timeout callback
 * @param ptr
 */
void ctimer_data_message_timeout_callback(void *ptr);

/**
 * Registers the sequence number and hmac of the last message sent
 * @param seqNumber
 * @param hmac
 */
void dmait_register_message_sent(uint8_t seqNumber, char * hmac);

/**
 * Compares the sequence number and hmac of the last sent message
 * @param seqNumber
 * @param hmac
 * @return true if they are the same, false otherwise
 */
bool dmait_check_last_message_data(uint8_t seqNumber, char * hmac);


/**
 * Process data ack message
 * @param sender_addr
 * @param data
 * @param datalen
 */
void dmait_process_data_ack_message(const uip_ipaddr_t *sender_addr, char *data, uint16_t datalen);

/**
 * Process FL-PROBE message (and sends FL-ACK back)
 * @param sender_addr
 * @param data
 * @param datalen
 * @param c
 */
void dmait_process_fl_probe_message(const uip_ipaddr_t *sender_addr, char *data, uint16_t datalen, struct simple_udp_connection *c);

/** @} */

#endif /* DMAIT_CLIENT_LOGIC_H */
