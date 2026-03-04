
/**
  ******************************************************************************
  * File Name          : Target/lwipopts.h
  * Description        : This file overrides LwIP stack default configuration
  *                      done in opt.h file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __LWIPOPTS__H__
#define __LWIPOPTS__H__

#include "main.h"

/*-----------------------------------------------------------------------------*/
/* Current version of LwIP supported by CubeMx: 2.1.2 -*/
/*-----------------------------------------------------------------------------*/

/* Within 'USER CODE' section, code will be kept by default at each generation */
/* USER CODE BEGIN 0 */

#if MAIN_DEBUG

#define LWIP_DEBUG					1

#if LWIP_DEBUG

#define LWIP_DBG_MIN_LEVEL			0

#define SSDP_DEBUG                 LWIP_DBG_OFF
#define ALTCP_MBEDTLS_DEBUG	       LWIP_DBG_OFF
#define HTTPD_DEBUG                LWIP_DBG_OFF
#define PPP_DEBUG                  LWIP_DBG_OFF
#define MEM_DEBUG                  LWIP_DBG_OFF
#define MEMP_DEBUG                 LWIP_DBG_OFF
#define PBUF_DEBUG                 LWIP_DBG_OFF
#define API_LIB_DEBUG              LWIP_DBG_OFF
#define API_MSG_DEBUG              LWIP_DBG_OFF
#define TCPIP_DEBUG                LWIP_DBG_OFF
#define NETIF_DEBUG                LWIP_DBG_OFF
#define SOCKETS_DEBUG              LWIP_DBG_OFF
#define DNS_DEBUG                  LWIP_DBG_OFF
#define AUTOIP_DEBUG               LWIP_DBG_OFF
#define DHCP_DEBUG                 LWIP_DBG_OFF
#define IP_DEBUG                   LWIP_DBG_OFF
#define IP_REASS_DEBUG             LWIP_DBG_OFF
#define ICMP_DEBUG                 LWIP_DBG_OFF
#define IGMP_DEBUG                 LWIP_DBG_OFF
#define UDP_DEBUG                  LWIP_DBG_OFF
#define TCP_DEBUG                  LWIP_DBG_OFF
#define TCP_INPUT_DEBUG            LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG           LWIP_DBG_OFF
#define TCP_RTO_DEBUG              LWIP_DBG_OFF
#define TCP_CWND_DEBUG             LWIP_DBG_OFF
#define TCP_WND_DEBUG              LWIP_DBG_OFF
#define TCP_FR_DEBUG               LWIP_DBG_OFF
#define TCP_QLEN_DEBUG             LWIP_DBG_OFF
#define TCP_RST_DEBUG              LWIP_DBG_OFF
#define SNTP_DEBUG                 LWIP_DBG_OFF
#define ETHARP_DEBUG			   LWIP_DBG_OFF

#endif //LWIP_DEBUG

#define LWIP_DBG_TYPES_ON         (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)

#endif //MAIN_DEBUG

#ifndef LWIP_DECLARE_MEMORY_ALIGNED
#define LWIP_DECLARE_MEMORY_ALIGNED(variable_name, size) u8_t variable_name[LWIP_MEM_ALIGN_BUFFER(size)] __attribute__((section(".Lwip_MempSection"))) __attribute__((aligned(32)));
#endif

#define DEV_NAME						"MC33816"
#define DEV_DESC						"Driver"
#define FW_HI							1	//major
#define FW_LOW							0	//minor

//#define HTTPD_SERVER_AGENT 			"lwIP/" LWIP_VERSION_STRING " (http://savannah.nongnu.org/projects/lwip)"
#define HTTPD_SERVER_AGENT_WITH_VER		DEV_NAME "/" LWIP_VERSTR(FW_HI) "." LWIP_VERSTR(FW_LOW) " (" DEV_DESC " https://www.codelec.cz)"
#define HTTPD_SERVER_AGENT 				DEV_NAME " (" DEV_DESC " https://www.codelec.cz)"

#define LWIP_SO_RCVTIMEO				1
#define LWIP_HTTPD_POST_MANUAL_WND		1
#define LWIP_ALTCP_TLS_MBEDTLS 			MAIN_MBEDTLS
#define SO_REUSE						MAIN_MBEDTLS
#define ALTCP_MBEDTLS_RNG_FN 			mbedtls_entropy_func

/*
 * https://savannah.nongnu.org/patch/?9576
 */
#define LWIP_HTTPD_SUPPORT_AUTH_COOKIE	1
#if LWIP_HTTPD_SUPPORT_AUTH_COOKIE
#define LWIP_HTTPD_AUTH_COOKIE_NAME		"Auth"
#define LWIP_HTTPD_AUTH_SIZE			9 //include \0
#define LWIP_HTTPD_AUTH_PAGE 			"/dh_logout.shtml"
#endif

#define LWIP_HTTPD_WEBSOCKET 			0

//REST
#define LWIP_REST_SUPPORT_AUTH			1
#if LWIP_REST_SUPPORT_AUTH
#define LWIP_REST_AUTH_SIZE				16 //include \0
#define LWIP_REST_AUTH_PAGE 			"/401.html"
#endif

#define LWIP_REST_WEBSOCKET 			0

//SSDP
#define LWIP_SSDP						1 //LWIP_NUM_SYS_TIMEOUT_INTERNAL add timer +1
#define LWIP_TCPIP_TIMEOUT				1
#define LWIP_IGMP						1

//DHCP
#define LWIP_NETIF_HOSTNAME  			1

//AUTOIP
//AUTOIP_NET         					0xA9FE0000		169.254.x.x
#define LWIP_AUTOIP						1
#define LWIP_DHCP_AUTOIP_COOP			1 	//defines auto ip to work with dhcp
#define LWIP_DHCP_AUTOIP_COOP_TRIES		4 	// controls how many dhcp discovers to try before timing out to an auto ip address
//#define LWIP_AUTOIP_CREATE_SEED_ADDR     	//lets you control the first auto ip address the device will try

//sntp
#define LWIP_SNTP						1 //LWIP_NUM_SYS_TIMEOUT_INTERNAL add timer +1
#define LWIP_DHCP_GET_NTP_SRV			0
#define SNTP_SERVER_DNS					1
#define SNTP_UPDATE_DELAY           	3600000 //ms; 60s = 60000
//#define SNTP_SET_SYSTEM_TIME(sec)   	sntp_set_system_time(sec)

//mdns
#define LWIP_MDNS_RESPONDER             0
#define LWIP_NUM_NETIF_CLIENT_DATA		0

/* USER CODE END 0 */

#ifdef __cplusplus
 extern "C" {
#endif

/* STM32CubeMX Specific Parameters (not defined in opt.h) ---------------------*/
/* Parameters set in STM32CubeMX LwIP Configuration GUI -*/
/*----- WITH_RTOS enabled (Since FREERTOS is set) -----*/
#define WITH_RTOS 					1
/*----- WITH_MBEDTLS enabled (Since MBEDTLS and FREERTOS are set) -----*/
#define WITH_MBEDTLS 				MAIN_MBEDTLS
/*----- CHECKSUM_BY_HARDWARE enabled -----*/
#define CHECKSUM_BY_HARDWARE 		1
/*-----------------------------------------------------------------------------*/

/* LwIP Stack Parameters (modified compared to initialization value in opt.h) -*/
/* Parameters set in STM32CubeMX LwIP Configuration GUI -*/
/*----- Value in opt.h for LWIP_DHCP: 0 -----*/
#define LWIP_DHCP 					1
/*----- Value in opt.h for LWIP_DNS: 0 -----*/
#define LWIP_DNS 					1
/*----- Default Value for MEMP_NUM_TCP_PCB: 5 ---*/
#define MEMP_NUM_TCP_PCB 			24
/*----- Default Value for MEMP_NUM_UDP_PCB: 4 ---*/
#define MEMP_NUM_UDP_PCB 			6
/*----- Value in opt.h for MEM_ALIGNMENT: 1 -----*/
#define MEM_ALIGNMENT 				32
 /*----- Default Value for H7 devices: 0x30004000 -----*/
//#define LWIP_RAM_HEAP_POINTER 	0x30008000
//#define MEMP_SEPARATE_POOLS 		1
 /*----- Value supported for H7 devices: 1 -----*/
 #define LWIP_SUPPORT_CUSTOM_PBUF 	1
/*----- Default Value for MEM_SIZE: 1600 ---*/
#define MEM_SIZE 					(200 * 1024)
/*----- Default Value for MEMP_NUM_PBUF: 16 ---*/
#define MEMP_NUM_PBUF 				32
/*----- Default Value for MEMP_NUM_NETBUF: 2 ---*/
#define MEMP_NUM_NETBUF 			8
/*----- Default Value for MEMP_NUM_NETCONN: 4 ---*/
#define MEMP_NUM_NETCONN 			8
/*----- Default Value for PBUF_POOL_BUFSIZE: 592 ---*/
#define PBUF_POOL_BUFSIZE 			1520
 /*----- Default Value for PBUF_POOL_SIZE: 16 ---*/
#define PBUF_POOL_SIZE              24
/*----- Value in opt.h for LWIP_ETHERNET: LWIP_ARP || PPPOE_SUPPORT -*/
#define LWIP_ETHERNET 				1
/*----- Value in opt.h for LWIP_DNS_SECURE: (LWIP_DNS_SECURE_RAND_XID | LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING | LWIP_DNS_SECURE_RAND_SRC_PORT) -*/
#define LWIP_DNS_SECURE 			7
/*----- Default Value for TCP_MSS: 536 ---*/
#define TCP_MSS 					1460
 /*----- Default Value for TCP_WND: (4 * TCP_MSS) ---*/
#define TCP_WND 					(14 * TCP_MSS)
/*----- Default Value for MEMP_NUM_TCPIP_MSG_INPKT: 8 ---
 * adjust according TCP_WND, if lower TCP Retransmission occur
 */
#define MEMP_NUM_TCPIP_MSG_INPKT	20
 /*----- Value in opt.h for TCP_SND_BUF: (2 * TCP_MSS) ---*/
#define TCP_SND_BUF 				(4 * TCP_MSS)
/*----- Value in opt.h for TCP_SND_QUEUELEN: (4*TCP_SND_BUF + (TCP_MSS - 1))/TCP_MSS -----*/
#define TCP_SND_QUEUELEN 			(4*TCP_SND_BUF + (TCP_MSS - 1))/TCP_MSS
/*----- Value in opt.h for TCP_SNDQUEUELOWAT: LWIP_MAX(TCP_SND_QUEUELEN)/2, 5) -*/
#define TCP_SNDQUEUELOWAT 			5
/*----- Default Value for LWIP_ALTCP: 0 ---*/
#define LWIP_ALTCP MAIN_MBEDTLS
/*----- Default Value for LWIP_ALTCP_TLS: 0 ---*/
#define LWIP_ALTCP_TLS MAIN_MBEDTLS
/*----- Value in opt.h for LWIP_NETIF_LINK_CALLBACK: 0 -----*/
#define LWIP_NETIF_LINK_CALLBACK 	1
/*----- Default Value for TCPIP_THREAD_NAME: "tcpip_thread" ---*/
#define TCPIP_THREAD_NAME 			"tcpipTask"
/*----- Value in opt.h for TCPIP_THREAD_STACKSIZE: 0 -----*/
#define TCPIP_THREAD_STACKSIZE 		(6*1024)
/*----- Value in opt.h for TCPIP_THREAD_PRIO: 1 -----*/
#define TCPIP_THREAD_PRIO 			24
/*----- Value in opt.h for TCPIP_MBOX_SIZE: 0 -----*/
#define TCPIP_MBOX_SIZE 			MEMP_NUM_PBUF
/*----- Value in opt.h for SLIPIF_THREAD_STACKSIZE: 0 -----*/
#define SLIPIF_THREAD_STACKSIZE 	1024
/*----- Value in opt.h for SLIPIF_THREAD_PRIO: 1 -----*/
#define SLIPIF_THREAD_PRIO 			3
/*----- Value in opt.h for DEFAULT_THREAD_STACKSIZE: 0 -----*/
#define DEFAULT_THREAD_STACKSIZE 	1024
/*----- Value in opt.h for DEFAULT_THREAD_PRIO: 1 -----*/
#define DEFAULT_THREAD_PRIO 		3
/*----- Value in opt.h for DEFAULT_UDP_RECVMBOX_SIZE: 0 -----*/
#define DEFAULT_UDP_RECVMBOX_SIZE 	8
/*----- Value in opt.h for DEFAULT_TCP_RECVMBOX_SIZE: 0 -----*/
#define DEFAULT_TCP_RECVMBOX_SIZE 	8
/*----- Value in opt.h for DEFAULT_ACCEPTMBOX_SIZE: 0 -----*/
#define DEFAULT_ACCEPTMBOX_SIZE 	8
/*----- Value in opt.h for RECV_BUFSIZE_DEFAULT: INT_MAX -----*/
#define RECV_BUFSIZE_DEFAULT 		2000000000
/*----- Value in opt.h for LWIP_USE_EXTERNAL_MBEDTLS: 0 -----*/
#define LWIP_USE_EXTERNAL_MBEDTLS MAIN_MBEDTLS
/*----- Default Value for LWIP_HTTPD: 0 ---*/
#define LWIP_HTTPD 					1
/*----- Default Value for LWIP_HTTPD_CGI: 0 ---*/
#define LWIP_HTTPD_CGI 				0
/*----- Default Value for LWIP_HTTPD_CGI_SSI: 0 ---*/
#define LWIP_HTTPD_CGI_SSI			0
/*----- Default Value for LWIP_HTTPD_SSI: 0 ---*/
#define LWIP_HTTPD_SSI 				1
#define LWIP_REST_SSI				0
/*----- Default Value for LWIP_HTTPD_SSI_RAW: 0 ---*/
#define LWIP_HTTPD_SSI_RAW 			1
#define LWIP_REST_SSI_RAW 			0
/*----- Default Value for LWIP_HTTPD_SSI_MULTIPART: 0 ---*/
#define LWIP_HTTPD_SSI_MULTIPART 	1
#define LWIP_REST_SSI_MULTIPART 	0
/*----- Default Value for LWIP_HTTPD_SUPPORT_POST: 0 ---*/
#define LWIP_HTTPD_SUPPORT_POST 	1
/*----- Default Value for LWIP_HTTPD_DYNAMIC_HEADERS: 0 ---*/
#define LWIP_HTTPD_DYNAMIC_HEADERS 	1
/*----- Default Value for HTTPD_ENABLE_HTTPS: 0 ---*/
#define HTTPD_ENABLE_HTTPS 			MAIN_MBEDTLS
#define REST_ENABLE_HTTPS 			MAIN_MBEDTLS
/*----- Default Value for LWIP_HTTPD_SSI_INCLUDE_TAG: 1 ---*/
#define LWIP_HTTPD_SSI_INCLUDE_TAG 	0
/*----- Default Value for LWIP_HTTPD_FILE_STATE: 0 ---*/
#define LWIP_HTTPD_FILE_STATE 		0
/*----- Value in opt.h for HTTPD_USE_CUSTOM_FSDATA: 0 -----*/
#define HTTPD_USE_CUSTOM_FSDATA 	1
 /*----- Value in opt.h for HTTPD_USE_MEM_POOL: 0 -----*/
#define HTTPD_USE_MEM_POOL 			1
/*----- Value in opt.h for LWIP_STATS: 1 -----*/
#if MAIN_DEBUG
#define LWIP_STATS 					1
#else
#define LWIP_STATS 					0
#endif
/*----- Value in opt.h for CHECKSUM_GEN_IP: 1 -----*/
#define CHECKSUM_GEN_IP 			0
/*----- Value in opt.h for CHECKSUM_GEN_UDP: 1 -----*/
#define CHECKSUM_GEN_UDP 			0
/*----- Value in opt.h for CHECKSUM_GEN_TCP: 1 -----*/
#define CHECKSUM_GEN_TCP 			0
/*----- Value in opt.h for CHECKSUM_GEN_ICMP: 1 -----*/
#define CHECKSUM_GEN_ICMP 			0
/*----- Value in opt.h for CHECKSUM_GEN_ICMP6: 1 -----*/
#define CHECKSUM_GEN_ICMP6 			0
/*----- Value in opt.h for CHECKSUM_CHECK_IP: 1 -----*/
#define CHECKSUM_CHECK_IP 			0
/*----- Value in opt.h for CHECKSUM_CHECK_UDP: 1 -----*/
#define CHECKSUM_CHECK_UDP 			0
/*----- Value in opt.h for CHECKSUM_CHECK_TCP: 1 -----*/
#define CHECKSUM_CHECK_TCP 			0
/*----- Value in opt.h for CHECKSUM_CHECK_ICMP: 1 -----*/
#define CHECKSUM_CHECK_ICMP 		0
/*----- Value in opt.h for CHECKSUM_CHECK_ICMP6: 1 -----*/
#define CHECKSUM_CHECK_ICMP6 		0
/*-----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /*__LWIPOPTS__H__ */

/************************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
