#include "sensor_task.h"
#include "utils.h"
#include <stdlib.h>

// Función que simula la lectura de un sensor de temperatura
static void vSensorTask(void *pvParameters)
{
  
  /* MONITOREO DE STACK */
  UBaseType_t uxHighWater = uxTaskGetStackHighWaterMark(NULL);
  char str[10];
  int_to_string(uxHighWater, str);
  UARTSend("[INFO] Stack inicial para la tarea vSensorTask: ");
  UARTSend(str);
  UARTSend("\n\r");
  /* MONITOREO DE STACK */
  
  TickType_t xLastWakeTime;
  int temperature;

  xLastWakeTime = xTaskGetTickCount();

  /* MONITOREO DE STACK */
  uxHighWater = uxTaskGetStackHighWaterMark(NULL);
  int_to_string(uxHighWater, str);
  UARTSend("[INFO] Stack previo al bucle for para la tarea vSensorTask: ");
  UARTSend(str);
  UARTSend("\n\r");
  /* MONITOREO DE STACK */

  for(;;)
  {
    /* MONITOREO DE STACK */
    uxHighWater = uxTaskGetStackHighWaterMark(NULL);
    int_to_string(uxHighWater, str);
    UARTSend("[INFO] Stack al inicio del bucle for para la tarea vSensorTask: ");
    UARTSend(str);
    UARTSend("\n\r");
    /* MONITOREO DE STACK */

    temperature = TEMP_MIN + (rand() % (TEMP_MAX - TEMP_MIN + 1));

    if(xQueueSend(xTemperatureQueue, &temperature, portMAX_DELAY) != pdPASS)
    {
      UARTSendError("Fallo al enviar a cola");
    }
    
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SENSOR_PERIOD_MS));

    /* MONITOREO DE STACK */
    uxHighWater = uxTaskGetStackHighWaterMark(NULL);
    int_to_string(uxHighWater, str);
    UARTSend("[INFO] Stack al final del bucle for para la tarea vSensorTask: ");
    UARTSend(str);
    UARTSend("\n\r");
    /* MONITOREO DE STACK */
  }
}

// Función para crear la tarea
void vStartSensorTask(void)
{
  xTaskCreate(vSensorTask, "Sensor", SENSOR_TASK_STACK_SIZE, NULL, 3, NULL);
}