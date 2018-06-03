
#include "environement.h"
#include "cmsis_os.h"
#include "global.h"

//#include "plf_config.h"
//#include "dc_service.h"
//#include "dc_common.h"
//#include "error_handler.h"

#include "x_nucleo_iks01a2.h"
#include "x_nucleo_iks01a2_accelero.h"
#include "x_nucleo_iks01a2_gyro.h"
#include "x_nucleo_iks01a2_magneto.h"
#include "x_nucleo_iks01a2_humidity.h"
#include "x_nucleo_iks01a2_temperature.h"
#include "x_nucleo_iks01a2_pressure.h"


//#define USE_TRACE_INTERFACE_PING_CLIENT (1)

/* Private macro -------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
osThreadId EnvironementHandle = NULL;
char HumidityChar[6];
char PressureChar[8];
char TemperatureChar[6];

static void StartEnvironement(void const * argument)
{
	static void *ACCELERO_handle = NULL;
	static void *GYRO_handle = NULL;
	static void *MAGNETO_handle = NULL;
	static void *HUMIDITY_handle = NULL;
	static void *TEMPERATURE_handle = NULL;
	static void *PRESSURE_handle = NULL;
	static SensorAxes_t ACC_Value;                  /*!< Acceleration Value */
	static SensorAxes_t GYR_Value;                  /*!< Gyroscope Value */
	static SensorAxes_t MAG_Value;                  /*!< Magnetometer Value */
	static float PRESSURE_Value;           /*!< Pressure Value */
	static float HUMIDITY_Value;           /*!< Humidity Value */
	static float TEMPERATURE_Value;        /*!< Temperature Value */

	  /* Try to use automatic discovery. By default use LSM6DSL on board */
	  //BSP_ACCELERO_Init( ACCELERO_SENSORS_AUTO, &ACCELERO_handle );
	  /* Try to use automatic discovery. By default use LSM6DSL on board */
	  //BSP_GYRO_Init( GYRO_SENSORS_AUTO, &GYRO_handle );
	  /* Try to use automatic discovery. By default use LSM303AGR on board */
	  //BSP_MAGNETO_Init( MAGNETO_SENSORS_AUTO, &MAGNETO_handle );
	  /* Try to use automatic discovery. By default use HTS221 on board */
	  BSP_HUMIDITY_Init( HUMIDITY_SENSORS_AUTO, &HUMIDITY_handle );
	  /* Try to use automatic discovery. By default use HTS221 on board */
	  BSP_TEMPERATURE_Init( TEMPERATURE_SENSORS_AUTO, &TEMPERATURE_handle );
	  /* Try to use automatic discovery. By default use LPS22HB on board */
	  BSP_PRESSURE_Init( PRESSURE_SENSORS_AUTO, &PRESSURE_handle );

//	  BSP_ACCELERO_Sensor_Enable( ACCELERO_handle );
//	  BSP_GYRO_Sensor_Enable( GYRO_handle );
//	  BSP_MAGNETO_Sensor_Enable( MAGNETO_handle );
	  BSP_HUMIDITY_Sensor_Enable( HUMIDITY_handle );
	  BSP_TEMPERATURE_Sensor_Enable( TEMPERATURE_handle );
	  BSP_PRESSURE_Sensor_Enable( PRESSURE_handle );



  while(1)
  {
	  uint8_t status = 0;


	    if(BSP_PRESSURE_IsInitialized(PRESSURE_handle, &status) == COMPONENT_OK && status == 1)
	    {
	      BSP_PRESSURE_Get_Press(PRESSURE_handle, &PRESSURE_Value);

	      /* DEBUG sensor values */
	      sprintf(PressureChar, "%4.2f", PRESSURE_Value);
	      printf("-E- Pressure : %s\n", PressureChar);
	    }

	    if(BSP_HUMIDITY_IsInitialized(HUMIDITY_handle, &status) == COMPONENT_OK && status == 1)
	    {
	      BSP_HUMIDITY_Get_Hum(HUMIDITY_handle, &HUMIDITY_Value);

	      /* DEBUG sensor values */
	      sprintf(HumidityChar, "%3.1f", HUMIDITY_Value);
	      printf("-E- Humidity : %s\n", HumidityChar);
	    }

	    if(BSP_TEMPERATURE_IsInitialized(TEMPERATURE_handle, &status) == COMPONENT_OK && status == 1)
	    {
	        BSP_TEMPERATURE_Get_Temp(TEMPERATURE_handle, &TEMPERATURE_Value);

	        /* DEBUG sensor values */
		      sprintf(TemperatureChar, "%2.1f", TEMPERATURE_Value);
		      printf("-E- Temperature : %s\n", TemperatureChar);
	    }
	    osDelay(10000);
  }
}


void EnvironementInit(void)
{
  osThreadDef(EnvironementTask, StartEnvironement, osPriorityLow, 0, 256);
  EnvironementHandle = osThreadCreate(osThread(EnvironementTask), NULL);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
