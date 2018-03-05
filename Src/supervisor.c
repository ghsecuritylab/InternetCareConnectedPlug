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

#include "supervisor.h"
#include "cmsis_os.h"
#include "global.h"
#include "restart.h"

//#include "plf_config.h"
//#include "dc_service.h"
//#include "dc_common.h"
//#include "error_handler.h"


//#define USE_TRACE_INTERFACE_PING_CLIENT (1)

/* Private macro -------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
osThreadId SupervisorHandle = NULL;

static void StartSupervisor(void const * argument)
{
  uint16_t CableErrorCount = 0;
  uint16_t IPErrorCount = 0;
  uint16_t ExternalPingErrorCount = 0;
  uint16_t InternalPingErrorCount = 0;

  while(1)
  {
	uint16_t n;

	printf("\n-S- Etat : ");
	n = gstate;
	while (n) {
	    if (n & 1)
	        printf("1");
	    else
	        printf("0");

	    n >>= 1;
	}
	printf("\n");
	if(gstate & STATE_INIT){
		printf("-S- Pas sorti d'init, pas de cable, pas d'ip\n");
		CableErrorCount++;
	}
	else{
		if(gstate & STATE_NO_CABLE){
			CableErrorCount++;
			printf("-S- Pas de detection de cable, probleme....\n");
		}
		else {
			if(gstate & STATE_IP == 0){
				printf("-S- Pas d'IP, probleme....\n");
				IPErrorCount++;
			}
			else{
				if((gstate & (STATE_EXTERNAL_PING_OK | STATE_RESTART_EXTERNAL)) == STATE_EXTERNAL_PING_OK)
				{
					gstateRm(gstate_mutex, STATE_EXTERNAL_PING_OK);
					HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_SET);
					printf("-S- Ping externe OK\n");
					CableErrorCount = 0;
					IPErrorCount = 0;
					ExternalPingErrorCount = 0;
				}
				if((gstate & (STATE_EXTERNAL_PING_KO | STATE_RESTART_EXTERNAL)) == STATE_EXTERNAL_PING_KO)
				{
					gstateRm(gstate_mutex, STATE_EXTERNAL_PING_KO);
					HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_RESET);
					printf("-S- Ping externe KO\n");
					CableErrorCount = 0;
					IPErrorCount = 0;
					ExternalPingErrorCount++;
				}

				if((gstate & (STATE_INTERNAL_PING_OK | STATE_RESTART_INTERNAL)) == STATE_INTERNAL_PING_OK)
				{
					gstateRm(gstate_mutex, STATE_INTERNAL_PING_OK);
//					GPIO_SetPin(GPIOB, LD3_Pin);
					HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);
					printf("-S- Ping interne OK\n");
					CableErrorCount = 0;
					IPErrorCount = 0;
					InternalPingErrorCount = 0;
				}
				if((gstate & (STATE_INTERNAL_PING_KO | STATE_RESTART_INTERNAL)) == STATE_INTERNAL_PING_KO)
				{
					gstateRm(gstate_mutex, STATE_INTERNAL_PING_KO);
					HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_RESET);
					printf("-S- Ping interne KO\n");
					CableErrorCount = 0;
					IPErrorCount = 0;
					InternalPingErrorCount++;
				}
			}
		}
	}
	if ((CableErrorCount >0) || (IPErrorCount >0))
	{
		HAL_GPIO_WritePin(GPIOB, LD2_Pin|LD3_Pin, GPIO_PIN_RESET);
	}
	if (ExternalPingErrorCount > 10)
	{
		gstateAdd(gstate_mutex, STATE_RESTART_EXTERNAL);
		gstateRm(gstate_mutex, STATE_EXTERNAL_PING_KO);
		gstateRm(gstate_mutex, STATE_EXTERNAL_PING_OK);
		ExternalPingErrorCount = 0;
		restart_external_init();
	}
	if ((InternalPingErrorCount > 10) && ((gstate & STATE_RESTART_EXTERNAL) == 0))
	{
		gstateAdd(gstate_mutex, STATE_RESTART_INTERNAL);
		gstateRm(gstate_mutex, STATE_INTERNAL_PING_KO);
		gstateRm(gstate_mutex, STATE_INTERNAL_PING_OK);
		InternalPingErrorCount = 0;
		restart_internal_init();
	}
	if ((InternalPingErrorCount > 10) && ((gstate & STATE_RESTART_EXTERNAL) == STATE_RESTART_EXTERNAL))
	{
		gstateRm(gstate_mutex, STATE_INTERNAL_PING_KO);
		InternalPingErrorCount = 0;
	}

	printf("-S- CableErrorCount        = %d\n", CableErrorCount);
	printf("-S- IPErrorCount           = %d\n", IPErrorCount);
	printf("-S- ExternalPingErrorCount = %d\n", ExternalPingErrorCount);
	printf("-S- InternalPingErrorCount = %d\n", InternalPingErrorCount);
	//HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_RESET);
    osDelay(10000);
    //HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_SET);

  }
}


void SupervisorInit(void)
{
  osThreadDef(SupervisorTask, StartSupervisor, osPriorityNormal, 0, 128);
  SupervisorHandle = osThreadCreate(osThread(SupervisorTask), NULL);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
