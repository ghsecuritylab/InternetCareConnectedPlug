/**
  ******************************************************************************
  * @file    httpserver.c
  * @brief   Basic http server
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

#include "string.h"
#include <stdio.h>
#include "cmsis_os.h"

#define LWIP_HTTPD_SSI 1
#define LWIP_HTTPD_CGI 1

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"

#include "httpserver.h"
#include "global.h"
#include "pingclient.h"
#include "../Middlewares/Third_Party/LwIP/src/apps/httpd/fsdata.h"

#define LWIP_HTTPD_SSI 1
#define LWIP_HTTPD_CGI 1
#include "lwip/apps/httpd.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u32_t nPageHits = 0;

//static const char* day_tab[7] = {"sunday","monday", "tuesday","wednesday","thursdays","friday","saturday"};

/* CGI handler for Restart control */
const char * RESTART_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

/* Html request for "/restart.cgi" will start RESTART_CGI_Handler */
const tCGI RESTART_CGI={"/restart.cgi", RESTART_CGI_Handler};

/* Cgi call table, only one CGI used */
tCGI CGI_TAB[1];


/* Format of dynamic web page: the page header */

/* we will use character "t" as tag for CGI */
//char const* TAGCHAR="IP";
//char const** TAGS=&TAGCHAR;

char const** TAGS[] = {{"IP"}, {"EXpingOK"}, {"INpingOK"}, {"EXboot"}, {"INboot"}, {"Temp"}, {"Hum"}, {"Press"}};

SemaphoreHandle_t SSI_mutex = NULL;
osThreadId httpServerTaskHandle = NULL;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  test_Handler : SSI handler for index.html page
  */
u16_t test_Handler(int iIndex, char *pcInsert, int iInsertLen)
{
  //if( xSemaphoreTake(SSI_mutex, ( TickType_t ) 10 ) == pdTRUE ){
  if (iIndex == 0)
  {
	if (gstate & STATE_IP)
	{
	char* chaine = "<img src=\"images/led_rectangular_h_green.png\" alt=\"ON\">";
		memcpy(pcInsert, chaine, strlen(chaine));
//		xSemaphoreGive(SSI_mutex);
		return strlen(chaine);
	}
	else
	{
	char* chaine = "<img src=\"images/led_rectangular_h_red.png\" alt=\"OFF\">";
			memcpy(pcInsert, chaine, strlen(chaine));
//			xSemaphoreGive(SSI_mutex);
			return strlen(chaine);
	}
  }
  if (iIndex == 1)
  {
	if (gstate & STATE_EXTERNAL_PING_OK)
	{
	char* chaine = "<img src=\"images/led_rectangular_h_green.png\" alt=\"ON\">";
		memcpy(pcInsert, chaine, strlen(chaine));
//		xSemaphoreGive(SSI_mutex);
		return strlen(chaine);
	}
	else if (gstate & STATE_EXTERNAL_PING_NA)
	{
	char* chaine = "<img src=\"images/led_rectangular_h_grey.png\" alt=\"N/A\">";
			memcpy(pcInsert, chaine, strlen(chaine));
//			xSemaphoreGive(SSI_mutex);
			return strlen(chaine);
	}
	else
	{
	char chaine[100];
	    sprintf(chaine, "<img src=\"images/led_rectangular_h_red.png\" alt=\"OFF\"><br />%d errors on %d max", External_campaign_ko, PINGCLIENT_NB_KO_REBOOT);
			memcpy(pcInsert, chaine, strlen(chaine));
//			xSemaphoreGive(SSI_mutex);
			return strlen(chaine);
	}
  }
  if (iIndex == 2)
  {
	if (gstate & STATE_INTERNAL_PING_OK)
	{
	char* chaine = "<img src=\"images/led_rectangular_h_green.png\" alt=\"ON\">";
		memcpy(pcInsert, chaine, strlen(chaine));
//		xSemaphoreGive(SSI_mutex);
		return strlen(chaine);
	}
	if (gstate & STATE_INTERNAL_PING_NA)
	{
	char* chaine = "<img src=\"images/led_rectangular_h_grey.png\" alt=\"N/A\">";
			memcpy(pcInsert, chaine, strlen(chaine));
//			xSemaphoreGive(SSI_mutex);
			return strlen(chaine);
	}
	else
	{
		char chaine[100];
		    sprintf(chaine, "<img src=\"images/led_rectangular_h_red.png\" alt=\"OFF\"><br />%d errors on %d max", Internal_campaign_ko, PINGCLIENT_NB_KO_REBOOT);
				memcpy(pcInsert, chaine, strlen(chaine));
//				xSemaphoreGive(SSI_mutex);
				return strlen(chaine);
	}
  }
  if (iIndex == 3)
  {
	if (gstate & STATE_RESTART_EXTERNAL)
	{
	char* chaine = "<img src=\"images/led_rectangular_h_blue.png\" alt=\"ON\">";
		memcpy(pcInsert, chaine, strlen(chaine));
//		xSemaphoreGive(SSI_mutex);
		return strlen(chaine);
	}
	else
	{
	char* chaine = "<img src=\"images/led_rectangular_h_grey.png\" alt=\"OFF\">";
			memcpy(pcInsert, chaine, strlen(chaine));
//			xSemaphoreGive(SSI_mutex);
			return strlen(chaine);
	}
  }
  if (iIndex == 4)
  {
	if (gstate & STATE_RESTART_INTERNAL)
	{
	char* chaine = "<img src=\"images/led_rectangular_h_blue.png\" alt=\"ON\">";
		memcpy(pcInsert, chaine, strlen(chaine));
//		xSemaphoreGive(SSI_mutex);
		return strlen(chaine);
	}
	else
	{
	char* chaine = "<img src=\"images/led_rectangular_h_grey.png\" alt=\"OFF\">";
			memcpy(pcInsert, chaine, strlen(chaine));
//			xSemaphoreGive(SSI_mutex);
			return strlen(chaine);
	}
  }
  if (iIndex == 5)
  {
    memcpy(pcInsert, TemperatureChar, strlen(TemperatureChar));
//    xSemaphoreGive(SSI_mutex);
    return (strlen(TemperatureChar));
  }
  else if (iIndex == 6)
  {
    memcpy(pcInsert, HumidityChar, strlen(HumidityChar));
//    xSemaphoreGive(SSI_mutex);
    return (strlen(HumidityChar));
  }
  if (iIndex == 7)
  {
    memcpy(pcInsert, PressureChar, strlen(PressureChar));
//    xSemaphoreGive(SSI_mutex);
    return (strlen(PressureChar));
  }
//  xSemaphoreGive(SSI_mutex);
  return 0;
//}
}


const char * RESTART_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
  uint32_t i=0;

  /* We have only one CGI handler iIndex = 0 */
  if (iIndex==0)
  {
    /* Check cgi parameter : application GET /restart.cgi?external=1&internal=2 */
	  // /restart.cgi?restart=external&restart=internal
    for (i=0; i<iNumParams; i++)
    {
    	printf ("------->%d = %s + %s\n",i, pcParam[i], pcValue[i]);
//      /* check parameter "led" */
      if (strcmp(pcParam[i] , "restart")==0)
      {
          /* restart external device */
          if(strcmp(pcValue[i], "external") == 0)
          {
        	  restart_external_device();
          }
          /* restart internal device */
          if(strcmp(pcValue[i], "internal") == 0)
          {
        	  restart_internal_device();
          }
      }
    }
  }
  /* uri to send after cgi call*/
  return ("/ok.html");
}




static void http_server_socket_thread(void const * argument)
{
  printf("-H- Tache HTTPD demarre...\n");
  httpd_init();
  /* configure SSI handlers (test page SSI) */
  http_set_ssi_handler(test_Handler, (char const **)TAGS, 8);

  /* configure CGI handlers (LEDs control CGI) */
  CGI_TAB[0] = RESTART_CGI;
  http_set_cgi_handlers(CGI_TAB, 1);

  osThreadTerminate(NULL);
}

/**
  * @brief  Initialize the HTTP server (start its thread)
  * @param  none
  * @retval None
  */
void http_server_init()
{
  osThreadDef(httpServerTask, http_server_socket_thread, HTTPSERVER_THREAD_PRIO, 0, HTTPSERVER_THREAD_STACK_SIZE);
  httpServerTaskHandle = osThreadCreate(osThread(httpServerTask), NULL);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
