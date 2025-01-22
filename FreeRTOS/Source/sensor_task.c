#include "sensor_task.h"
#include <stdlib.h>  // para rand()
#include "utils.h"

// Función que simula la lectura de un sensor de temperatura
static void vSensorTask(void *pvParameters)
{
  TickType_t xLastWakeTime;
  int temperature;
  char str[3];

  xLastWakeTime = xTaskGetTickCount();

  for(;;)
  {
    temperature = TEMP_MIN + (rand() % (TEMP_MAX - TEMP_MIN + 1));

    if(xQueueSend(xTemperatureQueue, &temperature, portMAX_DELAY) != pdPASS)
    {
      UARTSendError("Fallo al enviar a cola");
    }

    UARTSend("Enviando a cola: ");  // Debug
    int_to_string(temperature, str);
    UARTSend(str);
    UARTSend("°C\r\n");
    
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SENSOR_PERIOD_MS));
  }
}

// Función para crear la tarea
void vStartSensorTask(void)
{
  xTaskCreate(vSensorTask, "Sensor", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
}