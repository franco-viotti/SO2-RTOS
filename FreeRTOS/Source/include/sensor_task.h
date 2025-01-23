#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

// Includes de FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "utils.h"

// Definiciones
#define SENSOR_FREQ_HZ    1 //0 // TODO: add cero back
#define SENSOR_PERIOD_MS  (1000 / SENSOR_FREQ_HZ)

// La declaración externa de la cola permite que sensor_task.c pueda usar la cola
// que está definida en main.c
extern QueueHandle_t xTemperatureQueue;

// Funcion que inicializa la tarea del sensor
void vStartSensorTask(void);

#endif /* SENSOR_TASK_H */