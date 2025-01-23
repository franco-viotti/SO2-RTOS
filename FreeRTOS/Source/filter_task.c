#include "filter_task.h"

int* samples = NULL;  // Buffer dinámico
int samplesCount = 0;  // Contador de muestras recibidas
static int currentIndex = 0; // Indice del buffer circular
int currentN = FILTER_N_DEFAULT;
volatile int requestedN = FILTER_N_DEFAULT;
SemaphoreHandle_t xNMutex;

static void resizeBuffer(int newSize)
{
  int* newBuffer = pvPortMalloc(newSize * sizeof(int));
  if(newBuffer == NULL)
  {
    UARTSendError("Error de memoria al redimensionar buffer");
    return;
  }

  // Inicializar nuevo buffer
  for(int i = 0; i < newSize; i++)
    newBuffer[i] = 0;

  // Copiar datos antiguos si existen
  if(samples != NULL)
  {
    int copySize = currentN < newSize ? currentN : newSize;
    for(int i = 0; i < copySize; i++)
      newBuffer[i] = samples[i];
    vPortFree(samples);
  }

  samples = newBuffer;
  currentN = newSize;
  // LOG NEW CURRENTN
  UARTSend("Desde resizeBuffer: nuevo N: ");
  char str[10];
  int_to_string(currentN, str);
  UARTSend(str);
  UARTSend("\r\n");
  //samplesCount = 0;
  //currentIndex = 0;
  //UARTSend("Nuevo samplesCount: ");
  //int_to_string(samplesCount, str);
  //UARTSend(str);
  //UARTSend("\r\n");
  //UARTSend("Nuevo currentIndex: ");
  //int_to_string(currentIndex, str);
  //UARTSend(str);
  //UARTSend("\r\n");
  vTaskDelay(pdMS_TO_TICKS(5000));  // Delay para evitar cambios muy rápidos
}

static void vFilterTask(void *pvParameters)
{
  vTaskDelay(pdMS_TO_TICKS(10000));
  
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
    // Verificar si hay que cambiar N
    if(xSemaphoreTake(xNMutex, 0) == pdPASS)
    {
      if(requestedN != currentN)
      {
        resizeBuffer(requestedN);
        // Log the new currentN
        UARTSend("Nuevo N: ");
        int_to_string(currentN, str);
        UARTSend(str);
        UARTSend("\r\n");
        samplesCount = 0;
        currentIndex = 0;
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay para evitar cambios muy rápidos
      }
      xSemaphoreGive(xNMutex);
    }
    else
    {
      UARTSendError("Error al tomar el semáforo xNMutex\r\n");
    }

    if(xQueueReceive(xTemperatureQueue, &newTemp, portMAX_DELAY) == pdPASS)
    {
      // Actualizar buffer circular
      samples[currentIndex] = newTemp;
      //currentIndex = (currentIndex + 1) % FILTER_N_DEFAULT;
      //if(samplesCount < FILTER_N_DEFAULT)
      //  samplesCount++;

      currentIndex = (currentIndex + 1) % currentN;
      if(samplesCount < currentN)
      {
        samplesCount++;
        UARTSend("Nuevo samplesCount: ");
        int_to_string(samplesCount, str);
        UARTSend(str);
        UARTSend("\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay para evitar cambios muy rápidos
      }
      else
      {
        UARTSend("samplesCount no es menor que currentN: ");
        int_to_string(samplesCount, str);
        UARTSend(str);
        UARTSend(" | ");
        int_to_string(currentN, str);
        UARTSend(str);
        UARTSend("\r\n");
      }

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