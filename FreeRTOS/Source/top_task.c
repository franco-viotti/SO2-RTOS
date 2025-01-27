#include "top_task.h"

#define TOP_DELAY_MS 3000

static void vTopTask(void *pvParameters) {
    char str[10];
    UBaseType_t uxHighWaterMark;
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    unsigned long ulTotalRunTime, ulStatsAsPercentage;

    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    UARTSend("TopTask initial stack: ");
    int_to_string(uxHighWaterMark, str);
    UARTSend(str);
    UARTSend("\n\r");

    /* Take a snapshot of the number of tasks in case it changes while this
       function is executing. */
    uxArraySize = uxTaskGetNumberOfTasks();

    /* Allocate a TaskStatus_t structure for each task. An array could be
      allocated statically at compile time. */
    pxTaskStatusArray = pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );

    if (pxTaskStatusArray != NULL) {
      for (;;) {
        /* Generate raw status information about each task. */
        uxArraySize = uxTaskGetSystemState( pxTaskStatusArray,
                                            uxArraySize,
                                            &ulTotalRunTime );

        // Encabezado
        UARTSend("\n\r----- Task Statistics -----\n\r");

        // Stack de la tarea actual
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        UARTSend("TopTask Stack: ");
        int_to_string(uxHighWaterMark, str);
        UARTSend(str);
        UARTSend("\n\r");

        // Memoria heap libre
        UARTSend("Heap Free: ");
        long_to_string(xPortGetFreeHeapSize(), str);
        UARTSend(str);
        UARTSend("\n\r");

        // Número total de tareas
        UARTSend("Total Tasks: ");
        int_to_string(uxTaskGetNumberOfTasks(), str);
        UARTSend(str);
        UARTSend("\n\r");

        vTaskDelay(pdMS_TO_TICKS(TOP_DELAY_MS));
      }
    }
    else {
      UARTSendError("Error al asignar memoria para TaskStatusArray");
      //vTaskDelete(NULL);
    }
}

void vStartTopTask(void) {
    xTaskCreate(vTopTask, 
                "Top", 
                configMINIMAL_STACK_SIZE,
                NULL, 
                1, 
                NULL);
}