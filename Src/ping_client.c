/**
  ******************************************************************************
  * @file    pingclient.c
  * @brief   application to send icmp ping
  ******************************************************************************
  *
  * Copyright (c) 2016 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "pingclient.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/icmp.h"
#include "string.h"
#include <stdio.h>
#include "cmsis_os.h"
//#include "plf_config.h"
//#include "dc_service.h"
//#include "dc_common.h"
//#include "error_handler.h"
#include "lwip/inet_chksum.h"
#include "lwip/ip4.h"
#include "ip.h"
#include "global.h"
#include "restart.h"

//#define USE_TRACE_INTERFACE_PING_CLIENT (1)
#define USE_PRINTF (1)
#if (USE_TRACE_INTERFACE_PING_CLIENT == 1)
#include "trace_interface.h"
#define PrintINFO(format, args...) TracePrint(DBG_CHAN_ATCMD, DBL_LVL_P0, "" format , ## args)
#elif (USE_PRINTF == 1)
#define PrintINFO(format, args...)  printf("" format , ## args)
#else
#define PrintINFO(format, args...)  do {} while(0)
#endif





/* Private macro -------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#ifndef NORMAL_USE
// INTERNAL IP
#define DEST1_IP_ADDR0   (uint8_t) 10
#define DEST1_IP_ADDR1   (uint8_t) 44
#define DEST1_IP_ADDR2   (uint8_t) 114
#define DEST1_IP_ADDR3   (uint8_t) 189

// EXTERNAL IP
#define DEST_IP_ADDR0   (uint8_t) 8
#define DEST_IP_ADDR1   (uint8_t) 8
#define DEST_IP_ADDR2   (uint8_t) 8
#define DEST_IP_ADDR3   (uint8_t) 8
#else
// INTERNAL IP
#define DEST1_IP_ADDR0   (uint8_t) 192
#define DEST1_IP_ADDR1   (uint8_t) 168
#define DEST1_IP_ADDR2   (uint8_t) 1
#define DEST1_IP_ADDR3   (uint8_t) 1
//
// EXTERNAL IP
#define DEST_IP_ADDR0   (uint8_t) 8
#define DEST_IP_ADDR1   (uint8_t) 8
#define DEST_IP_ADDR2   (uint8_t) 8
#define DEST_IP_ADDR3   (uint8_t) 8
#endif

/** ping identifier - must fit on a u16_t */
#ifndef PING_ID
#define PING_ID        0xAFAF
#endif

/** ping additional data size to include in the packet */
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE 32
#endif

/** ping target - should be a "ip_addr_t" */

#ifndef PING_TARGET
//#define PING_TARGET   (netif_default?netif_default->gw:ip_addr_any)
#define PING_TARGET   (target_ip_addr)
#endif
/** ping receive timeout - in milliseconds */
#ifndef PING_RCV_TIMEO
#define PING_RCV_TIMEO_SEC 5
#define PING_RCV_TIMEO_USEC 0
#define PING_RCV_TIMEO_MS 5000
#endif
    
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
osThreadId pingClientTaskHandle = NULL;
static u16_t ping_seq_num=0;
static uint8_t ping_counter = 0;
//const ip_addr_t target_ip_addr = IPADDR4_INIT(TARGET_IPADDR);
static u32_t ping_time;
static u16_t ping_toggle = 0;
static uint16_t External_ping_ok = 0;
static uint16_t Internal_ping_ok = 0;
uint16_t External_campaign_ko = 0;
uint16_t Internal_campaign_ko = 0;

/* State of PING_CLIENT (1 if active, 0 if not)*/
static uint8_t ping_client_process_flag = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//u16_t checksum(unsigned short *buf, int nwords)
//{       //
//        u16_t sum = 0;
//        for(sum=0; nwords>0; nwords--)
//                sum += *buf++;
//        sum = (sum >> 16) + (sum &0xffff);
//        sum += (sum >> 16);
//        return (u16_t)(~sum);
//}

static void ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len)
{
  size_t i;
  size_t data_len = len - sizeof(struct icmp_echo_hdr);
  char tampon[100];
  ICMPH_TYPE_SET(iecho, ICMP_ECHO);
  ICMPH_CODE_SET(iecho, 0);
  iecho->chksum = 0; //0x4E57
  iecho->id     = PING_ID;
  iecho->seqno  = htons(++ping_seq_num);

  /* fill the additional data buffer with some data */
  for(i = 0; i < data_len; i++) {
    ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
  }

//  memcpy(tampon,(unsigned short *)iecho,len);
//  iecho->chksum = checksum ((unsigned short *)tampon, len);
//  printf ("%x, %x\n", 0x4E57, iecho->chksum);
//  iecho->chksum = checksum ((unsigned short *)iecho+4, len-4);
//  printf ("%x, %x\n", 0x4E57, iecho->chksum);
//
//  iecho->chksum = inet_chksum((void *)iecho, len);
//  printf ("%x, %x\n", 0x4E57, iecho->chksum);

}




static uint32_t ping_recv(int s)
{
  char buf[64]="";
  int fromlen, len = 0;
  struct sockaddr_in from;
  struct ip_hdr *iphdr;
  struct icmp_echo_hdr *iecho;
  size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
  
 // while((len = recvfrom(s, buf, ping_size, 0, (struct sockaddr*)&from, (socklen_t*)&fromlen)) > 0) {
   if ((len = recvfrom(s, buf, ping_size, 0, (struct sockaddr*)&from, (socklen_t*)&fromlen)) > 0)
   {
    if (len >= (int)(sizeof(struct ip_hdr)+sizeof(struct icmp_echo_hdr))) {
//      ip_addr_t fromaddr;
//      inet_addr_to_ipaddr(&fromaddr, &from.sin_addr);
//      PrintINFO("ping response received\n\r");
      iphdr = (struct ip_hdr *)buf;
      iecho = (struct icmp_echo_hdr *)(buf + (IPH_HL(iphdr) * 4));
      if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) 
      {
        if (ICMPH_TYPE(iecho) == ICMP_ER)
        {
//          PrintINFO("ICMP ECHO REPLY received\n\r");
          PrintINFO("ping ok response time: %"U32_F" ms\n\r", (sys_now()- ping_time));
        }
        else
        {
          PrintINFO("ICMP Other Response received \n\r");
          return(1);
        }
    } 
    else  // len < ICMP ECHO REPLY DATA
    {
          PrintINFO("ping drop\n\r");
          return(1);
    }
   }
  }
  else 
    if (len == -1) { // recvfrom timeout expiry
      PrintINFO("timeout expires (%lu sec.%lu usec) \n\r",PING_RCV_TIMEO_SEC,PING_RCV_TIMEO_USEC );
      return(1);
  }
  return(0);
}


static uint32_t icmp_ping_client_process(ip_addr_t ping_target)
{
      struct sockaddr_in to;
      int sock;
      uint32_t r;

//      struct timeval timeout ;
      u32_t timeout=PING_RCV_TIMEO_MS ; // 10 seconds (non standard format)
      struct icmp_echo_hdr *pecho;

      size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;

//      timeout.tv_sec = PING_RCV_TIMEO_SEC ;
//      timeout.tv_usec= PING_RCV_TIMEO_USEC;
      PrintINFO("-P- ping %d.%d.%d.%d : %i bytes,", ip4_addr1(&ping_target),ip4_addr2(&ping_target),ip4_addr3(&ping_target),ip4_addr4(&ping_target),PING_DATA_SIZE);
//      PrintINFO("icmp_ping_client_process : socket open\n\r");
      if ((sock = socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0) 
      {
          PrintINFO("ping_client_process : socket fail %d\n\r", sock);
          close(sock);
          return(1);
      }
      if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))<0) 
      {
          PrintINFO("ping_client_process : setsockopt() fail\n\r");
          close(sock);
          return(1);
      }     
//      getsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, &read_timeout);
//      PrintINFO("ping_client_process:getsockopt() timeout=%lu s %lu us\n\r",timeout.tv_sec, timeout.tv_usec);
        
      pecho = (struct icmp_echo_hdr *)mem_malloc((mem_size_t)ping_size);
      if (!pecho) {
          PrintINFO("ping_client_process : message alloc fails\n\r");
          close(sock);
          return(1);
      }
      ping_prepare_echo(pecho, (u16_t)ping_size);

      to.sin_family      = AF_INET;
      to.sin_port        = htons(INADDR_ANY);
      to.sin_addr.s_addr = ping_target.addr;
//      PrintINFO("icmp_ping_client_process : sendto\n\r");      
      if( sendto(sock, pecho, ping_size, 0, (struct sockaddr*)&to, sizeof(to)) < 0)
      {
        PrintINFO("ping_client_process : send fail\n\r");
        mem_free(pecho);
        close(sock);
        return(1);
      }
      mem_free(pecho);
//      PrintINFO("ping_client_process : send ping ok \n\r");
      ping_time = sys_now();
//      PrintINFO("icmp_ping_client_process : receive from\n\r");            
      r = ping_recv(sock);
//      PrintINFO("icmp_ping_client_process : socket close\n\r");
      close(sock);
      return(r);
}


/**
  * @brief  ping client thread
  * @param arg: pointer on argument(not used here)
  * @retval None
  */
static void ping_client_socket_thread(void const * argument)
{

  ip_addr_t ping_target;
  printf("-P- Tache Ping demarre...\n");
  gstate_mutex = xSemaphoreCreateMutex();

  while(1)
  {
        if ((gstate & STATE_IP) == STATE_IP)
        {
          /*  PING CLIENT ACTIVE */
          if (ping_counter < PINGCLIENT_MAX_PING_COUNT * 2) // deux ping effectués alternativement
          // max ping count not reached
          {
            if ((gstate & STATE_RESTART_EXTERNAL) == 0)
            // router device not under reboot
            {
              if ((ping_toggle % 2)==0)
              {
                //EXTERNAL IP
                IP4_ADDR( &ping_target, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3 );
                if (icmp_ping_client_process(ping_target) == 0)
                {
            	  External_ping_ok++;
                }
              }
            }
            if ((gstate & (STATE_RESTART_EXTERNAL | STATE_RESTART_INTERNAL)) == 0)
            {
              // Neither internal device nor router is under reboot
                if ((ping_toggle % 2)==1)
                {
        	       // INTERNAL IP
                   IP4_ADDR( &ping_target, DEST1_IP_ADDR0, DEST1_IP_ADDR1, DEST1_IP_ADDR2, DEST1_IP_ADDR3 );
                   if (icmp_ping_client_process(ping_target) == 0)
                   {
            	     Internal_ping_ok++;
                   }
                }
              } // End Internal
              ping_counter++;
              ping_toggle++;
              osDelay(PINGCLIENT_SEND_PERIOD);
            } // Campaign iteration
            else
            {
              // End of campaign
              //Update datas
            	if ((gstate & STATE_RESTART_EXTERNAL) == 0) // External restart not on going
            	{
            	  printf("-P- Taux de reussite ping externe : %d%%\n", (100 * External_ping_ok) / PINGCLIENT_MAX_PING_COUNT);
                  if (((100 * External_ping_ok) / PINGCLIENT_MAX_PING_COUNT) > PINGCLIENT_MIN_EXTERNAL_PING_RATE)
  		          {
  		            //gstateRm(gstate_mutex, STATE_EXTERNAL_PING_KO);
  		            gstateAdd(gstate_mutex, STATE_EXTERNAL_PING_OK);
  		            HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_SET);
                    printf("-P- External campaign OK\n");
  		          }
  		          else
  		          {
  		            gstateRm(gstate_mutex, STATE_EXTERNAL_PING_OK);
  		            //gstateAdd(gstate_mutex, STATE_EXTERNAL_PING_KO);
  		            HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_RESET);
  		            dataSet(data_mutex, &External_campaign_ko, External_campaign_ko + 1);
                    printf("-P- External campaign KO %d time(s)\n", External_campaign_ko);
  		          }
            	} // END External restart not on going
            	if ((gstate & (STATE_RESTART_EXTERNAL | STATE_RESTART_INTERNAL)) == 0) // External or Internal restart on going
            	{
                  printf("-P- Taux de reussite ping interne : %d%%\n", (100 * Internal_ping_ok) / PINGCLIENT_MAX_PING_COUNT);
                  if (((100 * Internal_ping_ok) / PINGCLIENT_MAX_PING_COUNT) > PINGCLIENT_MIN_INTERNAL_PING_RATE)
  		          {
  		            //gstateRm(gstate_mutex, STATE_INTERNAL_PING_KO);
  		            gstateAdd(gstate_mutex, STATE_INTERNAL_PING_OK);
  		            HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);
                    printf("-P- Internal campaign OK\n");
  		          }
  		          else
  		          {
  		            gstateRm(gstate_mutex, STATE_INTERNAL_PING_OK);
  		            //gstateAdd(gstate_mutex, STATE_INTERNAL_PING_KO);
  		            HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_RESET);
  		            dataSet(data_mutex, &Internal_campaign_ko, Internal_campaign_ko + 1);
                    printf("-P- Internal campaign KO %d time(s)\n", Internal_campaign_ko);
  		          }
            	} // END External or Internal restart on going
                if (External_campaign_ko >= PINGCLIENT_NB_KO_REBOOT)
                {
                  restart_external_device();
                }
                if (Internal_campaign_ko >= PINGCLIENT_NB_KO_REBOOT)
                {
                  restart_internal_device();
                }
                printf("-P- Pause de %d secondes\n", PINGCLIENT_CAMPAIGN_PERIOD/1000);
                printf("-------------------------------------------------------\n");
                osDelay(PINGCLIENT_CAMPAIGN_PERIOD);
                ping_counter = 0;
                External_ping_ok = 0;
                Internal_ping_ok = 0;
              }
          }
  }
}


void ping_client_init(void)
{
  osThreadDef(pingClientTask, ping_client_socket_thread, PINGCLIENT_THREAD_PRIO, 0, PINGCLIENT_THREAD_STACK_SIZE);
  pingClientTaskHandle = osThreadCreate(osThread(pingClientTask), NULL);
  if(pingClientTaskHandle == NULL)
  {
      //ERROR_Handler(DBG_CHAN_MAIN, 50, ERROR_FATAL);
  }
  else
  {
#if (STACK_ANALYSIS_TRACE == 1)
      stackAnalysis_addThreadStackSizeByHandle(pingClientTaskHandle, PINGCLIENT_THREAD_STACK_SIZE);
#endif /* STACK_ANALYSIS_TRACE */
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
