#include "filter_task.h"

// Buffer circular para las muestras
static int samples[FILTER_N_DEFAULT];
static int currentIndex = 0;

static void vFilterTask(void *pvParameters)
{
  int newTemp, sum, average;
  TempData_t filteredData;
  char str[10];
  int samplesCount = 0;  // Contador de muestras recibidas
  TickType_t startTime = xTaskGetTickCount();  // Tiempo inicial

  // Inicializar el buffer con ceros
  for(int i = 0; i < FILTER_N_DEFAULT; i++)
  {
    samples[i] = 0;
  }

  for(;;)
  {
    if(xQueueReceive(xTemperatureQueue, &newTemp, portMAX_DELAY) == pdPASS)
    {
      // Actualizar buffer circular
      samples[currentIndex] = newTemp;
      currentIndex = (currentIndex + 1) % FILTER_N_DEFAULT;
      if(samplesCount < FILTER_N_DEFAULT)
        samplesCount++;

      // Calcular promedio usando samplesCount en lugar de FILTER_N_DEFAULT
      sum = 0;
      for(int i = 0; i < samplesCount; i++)
      {
        sum += samples[i];
      }
      int average = sum/samplesCount;

      // Preparar dato filtrado con timestamp actual
      filteredData.temperature = average;
      filteredData.time_ms = (xTaskGetTickCount() - startTime) * portTICK_PERIOD_MS;

      // Enviar a display
      if (xQueueSend(xFilteredTempQueue, &filteredData, portMAX_DELAY) != pdPASS)
      {
        UARTSendError("Fallo al enviar a cola");
      }

      // Mostrar resultado
      UARTSend("Filtrado (");
      int_to_string(samplesCount, str);
      UARTSend(str);
      UARTSend(" muestras): ");
      int_to_string(average, str);
      UARTSend(str);
      UARTSend("°C\r\n");
    }
    else
    {
      UARTSendError("Error al recibir de la cola del sensor");
    }
  }
}

void vStartFilterTask(void)
{
  xTaskCreate(vFilterTask, "Filter", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
}