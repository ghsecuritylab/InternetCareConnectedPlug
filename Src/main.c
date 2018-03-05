/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f2xx_hal.h"
#include "cmsis_os.h"
#include "lwip.h"

/* USER CODE BEGIN Includes */
//#include <stdio.h>
#include "pingclient.h"
#include "supervisor.h"
#include "global.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart3;

osThreadId defaultTaskHandle;

/* USER CODE BEGIN PV */

uint16_t gstate = STATE_INIT;

/* Private variables ---------------------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
       /* redirect to USART */
       HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);
       /* redirect to ITM channel 0 */
       //ITM_SendChar((uint8_t *)ch);
       return ch;
}

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
SemaphoreHandle_t gstate_mutex = NULL;
//gstate_mutex = xSemaphoreCreateMutex();

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  printf("\n\nDebut !!\n\r");

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  SupervisorInit();
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 13;
  RCC_OscInitStruct.PLL.PLLN = 195;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* USART3 init function */
static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
     PA8   ------> USB_OTG_FS_SOF
     PA9   ------> USB_OTG_FS_VBUS
     PA10   ------> USB_OTG_FS_ID
     PA11   ------> USB_OTG_FS_DM
     PA12   ------> USB_OTG_FS_DP
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(External_Relay_GPIO_Port, External_Relay_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Internal_Relay_GPIO_Port, Internal_Relay_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin External_Relay_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|External_Relay_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_SOF_Pin USB_ID_Pin USB_DM_Pin USB_DP_Pin */
  GPIO_InitStruct.Pin = USB_SOF_Pin|USB_ID_Pin|USB_DM_Pin|USB_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_VBUS_Pin */
  GPIO_InitStruct.Pin = USB_VBUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_VBUS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Internal_Relay_Pin */
  GPIO_InitStruct.Pin = Internal_Relay_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Internal_Relay_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for LWIP */
  //MX_LWIP_Init();

  /* USER CODE BEGIN 5 */
  uint32_t ip, a, b, c, d;
  uint32_t phyreg;
  uint8_t CablePresent = 0;
  ip4_addr_t CP_ipaddr;
  ip4_addr_t CP_netmask;
  ip4_addr_t CP_gw;
  extern struct netif gnetif;

  HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_RESET);
  gstate_mutex = xSemaphoreCreateMutex();
  gstateSet(gstate_mutex, STATE_INIT);
  /* add the network interface (IPv4/IPv6) with RTOS */
  CP_ipaddr.addr = 0;
  CP_netmask.addr = 0;
  CP_gw.addr = 0;
  //netif_add(&gnetif, &CP_ipaddr, &CP_netmask, &CP_gw, NULL, &ethernetif_init, &tcpip_input);
  while (CablePresent == 0) {
    netif_add(&gnetif, &CP_ipaddr, &CP_netmask, &CP_gw, NULL, &ethernetif_init, &tcpip_input);
    HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &phyreg);
    if ((phyreg & PHY_LINKED_STATUS)==0){
      gstateSet(gstate_mutex, STATE_NO_CABLE);
      netif_remove(&gnetif);
      printf("Pas de cable... pause de 5 s\n");
      osDelay(5000);
      /* add the network interface (IPv4/IPv6) with RTOS */
//      CP_ipaddr.addr = 0;
//      CP_netmask.addr = 0;
//      CP_gw.addr = 0;
      //netif_add(&gnetif, &CP_ipaddr, &CP_netmask, &CP_gw, NULL, &ethernetif_init, &tcpip_input);
    }
    else {
      gstateSet(gstate_mutex, STATE_CABLE_OK);
      printf("A y est, cable !\n");
      netif_remove(&gnetif);
      CablePresent = 1;
    }
  }
  printf ("Apres cable present\n");
  MX_LWIP_Init();
//  netif_set_up(&gnetif);
//  printf("aa\n");
//  dhcp_start(&gnetif);
//  printf("bb\n");
//  //dhcp_supplied_address(*gnetif);
  uint16_t mscnt = 0;
//  osDelay(20000);
  while (netif_is_link_up(&gnetif) == 0) {
	  gstateSet(gstate_mutex, STATE_NO_CABLE);
    printf("cable non branche.... attente de 5s\n");
    osDelay(5000);
  }

  printf("cable branche\n");
  gstateSet(gstate_mutex, STATE_CABLE_OK);
  while (gnetif.ip_addr.addr==0) {
	  printf("boucle courte %d\n", mscnt);
    sys_msleep(DHCP_FINE_TIMER_MSECS);
    dhcp_fine_tmr();
    mscnt += DHCP_FINE_TIMER_MSECS;
    if (mscnt >= DHCP_COARSE_TIMER_SECS*1000) {
  	  printf("boucle longue %d\n", mscnt);
      dhcp_coarse_tmr();
      mscnt = 0;
    }
  }

  gstateSet(gstate_mutex, STATE_IP);
  HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_SET);
  printf("\nDone with DHCP\n");
  ip = gnetif.ip_addr.addr;
        a = ip & 0xFF;
        b = ip & 0xFF00;
        b = b >> 0x8;
        c = ip & 0xFF0000 ;
        c = c >> 0x10;
        d = ip & 0xFF000000 ;
        d = d >> 0x18;
  printf("Nucleo ip address is  %" "3d.%03d.%03d.%03d\n",a,b,c,d);

  ip = gnetif.gw.addr;
        a = ip & 0xFF;
        b = ip & 0xFF00;
        b = b >> 0x8;
        c = ip & 0xFF0000 ;
        c = c >> 0x10;
        d = ip & 0xFF000000 ;
        d = d >> 0x18;
  printf("Gateway ip address is %03d.%03d.%03d.%03d\n",a,b,c,d);
  ip = gnetif.netmask.addr;
        a = ip & 0xFF;
        b = ip & 0xFF00;
        b = b >> 0x8;
        c = ip & 0xFF0000 ;
        c = c >> 0x10;
        d = ip & 0xFF000000 ;
        d = d >> 0x18;
  printf("Subnet mask is        %03d.%03d.%03d.%03d\n\n",a,b,c,d);

  ping_client_init();
  http_server_init();

  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
    HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &phyreg);
	if ((phyreg & PHY_LINKED_STATUS)==0){
		HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_RESET);
      gstateSet(gstate_mutex, STATE_NO_CABLE);
	  printf("Cable debranche...\n");
	  netif_set_down(&gnetif);
	  CablePresent = 0;
	}
	else if (CablePresent == 0) {
		gstateSet(gstate_mutex, STATE_CABLE_OK);
	  netif_set_up(&gnetif);
	  //netif_remove(&gnetif);
	  ///dhcp_start(&gnetif);
	  while (gnetif.ip_addr.addr==0) {
	  	  printf("boucle courte %d\n", mscnt);
	      sys_msleep(DHCP_FINE_TIMER_MSECS);
	      dhcp_fine_tmr();
	      mscnt += DHCP_FINE_TIMER_MSECS;
	      if (mscnt >= DHCP_COARSE_TIMER_SECS*1000) {
	    	  printf("boucle longue %d\n", mscnt);
	        dhcp_coarse_tmr();
	        mscnt = 0;
	      }
	    }
	  gstateSet(gstate_mutex, STATE_IP);
	  HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_SET);
	  printf("Cable rebranche...\n");
	  printf("\nAcquired IP =");
	  ip = gnetif.ip_addr.addr;
	  a = ip & 0xFF;
	  b = ip & 0xFF00;
	  b = b >> 0x8;
	  c = ip & 0xFF0000 ;
	  c = c >> 0x10;
	  d = ip & 0xFF000000 ;
	  d = d >> 0x18;
	  printf("got ip address %i.%i.%i.%i\n\r",a,b,c,d);
	  CablePresent = 1;
	}
  }
  /* USER CODE END 5 */ 
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
