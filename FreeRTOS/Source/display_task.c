#include "display_task.h"
#include "DriverLib.h"

// Mapea temperatura a coordenada Y del display
static int mapTempToY(int temp) {
  return DISPLAY_HEIGHT - 1 - ((temp - TEMP_MIN) * (DISPLAY_HEIGHT - 1)) / (TEMP_MAX - TEMP_MIN);
}

static void vDisplayTask(void *pvParameters)
{
  TempData_t newData;
  char str[10];
  char displayBuffer[DISPLAY_HEIGHT][DISPLAY_WIDTH + 1];  // +1 para null terminator

  for(;;)
  { 
    if(xQueueReceive(xFilteredTempQueue, &newData, portMAX_DELAY) == pdPASS)
    {
      UARTSend("Display recibio - Temp: ");
      int_to_string(newData.temperature, str);
      UARTSend(str);
      UARTSend(" Time(ms): ");
      long_to_string(newData.time_ms, str);
      UARTSend(str);
      UARTSend("\r\n");

      // Limpiar buffer del display
      for(int y = 0; y < DISPLAY_HEIGHT; y++) {
        for(int x = 0; x < DISPLAY_WIDTH; x++) {
          displayBuffer[y][x] = ' ';
        }
        displayBuffer[y][DISPLAY_WIDTH] = '\0';
      }

      // Dibujar punto actual
      int y = mapTempToY(newData.temperature);
      if(y >= 0 && y < DISPLAY_HEIGHT) {
        displayBuffer[y][DISPLAY_WIDTH-1] = '*';  // Último pixel
      }

      // Desplazar display hacia la izquierda
      // TODO: Implementar

      // Actualizar display
      for(int y = 0; y < DISPLAY_HEIGHT; y++) {
        OSRAMStringDraw(displayBuffer[y], 0, y);
      }
    }
    else
    {
      UARTSendError("Error al recibir de la cola del filtro");
    }
  }
}

void vStartDisplayTask(void)
{
  xTaskCreate(vDisplayTask, "Display", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
}