
#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

/*---------------------------------------------------------------------------*/

/* Free some code and RAM space */
#undef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU 12
#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU 12
//#define SICSLOWPAN_CONF_COMPRESSION SICSLOWPAN_COMPRESSION_IPV6


#define COOJA_RADIO_CONF_BUFSIZE 1024
#define PACKETBUF_CONF_SIZE 1027

#define CONF_SFA 1
#define CONF_SHA 1

//Memory optimization
#define PROCESS_CONF_NO_PROCESS_NAMES 1

/*---------------------------------------------------------------------------*/
#endif /* PROJECT_CONF_H_ */
/*---------------------------------------------------------------------------*/


