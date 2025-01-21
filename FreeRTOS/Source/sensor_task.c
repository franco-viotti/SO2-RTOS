#include "sensor_task.h"
#include <stdlib.h>  // para rand()
#include "DriverLib.h" // para el UART

// Función auxiliar para enviar un string por UART
static void UARTSend(const char *str)
{
  while(*str)
  {
    UARTCharPut(UART0_BASE, *str);
    str++;
  }
}

// Función para convertir int a string
static void int_to_string(int value, char* str) 
{
    // Para números de dos dígitos (TEMP_MIN-TEMP_MAX)
    str[0] = '0' + (value / 10);
    str[1] = '0' + (value % 10);
    str[2] = '\0';
}

// Función que simula la lectura de un sensor de temperatura
static void vSensorTask(void *pvParameters)
{
  // Almacena el tiempo de la última vez que la tarea se ejecutó
  TickType_t xLastWakeTime;
  // Almacena el valor de la temperatura simulada
  int temperature;
  char str[3];  // Buffer para convertir numeros

  // Inicializa la variable xLastWakeTime con el valor actual del contador de ticks del sistema
  xLastWakeTime = xTaskGetTickCount();

  for(;;)
  {
    temperature = TEMP_MIN + (rand() % (TEMP_MAX - TEMP_MIN + 1));

    UARTSend("Temperatura: ");
    int_to_string(temperature, str);
    UARTSend(str);
    UARTSend("°C\r\n");        
    
    // La tarea se bloquea hasta que pasen period ticks desde xLastWakeTime
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SENSOR_PERIOD_MS));
  }
}

// Función para crear la tarea
void vStartSensorTask(void)
{
  xTaskCreate(vSensorTask, "Sensor", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
}