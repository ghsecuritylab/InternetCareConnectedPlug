/*
 * restart.c
 *
 *  Created on: Feb 26, 2018
 *      Author: laporte
 */



#include "restart.h"
#include "cmsis_os.h"
#include "global.h"
#include "pingclient.h"
//#include "plf_config.h"
//#include "dc_service.h"
//#include "dc_common.h"
//#include "error_handler.h"


//#define USE_TRACE_INTERFACE_PING_CLIENT (1)

/* Private macro -------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
osThreadId RestartInternalHandle = NULL;
osThreadId RestartExternalHandle = NULL;

void restart_internal_device(void)
{
  if ((gstate & (STATE_RESTART_INTERNAL | STATE_RESTART_EXTERNAL)) == 0)
  {
    printf("-P- Restart internal\n");
    dataSet(data_mutex, &Internal_campaign_ko, 0);
    restart_internal_init();
  }
}

void restart_external_device(void)
{
  if ((gstate & STATE_RESTART_EXTERNAL) == 0)
  {
	printf("-P- Restart external\n");
    dataSet(data_mutex, &External_campaign_ko, 0);
    dataSet(data_mutex, &Internal_campaign_ko, 0);
    restart_external_init();
  }
  else
  {
    printf("Can\'t restart, restart already ongoing\n");
  }
}

static void restart_internal(void const * argument)
{
  uint32_t Counter = 0;
  uint32_t State = 0;

  while(1)
  {
	if (State == 0)
	{
	  printf ("/R/ Coupure du relais interne\n");
	  gstateRm(gstate_mutex, STATE_INTERNAL_PING_OK);
	  gstateAdd(gstate_mutex, STATE_RESTART_INTERNAL);
	  gstateAdd(gstate_mutex, STATE_INTERNAL_PING_NA);
      // Couper le relais pour la camera IP
	  HAL_GPIO_WritePin(Internal_Relay_GPIO_Port, Internal_Relay_Pin, GPIO_PIN_RESET);
      State++;
	}
	if ((State == 1) && (Counter >= RESTART_OFF_TIC_INTERNAL))
	{
	  printf("/R/ reprise du relais interne apres %d tics\n", RESTART_OFF_TIC_INTERNAL);
      // Remettre le relais de la camera IP
	  HAL_GPIO_WritePin(Internal_Relay_GPIO_Port, Internal_Relay_Pin, GPIO_PIN_SET);
	  State++;
	}
	//Clignotement de la LED
    if (Counter %2)
    {
      HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_RESET);
    }
    else
    {
      HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);
    }
    osDelay(RESTART_TIC_INTERNAL);
    // Au bout de toutes les tempos, on remet l'état du système sans "Restart internal"
    // et on tue cette tache
    if (++Counter >= RESTART_WAIT_TIC_INTERNAL + RESTART_OFF_TIC_INTERNAL)
    {
    	HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_RESET);
    	printf("/R/ Reprise normale pour l'appareil interne apres %d tics\n", RESTART_WAIT_TIC_INTERNAL);
    	gstateRm(gstate_mutex, STATE_RESTART_INTERNAL);
        gstateRm(gstate_mutex, STATE_INTERNAL_PING_NA);
    	//vTaskDelete(NULL);
    	osThreadTerminate(NULL);
    }
  }
}

void restart_internal_init(void)
{
  if ((gstate & (STATE_RESTART_INTERNAL | STATE_RESTART_EXTERNAL)) == 0)
  {
    osThreadDef(Restart_internalTask, restart_internal, osPriorityNormal, 0, 128);
    RestartInternalHandle = osThreadCreate(osThread(Restart_internalTask), NULL);
  }
}

//##############################################################################################################
//##############################################################################################################
//##############################################################################################################

static void restart_external(void const * argument)
{
  uint32_t Counter = 0;
  uint32_t State = 0;

  // Couper le relais pour le routeur
  //HAL_GPIO_WritePin(External_Relay_GPIO_Port, External_Relay_Pin, GPIO_PIN_RESET);
  // Remettre le relais de l'appareil interne (IP CAM)
  //HAL_GPIO_WritePin(External_Relay_GPIO_Port, External_Relay_Pin, GPIO_PIN_SET);

  // Attendre un instant de retrouver un état supposé fonctionnel
  while(1)
  {
	if (State == 0)
	{
	  printf ("/R/ Coupure du relais externe\n");
	  gstateRm(gstate_mutex, STATE_EXTERNAL_PING_OK);
	  gstateAdd(gstate_mutex, STATE_RESTART_EXTERNAL);
	  gstateAdd(gstate_mutex, STATE_EXTERNAL_PING_NA);
      // Couper le relais pour le routeur
      HAL_GPIO_WritePin(External_Relay_GPIO_Port, External_Relay_Pin, GPIO_PIN_RESET);
      State++;
	}
	if ((State == 1) && (Counter >= RESTART_OFF_TIC_EXTERNAL))
	{
	  printf("/R/ reprise du relais externe apres %d tics\n", RESTART_OFF_TIC_EXTERNAL);
      // Remettre le relais du routeur
	  HAL_GPIO_WritePin(External_Relay_GPIO_Port, External_Relay_Pin, GPIO_PIN_SET);
	  State++;
	}
	//Clignotement de la LED
    if (Counter %2)
    {
      HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_RESET);
    }
    else
    {
      HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_SET);
    }
    osDelay(RESTART_TIC_EXTERNAL);
    // Au bout de toutes les tempos, on remet l'état du système sans "Restart external"
    // et on tue cette tache
    if (++Counter >= RESTART_WAIT_TIC_EXTERNAL + RESTART_OFF_TIC_EXTERNAL)
    {
    	HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_RESET);
    	printf("/R/ Reprise normale pour l'appareil externe apres %d tics\n", RESTART_WAIT_TIC_EXTERNAL);
    	gstateRm(gstate_mutex, STATE_RESTART_EXTERNAL);
        gstateRm(gstate_mutex, STATE_EXTERNAL_PING_NA);

    	//vTaskDelete(NULL);
    	osThreadTerminate(NULL);
    }
  }
}


void restart_external_init(void)
{
  osThreadDef(Restart_externalTask, restart_external, osPriorityNormal, 0, 128);
  RestartExternalHandle = osThreadCreate(osThread(Restart_externalTask), NULL);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
