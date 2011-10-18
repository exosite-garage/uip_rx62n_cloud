/******************************************************************************
* exosite.c
* 
* Main helper functions for interacting with the cloud via Exosite.
* 
******************************************************************************/

/*
 * We define the application state (struct exosite_state) in the
 * exosite.h file, so we need to include it here. We also include
 * uip.h (since this cannot be included in exosite.h) and
 * <string.h>, since we use the memcpy() function in the code.
 */
#include "exosite.h"
#include "uip.h"
#include <string.h>
#include <stdio.h>
#include "exosite_strings.h"
#include "lcd.h"
#include "YRDKRX62N.h"
#include "rdk_meta.h"

/* Local defines */
#define OSN_SIZE    30
#define OSV_SIZE    16

#define CIK_LENGTH  40
#define PID_LENGTH  9
#define PID         "YRDKRX62N"

#define EXO_TIMEOUT 100

#define EXO_STATE_GET        0
#define EXO_STATE_POST       1
#define EXO_STATE_HEADERS    2
#define EXO_STATE_DATA       3
#define EXO_STATE_CLOSE      4
#define EXO_STATE_NEEDS_META 5

#define HTTPFLAG_NONE   0
#define HTTPFLAG_OK     1
#define HTTPFLAG_MOVED  2
#define HTTPFLAG_ERROR  3

#define ISO_nl       0x0a
#define ISO_cr       0x0d
#define ISO_space    0x20

/* Internal functions */
static void senddata(void);
static void acked(void);
static void newdata(void);
static u16_t parse_statusline(u16_t len);
static char casecmp(char *str1, const char *str2, char len);
static u16_t parse_headers(u16_t len);
char setup_server_connection(void);
unsigned char is_valid_cik(char *cik);
static void init_flash_content(void);
static void activate_device(void);

/* Internal Globals */
static struct exo_state s;
static unsigned char    OSN_LENGTH;
static char             OSN[OSN_SIZE]; // buffer size is fixed, length is arbitrary
static unsigned char    OSV_LENGTH;
static char             OSV[OSV_SIZE]; // buffer size is fixed, length is arbitrary

/******************************************************************************
* char Exosite_Reinit(void)
* 
* The initialization function. We must explicitly call this function
* from the system initialization code, some time after uip_init() is
* called.
* 
******************************************************************************/
char Exosite_Reinit(void)
{
//    update_m2ip(); // see if we need to update our server IP address
  activate_device();  // see if we need to get a new CIK value

  if (!is_valid_cik(s.cik)) return 0;
  
  s.state = EXO_STATE_CLOSE;   
  return 1;
}

/******************************************************************************
* char Exosite_Init(char * pOS, char * pVer)
* 
* The initialization function. We must explicitly call this function
* from the system initialization code, some time after uip_init() is
* called.
* 
******************************************************************************/
char Exosite_Init(char * pOS, char * pVer)
{ 
  s.state = EXO_STATE_NEEDS_META;   
  rdk_meta_init();
  init_flash_content();
  
  strncpy(OSN, pOS, OSN_SIZE);
  OSN[OSN_SIZE - 1] = 0;
  OSN_LENGTH = strlen(OSN);

  strncpy(OSV, pVer, OSV_SIZE);
  OSV[OSV_SIZE - 1] = 0;
  OSV_LENGTH = strlen(OSV);
     
  return Exosite_Reinit();
}

/******************************************************************************
* void Exosite_Use_CIK(char *cik)
* 
* Temporarily set a CIK for use (will not program to flash)
* 
******************************************************************************/
void Exosite_Use_CIK(char *cik)
{
  if (is_valid_cik(cik))
  {
    strncpy(s.cik, cik, CIK_LENGTH);
  }
  else
  {
    rdk_meta *meta_info = (rdk_meta *)RDK_META_LOCATION;
    strncpy(s.cik, meta_info->cik, CIK_LENGTH);
  }
  Exosite_Reinit();  
}

/******************************************************************************
* void Exosite_Set_CIK(char *cik)
* 
* Permanently set a new CIK for the system.  Accepts any CIK value regardless
* of validity.
* 
******************************************************************************/
void Exosite_Set_CIK(char *cik)
{
  rdk_meta *meta_info = (rdk_meta *)RDK_META_LOCATION;
  rdk_meta_write((unsigned char *)cik, CIK_LENGTH, (unsigned char *)meta_info->cik);

  // Get local copy 
  strncpy(s.cik, cik, CIK_LENGTH);
  Exosite_Reinit();    
}

/******************************************************************************
* void Exosite_Get_MAC(unsigned char *mac)
* 
* Fills a the provide buffer, mac, with 6 byte MAC address from data flash
* 
******************************************************************************/
void Exosite_Get_MAC(unsigned char * mac){
  rdk_meta_init(); //enable flash to read from our meta structure
  memcpy(mac, (char *)MAC_LOCATION, 6);
}

/******************************************************************************
* static void init_flash_content(void)
* 
* The initialization function. We must explicitly call this function
* from the system initialization code, some time after uip_init() is
* called.
* 
******************************************************************************/
static void init_flash_content(void)
{
  rdk_meta *meta_info;

  meta_info = (rdk_meta *)RDK_META_LOCATION;

  //check our meta structure. if it is hosed, we initialize
  //with defaults and continue
  if (strcmp(meta_info->mark,EXOMARK)) {
    rdk_meta_defaults();
    memset(s.cik, 0x00, CIK_LENGTH);
  } else strncpy(s.cik, meta_info->cik, CIK_LENGTH);

  // Convert stored ip and port to something usable
  memcpy(s.server_ip,meta_info->server,4);  //first 4 bytes are server IP
  s.port = (meta_info->server[4] << 8) | meta_info->server[5]; //last 2 bytes are port number
}

/******************************************************************************
* char Exosite_Check_State(void) 
* 
* Returns 0 if there is a problem, 1 if not.
* 
******************************************************************************/
char Exosite_Check_State(void) 
{
  if (EXO_STATE_NEEDS_META == s.state) return 0;
  return 1;
}

/******************************************************************************
* void Exosite_Post(char * data)
* 
* Sends a HTTP post request to the Exosite server to send the value to the alias
* resource - both of which are contained in the string data buffer.
* 
******************************************************************************/
char Exosite_Post(char *data)
{
  if (EXO_STATE_NEEDS_META == s.state) return 0;

  strncpy(s.file, data, sizeof(s.file));
  s.state = EXO_STATE_POST;
  return setup_server_connection();   	
}

/******************************************************************************
* void Exosite_Request_Get(char * alias)
* 
* Sends a HTTP get request to the Exosite server to fetch the current value
* of alias name contained in buffer data.  We only support fetching a single
* alias value currently, although this could be extended to retreive multiple
* values.
* 
******************************************************************************/  
char Exosite_Request_Get(char *alias)
{
  if (EXO_STATE_NEEDS_META == s.state) return 0;  
  strncpy(s.alias, alias, sizeof(s.alias));
  s.state = EXO_STATE_GET;
  return setup_server_connection(); 
}

/******************************************************************************
* void Exosite_Get(char * value)
* 
* Fills the provided buffer "value" with alias data returned from the last
* exosite_request_get() call.  If the requested alias is not available, 
* function will return 0. 
* 
******************************************************************************/
char Exosite_Get(char * value, char * alias)
{
  char read_name_OK = 0;
  char * temp = s.rxdata;
  char found_name[20];
  int i, len;
  int j = 0;
  
  if (EXO_STATE_NEEDS_META == s.state) return 0;  
  if (0 == *s.rxdata) return 0; // we haven't received the data yet
  len = sizeof(s.rxdata);
  // Get the data_name
  for(i=0; i<len; i++) {
    if(0 == read_name_OK) {
      if('=' == temp[i]) {
        found_name[j] = 0; // null terminate for strcmp
        if (strcmp(found_name,alias)) {
          j = 0;
          //skip to the next element
          if (i < len-2) while(temp[i++] != '&');
          found_name[j++] = temp[i];
        } else read_name_OK = 1; 
      } else {
        found_name[j++] = temp[i];
      }
    } else {
      if (('&' == temp[i]) || (0 == temp[i])) break;
      *value++ = temp[i];
    }
  }
  *value = 0; //null terminate
  return 1;
}

/******************************************************************************
* void exosite_appcall(void)
*
* This function is uIP's application function.  It is called whenever a uIP 
* event occurs (e.g. when a new connection is established, new data arrives, 
* sent data is acknowledged, data needs to be retransmitted, etc.).
* 
******************************************************************************/ 
void exosite_appcall(void)
{
  if(uip_connected()) {
    s.timer = 0;
    senddata();
    return;
  }

  if(s.state == EXO_STATE_CLOSE) {
    uip_abort();
    return;
  }
  
  if(uip_aborted()) {
  }
  if(uip_timedout()) {
  }

  if(uip_acked()) {
    s.timer = 0;
    acked();
  }

  if(uip_newdata()) {
    s.timer = 0;
    newdata();
  }
 
  if(uip_rexmit() || uip_newdata() || uip_acked()) {
    senddata();
  } else if(uip_poll()) {
    ++s.timer;
    if(s.timer == EXO_TIMEOUT) {
      uip_abort();
      return;
    }
  }
#if 0  
  if(uip_closed()) {
    if(s.httpflag != HTTPFLAG_MOVED) {
      memset(s.rxdata,0,sizeof(s.rxdata));
    } else {
      if(resolv_lookup(s.host) == NULL) {
	      resolv_query(s.host);
      }
    }
  }
#endif

}

/******************************************************************************
* static void senddata(void)
*
* This is the main send function.  We split it into GET and POST.
*
* Per Exosite documentation, the format of a GET request should look like:
*
*    GET /api:v1/stack/alias?1&temp&state HTTP/1.1
*    Host: m2.exosite.com
*    X-Exosite-CIK: 576e95bf2e7026911f53356d38a9f2afd2b82f83
*    Accept: application/x-www-form-urlencoded; charset=utf-8 
*
* Per Exosite documentation, the format of a POST request should look like:
*
*    POST /api:v1/stack/alias HTTP/1.1
*    Host: m2.exosite.com
*    X-Exosite-CIK: 576e95bf2e7026911f53356d38a9f2afd2b82f83
*    Content-Type: application/x-www-form-urlencoded; charset=utf-8
*    Content-Length: 31
*    1=453&temp=100402&state=running
*
******************************************************************************/
static void senddata(void)
{
  u16_t len;
  char *httprequest;
  char temp[100];	
        
  if(s.httprequestleft > 0) {
    httprequest = (char *)uip_appdata;
    if(s.state == EXO_STATE_POST) { 
      strcpy(httprequest, POST_HEADER);  
      strcat(httprequest, M2_HOST);
      strcat(httprequest, "X-Exosite-CIK: ");
      strncat(httprequest, s.cik, CIK_LENGTH);
      strcat(httprequest, "\r\nContent-Type: application/x-www-form-urlencoded; charset=utf-8\r\n");       
      sprintf(temp,"%s%d%s%s%s%s",CONTENT_LENGTH,strlen(s.file),NEXT_LINE,NEXT_LINE,s.file,NEXT_LINE);
      strcat(httprequest, temp); 
    } else if (s.state == EXO_STATE_GET) {
      strcpy(httprequest, GET_HEADER);
      sprintf(temp,"%s%s",s.alias,HTTP11);      
      strcat(httprequest, temp);
      strcat(httprequest, M2_HOST);
      strcat(httprequest, "X-Exosite-CIK: ");
      strncat(httprequest, s.cik, CIK_LENGTH);      
      strcat(httprequest, "\r\nAccept: application/x-www-form-urlencoded; charset=utf-8\r\n\r\n\r\n");           
    }
    s.httprequestleft = strlen(httprequest);    
             
    len = s.httprequestleft > uip_mss()?
      uip_mss():
      s.httprequestleft;
    uip_send(&(httprequest[s.httprequestptr]), len);
  }
}

/******************************************************************************
* static void newdata(void)
* 
* Called when uip has new data to be processed.  Stores output in our
* static structure's rxdata element.
* 
******************************************************************************/
static void newdata(void)
{
  volatile u16_t len;
    
  len = uip_datalen();

  if((EXO_STATE_POST == s.state) || (EXO_STATE_GET == s.state)) {
    len = parse_statusline(len);
  }
  
  if(s.state == EXO_STATE_HEADERS && len > 0) {
    len = parse_headers(len); 
  }

  if(len > 0 && s.state == EXO_STATE_DATA) 
  {
    char * temp = s.rxdata;
    if (len > sizeof(s.rxdata)) len = sizeof(s.rxdata);
    strncpy(s.rxdata,(char *)uip_appdata,len);
    temp += len;
    *temp = 0; //null terminate
  }
}

/******************************************************************************
* static void acked(void)
* 
* Check to see if server has acked but we have more to send
* 
******************************************************************************/
static void acked(void)
{
  u16_t len;
  
  if(s.httprequestleft > 0) {
    len = s.httprequestleft > uip_mss()?
      uip_mss():
      s.httprequestleft;
    s.httprequestleft -= len;
    s.httprequestptr += len;
  }
}

/******************************************************************************
* void setup_server_connection(void)
* 
* Open a TCP socket to the Exosite server
* 
******************************************************************************/
char setup_server_connection(void)
{
  uip_ipaddr_t ipaddr;
  struct uip_conn *conn;

  uip_ipaddr(ipaddr, s.server_ip[0], s.server_ip[1], s.server_ip[2], s.server_ip[3]);  // m2.exosite.com 
  conn = uip_connect(&ipaddr, htons(s.port));  
  if (conn == NULL) {
    volatile unsigned char i;
    i = 0;
    return 0;
  }
  
  *s.rxdata = 0;        //clear the first byte of our rx bufffer
  s.httprequestleft = 1;//set to a non-zero value 
  s.httprequestptr = 0;
  s.httpheaderlineptr = 0;
    
  return 1;
}

/******************************************************************************
* static u16_t parse_statusline(u16_t len)
* 
* Parses the status line of a HTTP response
* 
******************************************************************************/
static u16_t parse_statusline(u16_t len)
{
  char *cptr;
  while(len > 0 && s.httpheaderlineptr < sizeof(s.httpheaderline)) {
    s.httpheaderline[s.httpheaderlineptr] = *(char *)uip_appdata;
    ++((char *)uip_appdata);
    --len;
    
    if(s.httpheaderline[s.httpheaderlineptr] == ISO_nl) {
      if((strncmp(s.httpheaderline, http_10, sizeof(http_10) - 1) == 0) ||
        (strncmp(s.httpheaderline, http_11, sizeof(http_11) - 1) == 0)) 
      {
        cptr = &(s.httpheaderline[9]);
        s.httpflag = HTTPFLAG_NONE;
        if(strncmp(cptr, http_200, sizeof(http_200) - 1) == 0) {
          /* 200 OK */
          s.httpflag = HTTPFLAG_OK;
        } else if(strncmp(cptr, http_301, sizeof(http_301) - 1) == 0 ||
          strncmp(cptr, http_302, sizeof(http_302) - 1) == 0) {
          /* 301 Moved permanently or 302 Found. Location: header line will contain thw new location. */
          s.httpflag = HTTPFLAG_MOVED;
        } else if(strncmp(cptr, "401 Unauthorized", sizeof("401 Unauthorized") - 1) == 0 ) {
          s.state = EXO_STATE_NEEDS_META;
          uip_abort();
          return 0;          
        } else {
          s.httpheaderline[s.httpheaderlineptr - 1] = 0;
        }
      } else {
        uip_abort();
//        webclient_aborted();
        return 0;
      }
      
      /* We're done parsing the status line, so we reset the pointer and start parsing the HTTP headers.*/
      s.httpheaderlineptr = 0;
      s.state = EXO_STATE_HEADERS;
      break;
    } else {
      ++s.httpheaderlineptr;
    }
  }
  return len;
}


/******************************************************************************
* static char casecmp(char *str1, const char *str2, char len)
* 
* Case insensitive comparison helper function
* 
******************************************************************************/
static char casecmp(char *str1, const char *str2, char len)
{
  static char c;
  
  while(len > 0) {
    c = *str1;
    /* Force lower-case characters. */
    if(c & 0x40) {
      c |= 0x20;
    }
    if(*str2 != c) {
      return 1;
    }
    ++str1;
    ++str2;
    --len;
  }
  return 0;
}


/******************************************************************************
* static u16_t parse_headers(u16_t len)
* 
* Parse HTTP headers in HTTP response
* 
******************************************************************************/
static u16_t parse_headers(u16_t len)
{
  char *cptr;
  static unsigned char i;
  
  while(len > 0 && s.httpheaderlineptr < sizeof(s.httpheaderline)) {
    s.httpheaderline[s.httpheaderlineptr] = *(char *)uip_appdata;
    ++((char *)uip_appdata);
    --len;
    if(s.httpheaderline[s.httpheaderlineptr] == ISO_nl) {
      //We have an entire HTTP header line in s.httpheaderline, so we parse it.
      if(s.httpheaderline[0] == ISO_cr) {
        //This was the last header line (i.e., and empty "\r\n"), so
        //we are done with the headers and proceed with the actual data.
        s.state = EXO_STATE_DATA;
        return len;
      }

      s.httpheaderline[s.httpheaderlineptr - 1] = 0;
      //Check for specific HTTP header fields.
      if(casecmp(s.httpheaderline, http_content_type, sizeof(http_content_type) - 1) == 0) 
      {
        //Found Content-type field.
        cptr = strchr(s.httpheaderline, ';');
        if(cptr != NULL) {
          *cptr = 0;
        }
        strncpy(s.mimetype, s.httpheaderline +
        sizeof(http_content_type) - 1, sizeof(s.mimetype));
      } else if(casecmp(s.httpheaderline, http_location, sizeof(http_location) - 1) == 0) 
      {
        cptr = s.httpheaderline +
        sizeof(http_location) - 1;

        if(strncmp(cptr, http_http, 7) == 0) {
          cptr += 7;
          for(i = 0; i < s.httpheaderlineptr - 7; ++i) {
            if(*cptr == 0 ||
               *cptr == '/' ||
               *cptr == ' ' ||
               *cptr == ':') {
              s.host[i] = 0;
              break;
            }
            s.host[i] = *cptr;
            ++cptr;
          }
        }
        strncpy(s.file, cptr, sizeof(s.file));
        /*s.file[s.httpheaderlineptr - i] = 0;*/
      }
      //We're done parsing, so we reset the pointer and start the next line.
      s.httpheaderlineptr = 0;
    } else {
      ++s.httpheaderlineptr;
    }
  }
  return len;
}

/******************************************************************************
* unsigned char is_valid_cik(char *cik)
* 
* Checks to make sure the cik is syntactically legit.
* 
******************************************************************************/
unsigned char is_valid_cik(char *pCIK)
{
  unsigned char i;
  
  if (NULL == pCIK || 0 == *pCIK) return 0;
  
  for (i = 0; i < CIK_LENGTH; i++)
  {
    if (!(pCIK[i] >= 'a' && pCIK[i] <= 'f' || pCIK[i] >= '0' && pCIK[i] <= '9'))
      return 0;
  }
  return 1;
}

/******************************************************************************
* static void activate_device(void)
* 
* Activate this devices from its UUID + PID
* Currently just a stub - reference Micrium Exosite Demo (Ex4) for reference
* 
******************************************************************************/
static void activate_device(void) 
{
  volatile char stub[30]; //make volatile to enforce non-optimization for the stub
    
  memcpy((void *)stub, OSN, OSN_LENGTH);
  memcpy((void *)stub, OSV, OSV_LENGTH);
  memcpy((void *)stub, PID, PID_LENGTH);    

}


