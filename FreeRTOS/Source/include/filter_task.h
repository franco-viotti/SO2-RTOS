#ifndef FILTER_TASK_H
#define FILTER_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Tamaño default del filtro
#define FILTER_N_DEFAULT   10

// Función para crear la tarea
void vStartFilterTask(void);

#endif