#include "display_task2.h"
#include "filter_task.h"
#include "utils.h"

#define DISPLAY_WIDTH 96
#define DISPLAY_HEIGHT 16
#define GRAPH_START_X 20

extern QueueHandle_t xFilteredTempQueue;
extern int currentN;

static void getBitForTemperature(int temp, unsigned char *point) {
  point[0] = 0x00;  // byte superior
  point[1] = 0x80;  // byte inferior con el bit del eje X
  
  int bit_position = 7 - ((temp - TEMP_MIN) * 8) / (TEMP_MAX - TEMP_MIN);
  
  if (bit_position < 8) {
    point[0] |= (1 << bit_position);
  } else {
    point[1] |= (1 << (bit_position - 8));
  }
}

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
      long_to_string(newData.time_ms/1000 % 100, str);
      OSRAMStringDraw(str, 10, 1);
      
      // Dibujar eje Y
      unsigned char y_axis[] = {0xFF, 0xFF};
      OSRAMImageDraw(y_axis, GRAPH_START_X + 4, 0, 1, 2);
      
      // Dibujar eje X
      unsigned char x_axis[] = {0x80}; // Un solo byte
      for (int i = x_start; i < DISPLAY_WIDTH; i++) {
        OSRAMImageDraw(x_axis, i, 1, 1, 1);
      }

      unsigned char temp_point[2];
      getBitForTemperature(newData.temperature, temp_point);
      OSRAMImageDraw(temp_point, x_start, 0, 1, 2);
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void vStartDisplayTask(void) {
  xTaskCreate(vDisplayTask, "Display", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
}