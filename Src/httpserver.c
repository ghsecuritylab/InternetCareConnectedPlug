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

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"

#include "httpserver.h"
#include "global.h"
#include "../Middlewares/Third_Party/LwIP/src/apps/httpd/fsdata.h"
#include "lwip/apps/httpd.h"



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u32_t nPageHits = 0;
static const char* day_tab[7] = {"sunday","monday", "tuesday","wednesday","thursdays","friday","saturday"};

/* Format of dynamic web page: the page header */

/* we will use character "t" as tag for CGI */
//char const* TAGCHAR="IP";
//char const** TAGS=&TAGCHAR;

char const** TAGS[] = {{"IP"}, {"EXpingOK"}, {"INpingOK"}, {"EXboot"}, {"INboot"}};


osThreadId httpServerTaskHandle = NULL;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  test_Handler : SSI handler for index.html page
  */
u16_t test_Handler(int iIndex, char *pcInsert, int iInsertLen)
{
  if (iIndex == 0)
  {
	if (gstate & STATE_IP)
	{
	char* chaine = "<img src=\"images/led_rectangular_h_green.png\" alt=\"ON\">";
		memcpy(pcInsert, chaine, strlen(chaine));
        return strlen(chaine);
	}
	else
	{
	char* chaine = "<img src=\"images/led_rectangular_h_red.png\" alt=\"OFF\">";
			memcpy(pcInsert, chaine, strlen(chaine));
	        return strlen(chaine);
	}
  }
  if (iIndex == 1)
  {
	if (gstate & STATE_EXTERNAL_PING_OK)
	{
	char* chaine = "<img src=\"images/led_rectangular_h_green.png\" alt=\"ON\">";
		memcpy(pcInsert, chaine, strlen(chaine));
        return strlen(chaine);
	}
	else
	{
	char* chaine = "<img src=\"images/led_rectangular_h_red.png\" alt=\"OFF\">";
			memcpy(pcInsert, chaine, strlen(chaine));
	        return strlen(chaine);
	}
  }
  if (iIndex == 2)
  {
	if (gstate & STATE_INTERNAL_PING_OK)
	{
	char* chaine = "<img src=\"images/led_rectangular_h_green.png\" alt=\"ON\">";
		memcpy(pcInsert, chaine, strlen(chaine));
        return strlen(chaine);
	}
	else
	{
	char* chaine = "<img src=\"images/led_rectangular_h_red.png\" alt=\"OFF\">";
			memcpy(pcInsert, chaine, strlen(chaine));
	        return strlen(chaine);
	}
  }
  if (iIndex == 3)
  {
	if (gstate & STATE_RESTART_EXTERNAL)
	{
	char* chaine = "<img src=\"images/led_rectangular_h_blue.png\" alt=\"ON\">";
		memcpy(pcInsert, chaine, strlen(chaine));
        return strlen(chaine);
	}
	else
	{
	char* chaine = "<img src=\"images/led_rectangular_h_grey.png\" alt=\"OFF\">";
			memcpy(pcInsert, chaine, strlen(chaine));
	        return strlen(chaine);
	}
  }
  if (iIndex == 4)
  {
	if (gstate & STATE_RESTART_INTERNAL)
	{
	char* chaine = "<img src=\"images/led_rectangular_h_blue.png\" alt=\"ON\">";
		memcpy(pcInsert, chaine, strlen(chaine));
        return strlen(chaine);
	}
	else
	{
	char* chaine = "<img src=\"images/led_rectangular_h_grey.png\" alt=\"OFF\">";
			memcpy(pcInsert, chaine, strlen(chaine));
	        return strlen(chaine);
	}
  }
  return 0;
}

static void http_server_socket_thread(void const * argument)
{
  printf("-H- Tache HTTPD demarre...\n");
  httpd_init();
  /* configure SSI handlers (test page SSI) */
  http_set_ssi_handler(test_Handler, (char const **)TAGS, 5);

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
