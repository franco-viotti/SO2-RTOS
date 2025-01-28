/*
 * FreeRTOS V202212.01
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */


/*
 * This project contains an application demonstrating the use of the
 * FreeRTOS.org mini real time scheduler on the Luminary Micro LM3S811 Eval
 * board.  See http://www.FreeRTOS.org for more information.
 *
 * main() simply sets up the hardware, creates all the demo application tasks,
 * then starts the scheduler.  http://www.freertos.org/a00102.html provides
 * more information on the standard demo tasks.
 *
 * In addition to a subset of the standard demo application tasks, main.c also
 * defines the following tasks:
 *
 * + A 'Print' task.  The print task is the only task permitted to access the
 * LCD - thus ensuring mutual exclusion and consistent access to the resource.
 * Other tasks do not access the LCD directly, but instead send the text they
 * wish to display to the print task.  The print task spends most of its time
 * blocked - only waking when a message is queued for display.
 *
 * + A 'Button handler' task.  The eval board contains a user push button that
 * is configured to generate interrupts.  The interrupt handler uses a
 * semaphore to wake the button handler task - demonstrating how the priority
 * mechanism can be used to defer interrupt processing to the task level.  The
 * button handler task sends a message both to the LCD (via the print task) and
 * the UART where it can be viewed using a dumb terminal (via the UART to USB
 * converter on the eval board).  NOTES:  The dumb terminal must be closed in
 * order to reflash the microcontroller.  A very basic interrupt driven UART
 * driver is used that does not use the FIFO.  19200 baud is used.
 *
 * + A 'check' task.  The check task only executes every five seconds but has a
 * high priority so is guaranteed to get processor time.  Its function is to
 * check that all the other tasks are still operational and that no errors have
 * been detected at any time.  If no errors have every been detected 'PASS' is
 * written to the display (via the print task) - if an error has ever been
 * detected the message is changed to 'FAIL'.  The position of the message is
 * changed for each write.
 */



/* Environment includes. */
#include "drivers/DriverLib.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Demo app includes. */
#include "integer.h"
#include "PollQ.h"
#include "semtest.h"
#include "BlockQ.h"

/* My app includes */
#include "utils.h"
#include "sensor_task.h"
#include "filter_task.h"
#include "display_task.h"
#include "uart_cmd_task.h"
#include "top_task.h"

/* Delay between cycles of the 'check' task. */
#define mainCHECK_DELAY						( ( TickType_t ) 5000 / portTICK_PERIOD_MS )

/* UART configuration - note this does not use the FIFO so is not very
efficient. */
#define mainBAUD_RATE				( 19200 )
#define mainFIFO_SET				( 0x10 )

/* Demo task priorities. */
#define mainQUEUE_POLL_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY		( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY		( tskIDLE_PRIORITY + 2 )

/* Demo board specifics. */
#define mainPUSH_BUTTON             GPIO_PIN_4

/* Misc. */
#define mainQUEUE_SIZE				( 3 )
#define mainDEBOUNCE_DELAY			( ( TickType_t ) 150 / portTICK_PERIOD_MS )
#define mainNO_DELAY				( ( TickType_t ) 0 )
/*
 * Configure the processor and peripherals for this demo.
 */
static void prvSetupHardware( void );


/*
 * Configure the welcome message
 */
void vWelcomeMessage( void );

static volatile char *pcNextChar;

/* The semaphore used to wake the button handler task from within the GPIO
interrupt handler. */
SemaphoreHandle_t xButtonSemaphore;

/* The queue used to send strings to the print task for display on the LCD. */
QueueHandle_t xPrintQueue;

// Variables globales
// Cola para enviar los valores generados por el sensor al filtro
QueueHandle_t xTemperatureQueue;  // Cola sensor -> filtro
QueueHandle_t xFilteredTempQueue; // Cola filtro -> display

/*-----------------------------------------------------------*/

int main( void )
{
	/* Configure the clocks, UART and GPIO. */
	prvSetupHardware();

	/* Create the semaphore used to wake the button handler task from the GPIO
	ISR. */
  /* Crea un semaforo binario para el manejo del boton */
  /* El semaforo se crea estando disponible (1) */
  /* Inmediatamente se toma desde el main (0) */
  /* Esto nos permite que la tarea del boton se bloquee esperando el semaforo */
  /* Cuando hay una interrupcion provocada por la accion fisica del boton, el semaforo se pondra en (1) */
  /* La tarea del boton se desbloqueara y podra ejecutarse */
	vSemaphoreCreateBinary( xButtonSemaphore );
	xSemaphoreTake( xButtonSemaphore, 0 );

  /* Enviar mensaje de bienvenida por UART */
  vWelcomeMessage();
  
  /* Crear la cola del sensor al filtro pasa bajos */
  xTemperatureQueue = xQueueCreate( 1, sizeof(int) );
  
  /* Crear la cola del filtro al display */
  xFilteredTempQueue = xQueueCreate(10, sizeof(TempData_t));
  
  /* Verificar que las colas se hayan creado correctamente */
  if (xTemperatureQueue == NULL || xFilteredTempQueue == NULL)
  {
    UARTSendError("Error al crear la cola del sensor");
    while(1); // No podemos continuar
  }

	/* Create the queue used to pass message to vPrintTask. */
	xPrintQueue = xQueueCreate( mainQUEUE_SIZE, sizeof( char * ) );
  
  /* Start my tasks */
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
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	// Configura el PLL (Phase-Locked Loop) para el reloj del sistema
  // SYSCTL_SYSDIV_10: Divide el reloj del sistema por 10
  // SYSCTL_USE_PLL: Selecciona el PLL como fuente de reloj
  // SYSCTL_OSC_MAIN: Selecciona el oscilador principal como fuente de reloj
  // SYSCTL_XTAL_6MHZ: Selecciona un cristal de 6MHz
	SysCtlClockSet( SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ );

	// Configura el botón
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); // Habilita GPIO Puerto C
  GPIODirModeSet(GPIO_PORTC_BASE, mainPUSH_BUTTON, GPIO_DIR_MODE_IN); // Pin como entrada
	GPIOIntTypeSet( GPIO_PORTC_BASE, mainPUSH_BUTTON,GPIO_FALLING_EDGE ); // Interrupcion en flanco descendente
	IntPrioritySet( INT_GPIOC, configKERNEL_INTERRUPT_PRIORITY ); // Configura prioridad
	GPIOPinIntEnable( GPIO_PORTC_BASE, mainPUSH_BUTTON ); // Habilita interrupcion
	IntEnable( INT_GPIOC ); // Habilita interrupciones del puerto C

 	// Configura el UART
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0); // Habilita el UART0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // Habilita el GPIO Puerto A

  // Configura pines A0 y A1 para UART
	GPIODirModeSet( GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_DIR_MODE_HW );

	// Configura UART: 19233 baudios, 8 bits, sin paridad, 1 bit de parada
	UARTConfigSet( UART0_BASE, mainBAUD_RATE, UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE );

	/* We don't want to use the fifo.  This is for test purposes to generate
	as many interrupts as possible. */
	HWREG( UART0_BASE + UART_O_LCR_H ) &= ~mainFIFO_SET;

	/* Enable Tx interrupts. */
	HWREG( UART0_BASE + UART_O_IM ) |= UART_INT_TX;
	IntPrioritySet( INT_UART0, configKERNEL_INTERRUPT_PRIORITY );
	IntEnable( INT_UART0 );


	/* Initialise the LCD> */
  //OSRAMInit(false);
  //OSRAMStringDraw("TP4: RTOS", 21, 0);
  //OSRAMStringDraw("FCEFYN: SO2", 16, 1);
  //OSRAMClear();
}
/*-----------------------------------------------------------*/

void vUART_ISR(void)
{
unsigned long ulStatus;

	/* What caused the interrupt. */
	ulStatus = UARTIntStatus( UART0_BASE, pdTRUE );

	/* Clear the interrupt. */
	UARTIntClear( UART0_BASE, ulStatus );

	/* Was a Tx interrupt pending? */
	if( ulStatus & UART_INT_TX )
	{
		/* Send the next character in the string.  We are not using the FIFO. */
		if( *pcNextChar != 0 )
		{
			if( !( HWREG( UART0_BASE + UART_O_FR ) & UART_FR_TXFF ) )
			{
				HWREG( UART0_BASE + UART_O_DR ) = *pcNextChar;
			}
			pcNextChar++;
		}
	}
}
/*-----------------------------------------------------------*/

void vGPIO_ISR( void )
{
  /* Variable para indicar si se desperto una tarea de mayor prioridad */
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Limpia la bandera de interrupcion del boton */
	GPIOPinIntClear(GPIO_PORTC_BASE, mainPUSH_BUTTON);

	/* Da el semaforo desde la ISR */
  /* Si hay una tarea de mayor prioridad esperando el semaforo, cambia */
  /* xHigherPriorityTaskWoken a pdTRUE */
  /* La funcion xSemaphoreGiveFromISR se usa para liberar un semaforo desde una ISR */
	xSemaphoreGiveFromISR( xButtonSemaphore, &xHigherPriorityTaskWoken );

	/* Si se desperto una tarea de mayor prioridad, realiza el cambio de contexto */
  /* Si xHigherPriorityTaskWoken es pdTRUE: */
  /* 1. Dispara PendSV para realizar el cambio de contexto */
  /* 2. El cambio se realizará cuando se salga de la ISR */
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void vWelcomeMessage(void)
{
  UARTSend("\r\n\r\nBienvenidos al Trabajo Práctico N°4: RTOS\r\n");
  UARTSend("FCEFYN: SO2\r\n");
  UARTSend("Integrantes:\r\n");
  UARTSend("  - Franco Viotti\r\n");
  UARTSend("Algunas instrucciones: \r\n");
  UARTSend("  - Para cambiar la cantidad de muestras del filtro pasa bajos, ingrese N=X donde X es el nuevo valor\r\n\r\n\r\n");
}
