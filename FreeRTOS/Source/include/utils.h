#ifndef UTILS_H
#define UTILS_H

#include "DriverLib.h"
#include  <string.h>

// Función para enviar strings por UART
void UARTSend(const char *str);

// Función para enviar mensajes de error
void UARTSendError(const char *errorMsg);

// Función para convertir int a string
void int_to_string(int value, char* str);

#endif /* UTILS_H */