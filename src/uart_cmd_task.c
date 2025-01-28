#include "uart_cmd_task.h"

static void vUartCmdTask(void *pvParameters) {
  char cmd[CMD_BUFFER_SIZE];
  int index = 0;
  int number = 0;
  bool isNumber = true;

  for (;;) {
    // Chequear si hay datos en el buffer de recepción
    if (!(HWREG(UART0_BASE + UART_O_FR) & UART_FR_RXFE)) {
      // Obtener desde el UART caracter por caracter
      char c = UARTCharGet(UART0_BASE);

      // Si es un retorno de carro o nueva línea, procesar comando completo
      if (c == '\r' || c == '\n') {
        if (index > 0) {
          cmd[index] = '\0'; // Terminar el string

          // Comprobar que el formato del comando sea el esperado
          if (cmd[0] == 'N' && cmd[1] == '=') {
            number = 0;  // Reiniciar el número
            isNumber = true;

            // Construcción manual del número
            for (int i = 2; cmd[i] != '\0'; i++) {
              if (cmd[i] >= '0' && cmd[i] <= '9')
                number = number * 10 + (cmd[i] - '0');
              else {
                isNumber = false;  // Caracter no válido
                break;
              }
            }

            // Enviar el numero convertido a string utilizando la variable global requestedN
            if (isNumber)
              if (number > 0)
                requestedN = number;
            else
              UARTSend("Error: Entrada no numérica\r\n");
          }
          index = 0;
        }
      }
      // Si no es un retorno de carro o nueva línea, almacenar en buffer y continuar leyendo
      else if (index < CMD_BUFFER_SIZE - 1)
        cmd[index++] = c;
    }
  }
}


void vStartUartCmdTask(void) {
  xTaskCreate(vUartCmdTask, "UARTCmd", UART_CMD_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
}
