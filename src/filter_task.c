#include "filter_task.h"

int* samples = NULL;  // Buffer dinámico
int samplesCount = 0;  // Contador de muestras recibidas
static int currentIndex = 0; // Indice del buffer dinámico
int currentN = FILTER_N_DEFAULT; // Inicialmente, N es igual a FILTER_N_DEFAULT
volatile int requestedN = FILTER_N_DEFAULT; // Lo mismo para requestedN
SemaphoreHandle_t xNMutex;

// Función para redimensionar el buffer
static void resizeBuffer(int newSize) {
  int* newBuffer = pvPortMalloc(newSize * sizeof(int));
  if(newBuffer == NULL) {
    UARTSendError("Error de memoria al redimensionar buffer");
    return;
  }

  // Inicializar nuevo buffer
  for(int i = 0; i < newSize; i++)
    newBuffer[i] = 0;

  // Copiar datos antiguos si existen
  if(samples != NULL) {
    // La cantidad de muestras a copiar puede ser menor si se reduce N
    // o igual a currentN si se aumenta N
    int copySize = currentN < newSize ? currentN : newSize;
    
    for(int i = 0; i < copySize; i++)
      newBuffer[i] = samples[i];
    
    // Liberar memoria del buffer anterior
    vPortFree(samples);
  }

  samples = newBuffer;
  currentN = newSize;
}

static void vFilterTask(void *pvParameters) {
  int newTemp, sum, average;
  TempData_t filteredData;
  char str[10];
  int samplesCount = 0;  // Contador de muestras recibidas
  TickType_t startTime = xTaskGetTickCount();  // Tiempo inicial

  // Asignar memoria para el buffer e inicializar con ceros
  samples = pvPortMalloc(FILTER_N_DEFAULT * sizeof(int));
  if(samples == NULL) {
    UARTSendError("Error de memoria al asignar buffer 'samples'");
    return;
  }

  for(int i = 0; i < FILTER_N_DEFAULT; i++)
    samples[i] = 0;

  for(;;) {
    // Verificar si hay que cambiar N
    if(requestedN != currentN) {
      resizeBuffer(requestedN);
      // Arrancamos el buffer desde el indice 0 y sin muestras
      samplesCount = 0;
      currentIndex = 0;
    }

    if(xQueueReceive(xTemperatureQueue, &newTemp, portMAX_DELAY) == pdPASS) {
      // Actualizar buffer circular con el nuevo valor de temperatura obtenido de la cola
      samples[currentIndex] = newTemp;

      // Actualizar el índice teniendo en cuenta el tamaño actual de N
      currentIndex = (currentIndex + 1) % currentN;
      if(samplesCount < currentN)
        samplesCount++;

      // Calcular promedio usando samplesCount en lugar de currentN
      // esto nos sirve sobre todo cuando samplesCount < currentN
      sum = 0;
      for(int i = 0; i < samplesCount; i++)
        sum += samples[i];

      int average = sum/samplesCount;

      // Preparar dato filtrado con timestamp actual
      filteredData.temperature = average;
      filteredData.time_ms = (xTaskGetTickCount() - startTime) * portTICK_PERIOD_MS;

      // Enviar a display
      if (xQueueSend(xFilteredTempQueue, &filteredData, portMAX_DELAY) != pdPASS) {
        UARTSendError("Fallo al enviar a cola");
      }

      // Mostrar resultado
      // TODO: descomentar para mostrar el filtrado con la cantidad de muestras
      UARTSend("Filtrado (");
      int_to_string(samplesCount, str);
      UARTSend(str);
      UARTSend(" muestras): ");
      int_to_string(average, str);
      UARTSend(str);
      UARTSend("°C\r\n");
    }
    else
      UARTSendError("Error al recibir de la cola del sensor");
  }
}

void vStartFilterTask(void) {
  xTaskCreate(vFilterTask, "Filter", FILTER_TASK_STACK_SIZE, NULL, configMAX_PRIORITIES - 2, NULL);
}