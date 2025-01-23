#include "uart_cmd_task.h"
#include <stdbool.h>

/*static void vUartCmdTask(void *pvParameters)
{
  char cmd[CMD_BUFFER_SIZE];
  int index = 0;
  
  for(;;)
  {
    UARTSend("Ingrese un comando con el formato N=X: ");
    UARTSend("\r\n");
    
    // Se verifica si hay un caracter disponible sin bloquear la tarea
    if(!(HWREG(UART0_BASE + UART_O_FR) & UART_FR_RXFE))
    {
      char c = UARTCharGet(UART0_BASE);
      UARTSend("Recibido: ");
      UARTCharPut(UART0_BASE, c);
      UARTSend("\r\n");
      UARTSend("Buffer: ");
      UARTSend(cmd);
      UARTSend("\r\n");
      
      if(c == '\r' || c == '\n')
      {
        if(index > 0)
        {
          UARTSend("Comando recibido LIMPIO: ");
          cmd[index] = '\0';
          UARTSend(cmd);
          UARTSend("\r\n");
          // Procesar comando "N=X" donde X es el nuevo valor
          if(cmd[0] == 'N' && cmd[1] == '=')
          {

            UARTSend("Procesando comando N=X\r\n");  
            
            char testStr[] = "4";
            char numStr[16];
            UARTSend("Prueba atoi con \"4\": ");
            int testValue = atoi(testStr);
            int_to_string(testValue, numStr);
            UARTSend(numStr);
            UARTSend("\r\n");;

            UARTSend("Valor recibido: ");
            UARTSend(&cmd[2]);
            UARTSend("\r\n");


            int newN = atoi(&cmd[2]);
            UARTSend("Conversion terminada\r\n");
            UARTSend("Nuevo N: ");
            int_to_string(newN, cmd);
            UARTSend(cmd);
            UARTSend("\r\n");
            if(newN > 0)
            {
              currentN = newN;
              UARTSend("Nuevo N: ");
              int_to_string(currentN, cmd);
              UARTSend(cmd);
              UARTSend("\r\n");
            }
          }
          index = 0;
        }
      }
      else if(index < CMD_BUFFER_SIZE - 1)
      {
        cmd[index++] = c;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}*/

static void vUartCmdTask(void *pvParameters)
{
  char cmd[CMD_BUFFER_SIZE];
  int index = 0;
  int number = 0;
  bool isNumber = true;

  for (;;)
  {
    if (!(HWREG(UART0_BASE + UART_O_FR) & UART_FR_RXFE))
    {
      char c = UARTCharGet(UART0_BASE);
      UARTSend("Recibido: ");
      UARTCharPut(UART0_BASE, c);
      UARTSend("\r\n");

      if (c == '\r' || c == '\n')
      {
        if (index > 0)
        {
          cmd[index] = '\0';
          UARTSend("Comando recibido: ");
          UARTSend(cmd);
          UARTSend("\r\n");

          if (cmd[0] == 'N' && cmd[1] == '=')
          {
            number = 0;  // Reiniciar el número
            isNumber = true;

            // Construcción manual del número
            for (int i = 2; cmd[i] != '\0'; i++)
            {
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

            if (isNumber)
            {
              UARTSend("Número convertido: ");
              char numStr[16];
              int_to_string(number, numStr);
              UARTSend(numStr);
              UARTSend("\r\n");

              if (number > 0)
              {
                if (xSemaphoreTake(xNMutex, 0) == pdPASS)
                {  
                  requestedN = number;
                  xSemaphoreGive(xNMutex);
                }
                else
                {
                  UARTSendError("Error al tomar el semáforo xNMutex\r\n");
                }
                UARTSend("Nuevo N almacenado: ");
                int_to_string(currentN, numStr);
                UARTSend(numStr);
                UARTSend("\r\n");
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
  xTaskCreate(vUartCmdTask, "UARTCmd", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
}
