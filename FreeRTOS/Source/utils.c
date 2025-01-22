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
  // Limpiar el string que viene como parámetro
  memset(str, 0, sizeof(str));
  
  // Para números de dos dígitos (TEMP_MIN-TEMP_MAX)
  str[0] = '0' + (value / 10);
  str[1] = '0' + (value % 10);
  str[2] = '\0';
}