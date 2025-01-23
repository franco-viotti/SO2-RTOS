#include "display_task2.h"
#include "filter_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define GRAPH_WIDTH  100  // Ancho del gráfico
#define GRAPH_HEIGHT 40   // Altura del gráfico
#define GRAPH_X      20   // Posición X del gráfico
#define GRAPH_Y      10   // Posición Y del gráfico

extern QueueHandle_t xFilteredTempQueue;
extern int currentN; // Número de muestras actual para el filtro
static const unsigned char dsimPixel[] = { 0xFF };

static void vDisplayTask(void *pvParameters)
{
  char str[10];
  TempData_t newData;
  int newTemp;
  long newTimeStamp;
  
  OSRAMInit(false);
  OSRAMStringDraw("TP4: RTOS", 21, 0);
  OSRAMStringDraw("FCEFYN: SO2", 16, 1);
  vTaskDelay(pdMS_TO_TICKS(3000));  // Esperar 3 segundos para mostrar el mensaje inicial

  for (;;)
  {
    if (xQueueReceive(xFilteredTempQueue, &newData, portMAX_DELAY) != pdPASS)
    {
      UARTSendError("Error al recibir de la cola de datos filtrados");
    }

    newTemp = newData.temperature;
    newTimeStamp = newData.time_ms/1000; // Representar en segundos

    // Limpiar la zona del gráfico antes de redibujar
    OSRAMClear();
    OSRAMStringDraw("T:", 0, 0);
    OSRAMStringDraw("t:", 0, 1);
    int_to_string(newTemp, str);
    OSRAMStringDraw(str, 10, 0);
    long_to_string(newTimeStamp, str);
    OSRAMStringDraw(str, 10, 1);

    UARTSend("Display recibio - Temp: ");
    int_to_string(newTemp, str);
    UARTSend(str);
    UARTSend(" Time(ms): ");
    long_to_string(newTimeStamp, str);
    UARTSend(str);
    UARTSend("\r\n");
    
    //// Graficar una barra vertical proporcional a currentN
    //int barHeight = currentN * GRAPH_HEIGHT / 100;  // Escalado
    //for (int y = 0; y < barHeight; y++)
    //{
    //  OSRAMImageDraw(dsimPixel, GRAPH_X, GRAPH_Y + GRAPH_HEIGHT - y, 1, 1);
    //}

    vTaskDelay(pdMS_TO_TICKS(500));  // Redibujar cada 500ms
  }
}

void vStartDisplayTask(void)
{
  xTaskCreate(vDisplayTask, "Display", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
}