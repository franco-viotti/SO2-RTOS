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