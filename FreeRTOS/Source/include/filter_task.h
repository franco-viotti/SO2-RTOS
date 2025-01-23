#ifndef FILTER_TASK_H
#define FILTER_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "utils.h"

// Tamaño default del filtro
#define FILTER_N_DEFAULT   10

extern QueueHandle_t xTemperatureQueue;
extern QueueHandle_t xFilteredTempQueue;
extern SemaphoreHandle_t xNMutex;
extern int currentN;  // Número de muestras actual para el filtro
extern volatile int requestedN;  // Nuevo número de muestras para el filtro

// Función para crear la tarea
void vStartFilterTask(void);

#endif /* FILTER_TASK_H */