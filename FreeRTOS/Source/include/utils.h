#ifndef UTILS_H
#define UTILS_H

#include "DriverLib.h"
#include "FreeRTOS.h"
#include "queue.h"
#include  <string.h>

#define TEMP_MIN        20  // Temperatura mínima
#define TEMP_MAX        30  // Temperatura máxima

// Tamaños de stack para cada tarea. Se deja un 20% de margen
#define SENSOR_TASK_STACK_SIZE 42
#define FILTER_TASK_STACK_SIZE 58
#define DISPLAY_TASK_STACK_SIZE 60
#define UART_CMD_TASK_STACK_SIZE 48

// Función para enviar strings por UART
void UARTSend(const char *str);

// Función para enviar mensajes de error
void UARTSendError(const char *errorMsg);

// Función para convertir int a string
void int_to_string(int value, char* str);

// Función para convertir long a string
void long_to_string(long value, char* str);

// Estructura para pasar datos del filtro al display
typedef struct {
  int temperature;
  long time_ms;
} TempData_t;

#endif /* UTILS_H */