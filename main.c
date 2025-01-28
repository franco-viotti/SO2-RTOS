/*
 * Trabajo Práctico N°4: RTOS
 * Facultad de Ciencias Exactas, Físicas y Naturales
 * Universidad Nacional de Córdoba
 * 
 * Descripción:
 * Este programa implementa un sistema de tiempo real utilizando FreeRTOS.
 * El sistema consta de varias tareas que interactúan entre sí mediante colas.
 * 
 * Tareas:
 * - Sensor Task: Lee valores de un sensor de temperatura.
 * - Filter Task: Aplica un filtro pasa bajos a los valores de temperatura.
 * - Display Task: Muestra los valores filtrados en una pantalla.
 * - UART Command Task: Procesa comandos recibidos por UART.
 * - Top Task: Imprime información sobre el funcionamiento del sistema.
 * 
 * Autor:
 * Franco Viotti
 */

/* Environment includes. */
#include "drivers/DriverLib.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Application includes. */
#include "utils.h"
#include "sensor_task.h"
#include "filter_task.h"
#include "display_task.h"
#include "uart_cmd_task.h"
#include "top_task.h"

/*
 * Configure the welcome message
 */
void vWelcomeMessage(void);

/* Cola para enviar los valores generados por el sensor al filtro */
QueueHandle_t xTemperatureQueue;  // Cola sensor -> filtro
QueueHandle_t xFilteredTempQueue; // Cola filtro -> display

int main(void) {
  /* Enviar mensaje de bienvenida por UART */
  vWelcomeMessage();

  /* Crear la cola del sensor al filtro pasa bajos */
  xTemperatureQueue = xQueueCreate(10, sizeof(int));

  /* Crear la cola del filtro al display */
  xFilteredTempQueue = xQueueCreate(10, sizeof(TempData_t));

  /* Verificar que las colas se hayan creado correctamente */
  if (xTemperatureQueue == NULL || xFilteredTempQueue == NULL) {
    UARTSendError("Error al crear la cola del sensor");
    while (1); // No podemos continuar
  }

  /* Start tasks */
  vStartSensorTask();
  vStartFilterTask();
  vStartDisplayTask();
  vStartUartCmdTask();
  vStartTopTask();

  /* Start the scheduler. */
  vTaskStartScheduler();

  /* Will only get here if there was insufficient heap to start the
  scheduler. */
  UARTSendError("Error al iniciar el scheduler"); // No deberiamos llegar aqui

  return 0;
}

void vWelcomeMessage(void) {
  UARTSend("\r\n\r\nBienvenidos al Trabajo Práctico N°4: RTOS\r\n");
  UARTSend("FCEFYN: SO2\r\n");
  UARTSend("Integrantes:\r\n");
  UARTSend("  - Franco Viotti\r\n");
  UARTSend("Algunas instrucciones: \r\n");
  UARTSend("  - Para cambiar la cantidad de muestras del filtro pasa bajos, ingrese N=X donde X es el nuevo valor\r\n\r\n\r\n");
}
