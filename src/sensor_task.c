#include "sensor_task.h"
#include "utils.h"
#include <stdlib.h>

// Función que simula la lectura de un sensor de temperatura
static void vSensorTask(void *pvParameters) {
  TickType_t xLastWakeTime;
  int temperature;
  char str[3];

  xLastWakeTime = xTaskGetTickCount();

  for(;;) {
    temperature = TEMP_MIN + (rand() % (TEMP_MAX - TEMP_MIN + 1));

    // Si no hay espacio en la cola, la tarea se bloquea
    if(xQueueSend(xTemperatureQueue, &temperature, portMAX_DELAY) != pdPASS)
      UARTSendError("Fallo al enviar a cola");
    
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SENSOR_PERIOD_MS));
  }
}

// Función para crear la tarea
void vStartSensorTask(void) {
  xTaskCreate(vSensorTask, "Sensor", SENSOR_TASK_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
}