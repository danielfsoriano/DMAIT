/*
 * DMAIT - Deteccao e Mitigacao de Ataques de Interrupcao de Trafego Extension
 *
 * This code is based on the rpl-udp example from Contiki operating system.
 *
 * Created by Daniel Francis Soriano
 *
 */

#ifndef DMAIT_LOGIC_H
#define DMAIT_LOGIC_H


/********** Includes **********/
#include "dmait-types.h"

#include "random.h"

#include "lib/list.h"
#include "sys/node-id.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/routing/routing.h"
#include "net/routing/rpl-lite/rpl-neighbor.h"


/********** Public symbols **********/

/** \brief MAX ROUTES PER NODE */
//#define MRV_MAX_ROUTES  10

#define SHA256_HASH_SIZE 32

/** dmait_host_addr_t CMP MACRO */
#define dmait_host_addr_cmp(addr1, addr2) (memcmp(addr1, addr2, sizeof(dmait_host_addr_t)) == 0)
/** dmait_host_addr_t COPY MACRO */
#define dmait_host_addr_copy(dest, src) (*((dmait_host_addr_t *)dest) = *((dmait_host_addr_t *)src))
/** dmait_triple_node_block_t COPY MACRO */
//#define dmait_triple_node_block_copy(dest, src) (*((dmait_triple_node_block_t *)dest) = *((dmait_triple_node_block_t *)src))


/** linkaddr_t CMP MACRO */
#define lladdr_cmp(addr1, addr2) (memcmp(addr1, addr2, sizeof(linkaddr_t)) == 0)
/** linkaddr_t COPY MACRO */
#define lladdr_copy(dest, src) (*((linkaddr_t *)dest) = *((linkaddr_t *)src))


/********** Public functions **********/
/*
//Initialize the route list
void dmait_init_routes();

//Get the route list lenght
int dmait_routes_length();

//Add a route to the list
void dmait_add_route(dmait_route_t *route);

//Remove a route from the list
void dmait_remove_route(dmait_route_t *route);

//Get a route by it's ID or null if not found
dmait_route_t* dmait_get_route_by_id(uint8_t route_id);

//Get a route by an ip address in it's list
dmait_route_t * dmait_get_first_route_by_ip_addr(uip_ip6addr_t *ip_addr);

//Searches all the routes for the child_ip and then updates its parent ip (the next ip on the route list).
//If the child_ip has no parent, adds the new parent
//If the child_ip has an old parent, deletes all above parents and adds the new parent
//If the child_ip has the same parent, does nothing (keeping all above parents).
void dmait_update_route_parents(uip_ip6addr_t *child_ip, uip_ip6addr_t *new_parent_ip);
*/

/**
 * Substring method
 * @param dst
 * @param src
 * @param start
 * @param end
 * @return
 */
char *getSubstring(char *dst, const char *src, size_t start, size_t end);

/**
 * Gets the length of a string up to a max value (to avoid invading other memory spaces)
 * If it didn't find the "NULL character" ('\0') at the end it returns -1
 *
 * @param s the String
 * @param max the max index to search for the NULL caracter
 * @return The length of the string of -1 if it's not NULL terminated at up to max
 */
int my_strlen(const char *s, int max);

/**
 * Replaces the current prefered parent.
 * Identical to
 * void rpl_neighbor_set_preferred_parent(rpl_nbr_t *nbr);
 * from rpl-neighbor.c, but removes the previous parent from neighbours list.
 *
 * \param nbr The new parent
 */
void dmait_replace_parent(rpl_nbr_t *nbr);


/*---------------------------------------------------------------------------*/

/**
 * Based on
 * void rpl_neighbor_print_list(const char *str);
 * from rpl-neighbor.c, but with some tweaks and extra info about the neighbors
 *
 * \param nbr The new parent
 */
void dmait_print_nbr_list();

/*
 * Print an IP address into the buffer "buf".
 * Wrapper function for "uiplib_ipaddr_snprint" from uiplib.
 * @param buf The buffer to be filled by the ip address
 * @param size buffer size
 * @param addr The IP address
 * @return the status returned by uiplib_ipaddr_snprint
 */
//int dmait_ip_to_str(char *buf, size_t size, const uip_ipaddr_t *addr);

/*
 * Converts an String with an IP address to an uip_ip6addr_t
 * Wrapper function for "uiplib_ip6addrconv" from uiplib
 * @param addrstr The IP address in string format
 * @param addr The IP address in uip_ip6addr_t
 * @return the status returned by uiplib_ip6addrconv
 */
//int dmait_str_to_ip(const char *addrstr, uip_ip6addr_t *addr);

/**
 * Prints a host addr in a String
 * @param buf
 * @param bufflen
 * @param hostAddr
 */
void dmait_snprint_host_addr(char *buf, size_t bufflen, dmait_host_addr_t *hostAddr);

/**
 * Prints a host addr in the console
 * @param hostAddr
 */
void dmait_print_host_addr(dmait_host_addr_t *hostAddr);

/**
 * Generates node key based on its hostaddr
 * @param buf
 * @param bufflen
 * @param hostAddr
 */
void dmait_get_node_key_for_host_addr(char *buf, size_t bufflen, dmait_host_addr_t *hostAddr);

/**
 * Prints current node host addr to "buf"
 * @param buf
 * @param bufflen
 * @return
 */
char* dmait_snprint_current_host_addr(char *buf, size_t bufflen);

/**
 * Generates node key for current node
 * At the time only calls "dmait_snprint_current_host_addr()"
 * @param buf
 * @param bufflen
 */
char* dmait_get_current_node_key(char *buf, size_t bufflen);

/**
 * Generates a sha256 hmac digest of str_data using key
 * @param str_data
 * @param key
 * @param outBuff
 * @param outBuffLen
 */
void dmait_generate_hmac(char *str_data, char *key, char *outBuff, size_t outBuffLen);


/**
 * Print the DMAIT-RV Triple Node Block structure
 * @param tnb
 */
void dmait_print_triple_node_block(dmait_triple_node_block_t tnb);


/**
 * Build a dmait_host_addr_t with the last four 16-bit words from uip_ipaddr_t
 * @param hostAddr
 * @param addr
 */
void extractHostAddrFromIpv6(dmait_host_addr_t *hostAddr, const uip_ipaddr_t *addr);


/**
 * Build a linked-scoped unicast uip_ip6addr_t from a dmait_host_addr_t
 * Address starts with "fe80"
 * This kind of IP is not routed. It is used only for direct, node-to-node, packets.
 * @param unicastAddr
 * @param hostAddr
 */
void buildLinkLocalUnicastAddr(uip_ip6addr_t *unicastAddr, dmait_host_addr_t *hostAddr);

/**
 * Build a DS6 Default unicast uip_ip6addr_t from a dmait_host_addr_t
 * Address starts with "fd00"
 * This kind of IP is routed.
 * @param unicastAddr
 * @param hostAddr
 */
void buildDS6DefaultPrefixAddr(uip_ipaddr_t *unicastAddr, dmait_host_addr_t *hostAddr);

/**
 * Build an instance of dmait_host_addr_t from an string with only the last 8 octects (32bit part) of the IPv6.
 * Ex: "0000:0000:0000:0000"
 * Used to parse the DMAITRV blocks
 * @param blockField
 * @param dmaitHostAddr
 */
void dmait_build_host_addr_from_block_field(char *blockField, dmait_host_addr_t *dmaitHostAddr);

/**
 * Checks if the host address is equal to the host part of the IP
 * @param hostAddr
 * @param ipAddr
 * @return true if it is equal, false otherwise
 */
bool dmait_is_host_addr_equal_ip(dmait_host_addr_t *hostAddr, uip_ipaddr_t *ipAddr);
/*

 //Build an instance of dmait_ipaddr_t from an uip_ipaddr_t
//dmait_ipaddr_t is needed to be used in a list
dmait_ipaddr_t * build_dmait_ip(uip_ipaddr_t *addr);

//Extracts an uip_ipaddr_t from a dmait_ipaddr_t
uip_ipaddr_t * extract_ip(dmait_ipaddr_t *dmait_ip);

 */

/**
 * Simple function to send data.
 * Wrapper for simple_udp_sendto(struct simple_udp_connection *c,
                  const void *data, uint16_t datalen,
                  const uip_ipaddr_t *to)
                  from "simple-udp.c"
 * @param udp_conn
 * @param data
 * @param datalen
 * @param dest_ipaddr
 */
/*
void dmait_send_data(struct simple_udp_connection *udp_conn,
              const void *data,
              const size_t datalen,
              const uip_ipaddr_t *dest_ipaddr);
*/
void dmait_send_text_data(struct simple_udp_connection *udp_conn,
                          const char *data,
                          const uip_ipaddr_t *dest_ipaddr);

void dmait_send_data(struct simple_udp_connection *udp_conn,
                     const void *data, uint16_t datalen,
                     const uip_ipaddr_t *dest_ipaddr);
/*
//TEST FUNCTION
void dmait_TEST_build_main_route(dmait_route_t *route);

//Build a string payload for a route
//The payload is structured as values separated by comas in this order:
//- route_id
//- suspicion level
//- route depth (how many IPs are in this route)
//- each IP address in this route
//The "active" flag is not sent, as only the active routes are sent.
void dmait_build_route_payload(char *buf, size_t bufflen, dmait_route_t *route);

//Parse a route paylod into an instance of dmait_route_t
void dmait_parse_route_payload(char *payload, dmait_route_t *route);


*/

/**
 * Calculate hmac from triple node block using node key
 * @param buf
 * @param bufflen
 * @param dmaitTnb
 * @param nodeKey
 */
void dmait_calculate_hmac_from_triple_node_block(char *buf, size_t bufflen, dmait_triple_node_block_t *dmaitTnb, char *nodeKey);

/**
 * Verify if the hmac in Triple Node Block is valid
 * @param dmaitTnb
 * @param nodeKey
 * @return
 */
bool dmait_verify_hmac_in_triple_node_block(dmait_triple_node_block_t *dmaitTnb, char *nodeKey);

/**
 * Add (concatenate) a new triple node block into an existing route validation payload
 * @param buf
 * @param bufflen
 * @param dmaitTnbArray An array of "triple node block"
 */
void dmait_route_validation_add_block(char *buf, size_t bufflen, dmait_triple_node_block_t *dmaitTnbArray);


/**
 * Build a string payload for the DMAIT-RV (route validation) packet
 * @param buf
 * @param bufflen
 * @param dmaitTnbArray
 * @param arraySize
 */
void dmait_route_validation_build_payload(char *buf, size_t bufflen, dmait_triple_node_block_t dmaitTnbArray[], int arraySize);


/**
 * Parse a DMAIT-RV (route validation) block string into a dmait_triple_node_block_t instance.
 * It works like the strtok_r() function
 * @param payload A string in the first call, the next calls pass NULL and use the same "blockState" to continue
 * @param tnb
 * @param blockState It is used to continue the parsing from the last point
 */
bool dmait_parse_next_block_from_rv_payload(char *payload, dmait_triple_node_block_t *tnb, char **blockState);


 /**
 * Parse a block string and extract a dmait_triple_node_block_t from it
  * @param blockString
  * @param tnb
  * @return true if had another block to parse, false if there are no more blocks
  */
bool parseBlock(char *blockString, dmait_triple_node_block_t *tnb);


/**
 * Select a currently available neighbor that is not in the hostAddrArray
 * @param selectedNeighbor
 * @param hostAddrArray
 * @param arraySize
 * @return true if found a neighbor, false otherwise
 */
bool dmait_select_neighbor_not_in_array(dmait_host_addr_t *selectedNeighbor, dmait_host_addr_t hostAddrArray[], int arraySize);

/** @} */

#endif /* DMAIT_LOGIC_H */
