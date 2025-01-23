#include "display_task.h"

// Variables globales para el historial
static TempData_t tempHistory[DISPLAY_WIDTH];
static int historyIndex = 0;

// Mapea temperatura a coordenada Y del display
static int mapTempToY(int temp) {
  return DISPLAY_HEIGHT - 1 - ((temp - TEMP_MIN) * (DISPLAY_HEIGHT - 1)) / (TEMP_MAX - TEMP_MIN);
}

/*
static void vDisplayTask(void *pvParameters)
{
  vTaskDelay(pdMS_TO_TICKS(10000));
  
  TempData_t newData;
  char str[10];
  char displayBuffer[DISPLAY_HEIGHT][DISPLAY_WIDTH + 1];

  // Inicializar historial
  for(int i = 0; i < DISPLAY_WIDTH; i++) {
    tempHistory[i].temperature = 0;
    tempHistory[i].time_ms = 0;
  }

  // Limpiar buffer y dibujar ejes
  for(int y = 0; y < DISPLAY_HEIGHT; y++) {
    for(int x = 0; x < DISPLAY_WIDTH; x++) {
      if(y == DISPLAY_HEIGHT-1) {  // Eje X
        displayBuffer[y][x] = '-';
      } else if(x == 0) {          // Eje Y
        displayBuffer[y][x] = '|';
      } else {
        displayBuffer[y][x] = ' ';
      }
    }
    displayBuffer[y][DISPLAY_WIDTH] = '\0';
  }

  // Dibujar los ejes una sola vez
  OSRAMClear();
  for(int y = 0; y < DISPLAY_HEIGHT; y++) {
    OSRAMStringDraw(displayBuffer[y], 0, y);
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  
  for(;;)
  { 
    if(xQueueReceive(xFilteredTempQueue, &newData, portMAX_DELAY) == pdPASS)
    {
      // Debug output
      UARTSend("Display recibio - Temp: ");
      int_to_string(newData.temperature, str);
      UARTSend(str);
      UARTSend(" Time(ms): ");
      long_to_string(newData.time_ms, str);
      UARTSend(str);
      UARTSend("\r\n");

      // Agregar nuevo punto al historial
      tempHistory[historyIndex] = newData;
      historyIndex = (historyIndex + 1) % DISPLAY_WIDTH;

      // Limpiar buffer
      for(int y = 0; y < DISPLAY_HEIGHT; y++) {
        for(int x = 0; x < DISPLAY_WIDTH; x++) {
          displayBuffer[y][x] = ' ';
        }
        displayBuffer[y][DISPLAY_WIDTH] = '\0';
      }

      // Dibujar todos los puntos
      for(int i = 0; i < DISPLAY_WIDTH; i++) {
        if(tempHistory[i].temperature != 0) {
          int y = mapTempToY(tempHistory[i].temperature);
          if(y >= 0 && y < DISPLAY_HEIGHT) {
            displayBuffer[y][i] = '*';
          }
        }
      }

      // Actualizar display
      for(int y = 0; y < DISPLAY_HEIGHT; y++) {
        OSRAMStringDraw(displayBuffer[y], 0, y);
        vTaskDelay(pdMS_TO_TICKS(10));
      }
    }
  }
}
*/

/*static void vDisplayTask(void *pvParameters)
{
    TempData_t newData;
    char displayBuffer[DISPLAY_HEIGHT][DISPLAY_WIDTH + 1];
    
    // Limpiar buffer y dibujar ejes
    for(int y = 0; y < DISPLAY_HEIGHT; y++) {
        for(int x = 0; x < DISPLAY_WIDTH; x++) {
            if(y == DISPLAY_HEIGHT-1) {  // Eje X
                displayBuffer[y][x] = '-';
            } else if(x == 0) {          // Eje Y
                displayBuffer[y][x] = '|';
            } else {
                displayBuffer[y][x] = ' ';
            }
        }
        displayBuffer[y][DISPLAY_WIDTH] = '\0';
    }

    // Dibujar los ejes una sola vez
    OSRAMClear();
    for(int y = 0; y < DISPLAY_HEIGHT; y++) {
        OSRAMStringDraw(displayBuffer[y], 0, y);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    for(;;)
    {
        if(xQueueReceive(xFilteredTempQueue, &newData, portMAX_DELAY) == pdPASS)
        {
            int y = mapTempToY(newData.temperature);
            if(y >= 0 && y < DISPLAY_HEIGHT) {
                OSRAMStringDraw("*", DISPLAY_WIDTH-1, y);
                vTaskDelay(pdMS_TO_TICKS(50));
            }
        }
    }
}*/

/*static void vDisplayTask(void *pvParameters)
{
  TempData_t newData;
  char str[10];
  
  // Test inicial
  OSRAMClear();
  OSRAMStringDraw("|", 0, 0);  // Eje Y
  
  UARTSend("Display iniciado\r\n");

  for(;;)
  {
    if(xQueueReceive(xFilteredTempQueue, &newData, portMAX_DELAY) == pdPASS)
    {
      int y = mapTempToY(newData.temperature);
      
      // Debug por UART
      UARTSend("Dibujando punto en y=");
      int_to_string(y, str);
      UARTSend(str);
      UARTSend("\r\n");
      
      // Intento simple de dibujo
      OSRAMStringDraw("*", 1, y);
      vTaskDelay(pdMS_TO_TICKS(100));
    }
}
}*/

static void vDisplayTask(void *pvParameters)
{
    TempData_t newData;
    char str[10];
    int x = 10;  // Dejamos espacio para la escala
    char scaleBuffer[DISPLAY_WIDTH + 1];
    
    OSRAMClear();
    
    // Dibujar escala de temperatura
    int_to_string(TEMP_MAX, str);
    OSRAMStringDraw(str, 0, 0);
    int_to_string(TEMP_MIN, str);
    OSRAMStringDraw(str, 0, DISPLAY_HEIGHT-1);
    
    // Dibujar eje vertical
    for(int y = 0; y < DISPLAY_HEIGHT; y++) {
        OSRAMStringDraw("|", 8, y);
    }
    
    for(;;)
    {
        if(xQueueReceive(xFilteredTempQueue, &newData, portMAX_DELAY) == pdPASS)
        {
            int y = mapTempToY(newData.temperature);
            OSRAMStringDraw("*", x, y);
            x = (x + 1) % DISPLAY_WIDTH;
            if(x < 10) x = 10;  // Mantener espacio para escala
        }
    }
}

void vStartDisplayTask(void)
{
  xTaskCreate(vDisplayTask, "Display", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
}