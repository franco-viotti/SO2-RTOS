#include "top_task.h"

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

      // Mostrar información de cada tarea
      UARTSend("\n\r----- Task Statistics (Total: ");
      int_to_string(uxArraySize, str);
      UARTSend(str);
      UARTSend(") -----\n\r");
      UARTSend("Name\t\tState\tStack\tPrio\tCPU%\n\r"); // Encabezado
      
      // Recorrer cada tarea en el arreglo
      for (x = 0; x < uxArraySize; x++) {
        // Nombre de la tarea
        UARTSend(pxTaskStatusArray[x].pcTaskName);
        UARTSend("\t\t");

        // Estado de la tarea
        switch (pxTaskStatusArray[x].eCurrentState) {
          case eRunning:   UARTSend("RUN"); break;
          case eReady:     UARTSend("RDY"); break;
          case eBlocked:   UARTSend("BLK"); break;
          case eSuspended: UARTSend("SUS"); break;
          default:         UARTSend("???"); break;
        }
        UARTSend("\t");

        // Stack total disponible // TODO: corregir
        //int_to_string(pxTaskStatusArray[x].usStackHighWaterMark, str);
        //UARTSend(str);
        //UARTSend("\t");
        
        // Stack restante
        int_to_string(pxTaskStatusArray[x].usStackHighWaterMark, str);
        UARTSend(str);
        UARTSend("\t");

        // Prioridad
        int_to_string(pxTaskStatusArray[x].uxCurrentPriority, str);
        UARTSend(str);
        UARTSend("\t");

        UARTSend("\n\r");

        //// Porcentaje de CPU
        //if (ulTotalRunTime > 0) {
        //  ulStatsAsPercentage = (pxTaskStatusArray[x].ulRunTimeCounter * 100UL) / ulTotalRunTime;
        //  UARTSend("\t");
        //  int_to_string(ulStatsAsPercentage, str);
        //  UARTSend(str);
        //  UARTSend("%");
        //}

      }

      vTaskDelay(pdMS_TO_TICKS(TOP_DELAY_MS));
    }
  }
  else {
    UARTSendError("Error al asignar memoria para TaskStatusArray");
    //vTaskDelete(NULL);
  }
}

void vStartTopTask(void) {
  xTaskCreate(vTopTask, "Top", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
}