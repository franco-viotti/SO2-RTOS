#ifndef DISPLAY_TASK_H
#define DISPLAY_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "utils.h"

// Definiciones para el display
#define DISPLAY_WIDTH    96  // Ancho del OLED
#define DISPLAY_HEIGHT   16  // Alto del OLED

// Declaración externa de la cola que usaremos
extern QueueHandle_t xFilteredTempQueue;

// Función para crear la tarea
void vStartDisplayTask(void);

#endif