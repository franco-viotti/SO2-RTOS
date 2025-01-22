#include "filter_task.h"
#include "utils.h"

// Buffer circular para las muestras
static int samples[FILTER_N_DEFAULT];
static int currentIndex = 0;

extern QueueHandle_t xTemperatureQueue;

static void vFilterTask(void *pvParameters)
{
  int newTemp, sum, average;
  char str[10];
  int samplesCount = 0;  // Contador de muestras recibidas

  // Inicializar el buffer con ceros
  for(int i = 0; i < FILTER_N_DEFAULT; i++)
  {
    samples[i] = 0;
  }

  for(;;)
  {
    if(xQueueReceive(xTemperatureQueue, &newTemp, portMAX_DELAY) == pdPASS)
    {
      UARTSend("Recibido de cola: ");
      int_to_string(newTemp, str);
      UARTSend(str);
      UARTSend("°C\r\n");
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

      // Mostrar resultado
      UARTSend("Filtrado (");
      int_to_string(samplesCount, str);
      UARTSend(str);
      UARTSend(" muestras): ");
      int_to_string(average, str);
      UARTSend(str);
      UARTSend("°C\r\n");
    }
  }
}

void vStartFilterTask(void)
{
  xTaskCreate(vFilterTask, "Filter", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
}