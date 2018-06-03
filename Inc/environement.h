
#ifndef __ENVIRONEMENT_H
#define __ENVIRONEMENT_H

//#ifdef __cplusplus
// extern "C" {
//#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_gpio.h"
#include "cmsis_os.h"

#define ENVIRONEMENT_THREAD_STACK_SIZE        256
#define ENVIRONEMENT_THREAD_PRIO osPriorityNormal

 /* Exported types ------------------------------------------------------------*/

extern osThreadId EnvironementHandle;

/* Exported functions ------------------------------------------------------------*/
void EnvironementInit(void);


//#ifdef __cplusplus
//}
//#endif
#endif
/***************************** (C) COPYRIGHT STMicroelectronics *******END OF FILE ************/
