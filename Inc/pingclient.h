/**
  ******************************************************************************
  * @file    pingclient.h
  * @brief   Basic ping client implementation to send icmp ping   
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
#ifndef __PING_CLIENT_H
#define __PING_CLIENT_H

//#ifdef __cplusplus
// extern "C" {
//#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"

#ifndef PING_USE_SOCKETS
#define PING_USE_SOCKETS    1
#endif

#define PINGCLIENT_THREAD_STACK_SIZE        256
#define PINGCLIENT_THREAD_PRIO osPriorityNormal

#define PINGCLIENT_MAX_PING_COUNT         5    // number of ping for each of the 2 IPs to ping
#define PINGCLIENT_MIN_EXTERNAL_PING_RATE 80  // Percent between 0 and 100
#define PINGCLIENT_MIN_INTERNAL_PING_RATE 80  // Percent between 0 and 100
#define PINGCLIENT_SEND_PERIOD            500  // in millisecond
#define PINGCLIENT_CAMPAIGN_PERIOD        3000 // in millisecond
 /* Exported types ------------------------------------------------------------*/
typedef enum
{
    PING_CLIENT_OK = 0x00,
    PING_CLIENT_ERROR
} ping_client_status_t;


extern osThreadId pingClientTaskHandle;

/* Exported functions ------------------------------------------------------------*/
void ping_client_init(void);


//#ifdef __cplusplus
//}
//#endif

#endif /* __PING_CLIENT_H */
/***************************** (C) COPYRIGHT STMicroelectronics *******END OF FILE ************/
