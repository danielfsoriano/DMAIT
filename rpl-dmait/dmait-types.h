/*
 * DMAIT - Deteccao e Mitigacao de Ataques de Interrupcao de Trafego Extension
 *
 * This code is based on the rpl-udp example from Contiki operating system.
 *
 * Created by Daniel Francis Soriano
 *
 */

#ifndef DMAIT_TYPES_H
#define DMAIT_TYPES_H

#include "lib/list.h"
#include "net/ipv6/uiplib.h"
#include "net/routing/rpl-lite/rpl-types.h"


/** \brief Invalid Hash Increment */
//#define MRV_INV_HASH_INC 8
/** \brief Invalid Signature Increment */
//#define MRV_INV_SIGN_INC 16

/** \brief MAX ROUTE DEPTH */
//#define MRV_MAX_ROUTE_DEPTH  5


/** DMAIT payload header names */
#define DMAIT_ROUTE_VALIDATION_HEADER "RV"
#define DMAIT_ROUTE_VALIDATION_ACK_HEADER "RK"
#define DMAIT_ALTERNATE_ROUTE_HEADER "AR"
#define DMAIT_ALTERNATE_ROUTE_AVOID_HEADER "AVOID"
#define DMAIT_ALTERNATE_ROUTE_ROUTE_HEADER "ROUTE"
#define DMAIT_ALTERNATE_ROUTE_ACK_HEADER "AK"
#define DMAIT_FAULT_LOCATION_PROBE_HEADER "FL"
#define DMAIT_FAULT_LOCATION_ACK_HEADER "FK"

#define DMAIT_DATA_MESSAGE_HEADER "DM"
#define DMAIT_DATA_MESSAGE_ACK_HEADER "DK"

#define DMAIT_RV_BLOCK_LENGTH 70
#define DMAIT_HOST_ADDR_LENGTH 19

#define DMAIT_HMAC_LENGTH 10
#define DMAIT_SEQUENCE_NUMBER_LENGTH 4
#define DMAIT_DATALEN_LENGHT 3 //up to 999

/** DMAIT field separators */
#define DMAIT_HEADER_SEPARATOR "-"
#define DMAIT_FIELD_SEPARATOR ","
#define DMAIT_BLOCK_SEPARATOR ";"


#define DMAIT_MAX_DATA_LENGHT 999
#define DMAIT_MAX_SEQUENCE_NUMBER 9999




/*---------------------------------------------------------------------------*/

/** \brief DMAIT route structure */
/*
struct dmait_route {
    struct dmait_route *next;
    uint8_t route_id;
    bool active;
    uint8_t suspicion_level;
    //uip_ipaddr_t ip_list[MRV_MAX_ROUTE_DEPTH];
    //uip_ipaddr_t ip_list[1];
    LIST_STRUCT(ip_list);
};
typedef struct dmait_route dmait_route_t;
*/
/*---------------------------------------------------------------------------*/

/** \brief DMAIT IP Address item */
/*
struct dmait_ipaddr {
    struct dmait_ipaddr *next;
    uip_ip6addr_t ip_addr;
};
typedef struct dmait_ipaddr dmait_ipaddr_t;
*/
/*---------------------------------------------------------------------------*/

/** only the last four blocks (uint 16) of IPv6 */
typedef union dmait_host_addr_t {
    uint8_t  u8[8];                      /* Initializer, must come first. */
    uint16_t u16[4];
} dmait_host_addr_t;

//----------- ROUTE VALIDATION

/** \brief DMAIT-RV (Route Validation) triple node block structure */
struct dmait_triple_node_block {
    dmait_host_addr_t prev_node_addr;
    dmait_host_addr_t node_addr;
    dmait_host_addr_t next_node_addr;
    char hmac_digest[11];
};
typedef struct dmait_triple_node_block dmait_triple_node_block_t;

/** \brief DMAIT-RV route validation structure list - used in server to track the validation of continuity and hmacs */
struct dmait_rv_route_validation {
    struct dmait_rv_route_validation *next;
    dmait_triple_node_block_t tnb;
    bool hmacValidated;
    bool previousValidated;
};
typedef struct dmait_rv_route_validation dmait_rv_route_validation_t;

//----------- ROUTE VALIDATION

//----------- FAULT LOCATION

/** \brief DMAIT-FL (Fault Location) node structure list - used in server to track if each node in a node route has received an FL-ACK*/
struct dmait_fl_acked_node {
    struct dmait_fl_acked_node *next;
    dmait_host_addr_t node_addr;
    bool acked;
};
typedef struct dmait_fl_acked_node dmait_fl_acked_node_t;

/** \brief DMAIT-FL (Fault Location) list of dmait_fl_acked_node_t indexed by child node - used in server to hold all of it's children nodes "dmait_fl_acked_node_t", indexed by (child) node_addr */
struct dmait_fl_acked_node_route {
    struct dmait_fl_acked_node_route *next;
    dmait_host_addr_t node_addr;
    LIST_STRUCT(acked_node_list); //dmait_fl_node_t
};
typedef struct dmait_fl_acked_node_route dmait_fl_acked_node_route_t;

//----------- FAULT LOCATION

/** \brief DMAIT route structure - used in clients to hold it's route*/
struct dmait_route_node {
    struct dmait_route_node *next;
    dmait_host_addr_t node_addr;
};
typedef struct dmait_route_node dmait_route_node_t;

//------------ AVOID ROUTE

/** \brief DMAIT suspect node structure - used in gateway to keep suspects scores*/
struct dmait_suspect_node {
    struct dmait_suspect_node *next;
    dmait_host_addr_t node_addr;
    uint8_t negative_score;
    uint8_t positive_score;
};
typedef struct dmait_suspect_node dmait_suspect_node_t;
/*
struct dmait_unacked_message {
    struct dmait_unacked_message *next;
    uint8_t seq_number;
    unsigned long seconds;
};
*/
//------------ AVOID ROUTE


/*---------------------------------------------------------------------------*/



//Constructs a DMAIT host address from four 16-bit words.
#define dmait_hostAddr(hostAddr, addr0,addr1,addr2,addr3) do { \
    (hostAddr)->u16[0] = UIP_HTONS(addr0);                                      \
    (hostAddr)->u16[1] = UIP_HTONS(addr1);                                      \
    (hostAddr)->u16[2] = UIP_HTONS(addr2);                                      \
    (hostAddr)->u16[3] = UIP_HTONS(addr3);                                      \
  } while(0)



/** @} */

#endif /* DMAIT_TYPES_H */
