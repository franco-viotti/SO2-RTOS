#include "uart_cmd_task.h"

static void vUartCmdTask(void *pvParameters)
{
  char cmd[CMD_BUFFER_SIZE];
  int index = 0;
  int number = 0;
  bool isNumber = true;
  
  /* MONITOREO DE STACK */
  UBaseType_t uxHighWater = uxTaskGetStackHighWaterMark(NULL);
  char str[10];
  int_to_string(uxHighWater, str);
  UARTSend("[INFO] Stack inicial para la tarea vUartCmdTask: ");
  UARTSend(str);
  UARTSend("\n\r");
  /* MONITOREO DE STACK */

  for (;;)
  {

    /* MONITOREO DE STACK */
    UBaseType_t uxHighWater = uxTaskGetStackHighWaterMark(NULL);
    int_to_string(uxHighWater, str);
    UARTSend("[INFO] Stack al inicio del bucle for para la tarea vUartCmdTask: ");
    UARTSend(str);
    UARTSend("\n\r");
    /* MONITOREO DE STACK */

    // Chequear si hay datos en el buffer de recepción
    if (!(HWREG(UART0_BASE + UART_O_FR) & UART_FR_RXFE))
    {
      // Obtener desde el UART caracter por caracter
      char c = UARTCharGet(UART0_BASE);

      /* MONITOREO DE STACK */
      UBaseType_t uxHighWater = uxTaskGetStackHighWaterMark(NULL);
      int_to_string(uxHighWater, str);
      UARTSend("[INFO] Stack luego de obtener el caracter para la tarea vUartCmdTask: ");
      UARTSend(str);
      UARTSend("\n\r");
      /* MONITOREO DE STACK */

      // Si es un retorno de carro o nueva línea, procesar comando completo
      if (c == '\r' || c == '\n')
      {

        /* MONITOREO DE STACK */
        UBaseType_t uxHighWater = uxTaskGetStackHighWaterMark(NULL);
        int_to_string(uxHighWater, str);
        UARTSend("[INFO] Stack en el inicio de procesamiento para la tarea vUartCmdTask: ");
        UARTSend(str);
        UARTSend("\n\r");
        /* MONITOREO DE STACK */

        if (index > 0)
        {
          cmd[index] = '\0'; // Terminar el string

          // Comprobar que el formato del comando sea el esperado
          if (cmd[0] == 'N' && cmd[1] == '=')
          {
            number = 0;  // Reiniciar el número
            isNumber = true;

            // Construcción manual del número
            for (int i = 2; cmd[i] != '\0'; i++)
            {

              /* MONITOREO DE STACK */
              UBaseType_t uxHighWater = uxTaskGetStackHighWaterMark(NULL);
              int_to_string(uxHighWater, str);
              UARTSend("[INFO] Stack durante la construccion manual del numero para la tarea vUartCmdTask: ");
              UARTSend(str);
              UARTSend(" iteracion: ");
              int_to_string(i, str);
              UARTSend(str);
              UARTSend("\n\r");
              /* MONITOREO DE STACK */

              if (cmd[i] >= '0' && cmd[i] <= '9')
              {
                number = number * 10 + (cmd[i] - '0');
              }
              else
              {
                isNumber = false;  // Caracter no válido
                break;
              }
            }

            // Enviar el numero convertido a string utilizando la variable global requestedN
            if (isNumber)
            {
              if (number > 0)
              {  
                requestedN = number;
              }
            }
            else
            {
              UARTSend("Error: Entrada no numérica\r\n");
            }
          }
          index = 0;
        }
      }
      // Si no es un retorno de carro o nueva línea, almacenar en buffer y continuar leyendo
      else if (index < CMD_BUFFER_SIZE - 1)
      {
        cmd[index++] = c;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


void vStartUartCmdTask(void)
{
  xTaskCreate(vUartCmdTask, "UARTCmd", UART_CMD_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
}
