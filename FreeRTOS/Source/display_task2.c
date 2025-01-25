#include "display_task2.h"
#include "filter_task.h"
#include "utils.h"
#include <stdbool.h>

// Definiciones para el display y el gráfico
#define DISPLAY_WIDTH 96
#define DISPLAY_HEIGHT 16
#define GRAPH_START_X 24
#define TEMP_HISTORY_SIZE (DISPLAY_WIDTH - GRAPH_START_X - 5)  // Espacio disponible para graficar

// Referencias externas
extern QueueHandle_t xFilteredTempQueue;
extern int currentN;

// Estructura para el buffer circular que almacena temperaturas
static struct {
  int values[TEMP_HISTORY_SIZE];  // Array de temperaturas
  int head;                       // Índice para la próxima inserción
} tempBuffer = {0};               // Inicializado en cero

// Control del estado del buffer
static bool buffer_is_full = false;

// Convierte una temperatura en su representación de bits para el display
static void getBitForTemperature(int temp, unsigned char *point) {
  point[0] = 0x00; // Byte superior: inicialmente todos los bits en 0
  point[1] = 0x80; // Byte inferior: bit 7 en 1 para mantener el eje X

  // Mapeo de temperatura dentro del LCD
  // Necesitamos:
  // 1. Normalizar la temperatura (temp - TEMP_MIN), haciendo que TEMP_MIN sea 0
  // 2. Obtener el rango total de temperaturas (TEMP_MAX - TEMP_MIN)
  // * 14 y el 14 - al inicio mapean este rango a nuestros bits disponibles (0-14, excluyendo el bit 15 que es para el eje X)
  int bit_position = 14 - ((temp - TEMP_MIN) * 14) / (TEMP_MAX - TEMP_MIN);

  // Manejo de casos especiales
  if (temp == TEMP_MAX) bit_position = 0;    // TEMP_MAX se dibuja en el bit más alto
  if (bit_position >= 15) bit_position = 14; // Prevenir escritura en bit del eje X

  // Finalmente, colocamos el bit en el byte correspondiente
  if (bit_position < 8)
    point[0] |= (1 << bit_position); // Bits 0-7 en byte superior
  else 
    point[1] |= (1 << (bit_position - 8)); // Bits 8-14 en byte inferior
}

static void vDisplayTask(void *pvParameters) {
  char str[10];
  TempData_t newData;
  const int x_axis_start = GRAPH_START_X + 1;
  
  OSRAMInit(false);
  
  for (;;) {
    if (xQueueReceive(xFilteredTempQueue, &newData, portMAX_DELAY) == pdPASS) {
      // Actualizar buffer circular
      tempBuffer.values[tempBuffer.head] = newData.temperature;
      tempBuffer.head = (tempBuffer.head + 1) % TEMP_HISTORY_SIZE;
      
      if (tempBuffer.head == 0)
        buffer_is_full = true;  // Indicar que completamos una vuelta

      OSRAMClear();
      
      // Dibujar información textual
      OSRAMStringDraw("T:", 0, 0);
      OSRAMStringDraw("t:", 0, 1);
      int_to_string(newData.temperature, str);
      OSRAMStringDraw(str, 10, 0);
      long_to_string(newData.time_ms/1000 % 100, str);
      OSRAMStringDraw(str, 10, 1);
      
      // Dibujar eje Y
      unsigned char y_axis[] = {0xFF, 0xFF};
      OSRAMImageDraw(y_axis, GRAPH_START_X, 0, 1, 2);
      
      // Dibujar eje X
      unsigned char x_axis[] = {0x80};
      for (int i = x_axis_start; i < DISPLAY_WIDTH; i++)
        OSRAMImageDraw(x_axis, i, 1, 1, 1);

      // Graficar histórico de temperaturas
      int points_to_draw = buffer_is_full ? TEMP_HISTORY_SIZE : tempBuffer.head;
      for (int i = 0; i < points_to_draw; i++) {
        // Calcular posición en el buffer de manera circular
        int buffer_pos = (tempBuffer.head - points_to_draw + i + TEMP_HISTORY_SIZE) % TEMP_HISTORY_SIZE;

        // Convertir temperatura a representación de bits y dibujar
        unsigned char temp_point[2];
        getBitForTemperature(tempBuffer.values[buffer_pos], temp_point);
        OSRAMImageDraw(temp_point, x_axis_start + i, 0, 1, 2);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(500));
}
}

void vStartDisplayTask(void) {
  xTaskCreate(vDisplayTask, "Display", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
}