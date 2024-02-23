//
// Created by daniel on 04/12/22.
//

#include "dmait-blacklist.h"



//MEMB dynamic memory allocation definition for dmait_route_node_t structure
MEMB(dmait_route_node_memb, dmait_route_node_t, 16); //16 route nodes max

/* Only instance of node blacklist list per node */
LIST(dmaitNodeBlacklist); //for dmait_route_node_t
