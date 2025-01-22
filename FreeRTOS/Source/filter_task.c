#include "filter_task.h"

int* samples = NULL;  // Buffer dinámico
int samplesCount = 0;  // Contador de muestras recibidas
static int currentIndex = 0; // Indice del buffer circular
int currentN = FILTER_N_DEFAULT;

static void vFilterTask(void *pvParameters)
{
  int newTemp, sum, average;
  TempData_t filteredData;
  char str[10];
  int samplesCount = 0;  // Contador de muestras recibidas
  TickType_t startTime = xTaskGetTickCount();  // Tiempo inicial

  // Asignar memoria para el buffer e inicializar con ceros
  samples = pvPortMalloc(FILTER_N_DEFAULT * sizeof(int));
  if(samples == NULL)
  {
    UARTSendError("Error de memoria al asignar buffer 'samples'");
    return;
  }
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

      // Imprimir valores recibidos desde el sensor
      UARTSend("Recibido desde el sensor: "); // TODO: BORRAR
      int_to_string(newTemp, str);
      UARTSend(str);
      UARTSend("°C\r\n");

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

      // Enviar a display // TODO: DEJAR COMENTADO PORQUE BLOQUEA
      //if (xQueueSend(xFilteredTempQueue, &filteredData, portMAX_DELAY) != pdPASS)
      //{
      //  UARTSendError("Fallo al enviar a cola");
      //}

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