#include "utils.h"

void UARTSend(const char *str)
{
  while(*str)
  {
    UARTCharPut(UART0_BASE, *str++);
  }
}

void UARTSendError(const char *errorMsg)
{
  UARTSend("[ERROR] ");
  UARTSend(errorMsg);
  UARTSend("\r\n");
}

void int_to_string(int value, char* str) 
{
  // Para números de dos dígitos (TEMP_MIN-TEMP_MAX)
  str[0] = '0' + (value / 10);
  str[1] = '0' + (value % 10);
  str[2] = '\0';
}

void long_to_string(long value, char* str) 
{
  int i = 0;
  
  // Manejar caso especial de 0
  if (value == 0) {
    str[0] = '0';
    str[1] = '\0';
    return;
  }
  
  // Convertir dígitos
  while (value > 0) {
    str[i++] = '0' + (value % 10);
    value = value / 10;
  }
  
  // Agregar terminador de string
  str[i] = '\0';
  
  // Revertir el string
  int len = i;
  for (i = 0; i < len/2; i++) {
    char temp = str[i];
    str[i] = str[len-1-i];
    str[len-1-i] = temp;
  }
}