#include "display_task2.h"
#include "filter_task.h"

#define DISPLAY_WIDTH 96
#define DISPLAY_HEIGHT 16
#define GRAPH_START_X 20

extern QueueHandle_t xFilteredTempQueue;
extern int currentN;

static void vDisplayTask(void *pvParameters) {
  char str[10];
  TempData_t newData;
  static int x_start = GRAPH_START_X + 5;
  unsigned char bufTmp[2];
  
  OSRAMInit(false);
  
  for (;;) {
    if (xQueueReceive(xFilteredTempQueue, &newData, portMAX_DELAY) == pdPASS) {
      OSRAMClear();
      
      // Dibujar textos
      OSRAMStringDraw("T:", 0, 0);
      OSRAMStringDraw("t:", 0, 1);
      int_to_string(newData.temperature, str);
      OSRAMStringDraw(str, 10, 0);
      long_to_string(newData.time_ms/1000, str);
      OSRAMStringDraw(str, 10, 1);
      
      // Dibujar eje Y
      unsigned char y_axis[] = {0xFF, 0xFF};
      OSRAMImageDraw(y_axis, GRAPH_START_X + 4, 0, 1, 2);
      
      // Dibujar eje X
      unsigned char x_axis[] = {0x80}; // Un solo byte
      for (int i = GRAPH_START_X + 5; i < DISPLAY_WIDTH; i++) {
        OSRAMImageDraw(x_axis, i, 1, 1, 1);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void vStartDisplayTask(void) {
  xTaskCreate(vDisplayTask, "Display", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
}