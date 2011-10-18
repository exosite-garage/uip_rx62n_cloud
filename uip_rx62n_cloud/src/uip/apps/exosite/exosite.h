/******************************************************************************
* exosite.h
* 
* Header file for Exosite cloud connectivity functionality
* 
******************************************************************************/
#ifndef __EXOSITE_H__
#define __EXOSITE_H__

/* Since this file will be included by uip.h, we cannot include uip.h
   here. But we might need to include uipopt.h if we need the u8_t and
   u16_t datatypes. */
#include "uipopt.h"
#include "psock.h"

/* External functions */
char Exosite_Init(char * pOS, char * pVer);
char Exosite_Reinit(void);
char Exosite_Check_State(void);
char Exosite_Post(char *data);
char Exosite_Request_Get(char *data);
char Exosite_Get(char *value, char *alias);
void Exosite_Set_CIK(char *cik);
void Exosite_Use_CIK(char *cik);
void Exosite_Get_MAC(unsigned char * mac);

/* Defines */
#define MAC_LOCATION  0x00107FF2  // mac address should be stored in flash at this position
#define EXOSITE_MAX_URLLEN 100

/* Define the uip_tcp_appstate_t datatype. This is the state
   of our application, and the memory required for this state is
   allocated together with each TCP connection. One application state
   for each TCP connection. */
typedef struct exosite_state {
  struct psock p;
  char inputbuffer[255];
  char rx[100];
} uip_tcp_appstate_t;

struct exo_state {
  struct psock socket;
  u16_t port;
  char server_ip[4];
    
  u8_t timer;
  u8_t state;
  u8_t httpflag;

  char cik[40];
  char alias[20];
  char rxdata[40];
  char host[40];
  char file[EXOSITE_MAX_URLLEN];
  u16_t httprequestptr;
  u16_t httprequestleft;
  
  char httpheaderline[200];
  u16_t httpheaderlineptr;

  char mimetype[32];
};

/* Finally we define the application function to be called by uIP. */
void exosite_appcall(void);
#ifndef UIP_APPCALL
#define UIP_APPCALL exosite_appcall
#endif /* UIP_APPCALL */


#endif /* __EXOSITE_H__ */


