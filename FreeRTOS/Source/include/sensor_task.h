#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

// Includes de FreeRTOS
#include "FreeRTOS.h"
#include "task.h"

// Definiciones
#define SENSOR_FREQ_HZ    10
#define SENSOR_PERIOD_MS  (1000 / SENSOR_FREQ_HZ)
#define TEMP_MIN          0 //.0f // temperatura mínima en grados Celsius
#define TEMP_MAX          30 //.0f // temperatura máxima en grados Celsius

// Funcion que inicializa la tarea del sensor
void vStartSensorTask(void);

#endif /* SENSOR_TASK_H */