#ifndef UTILS_H
#define UTILS_H

#include "DriverLib.h"

// Función para enviar strings por UART
void UARTSend(const char *str);

// Función para enviar mensajes de error
void UARTSendError(const char *errorMsg);

#endif /* UTILS_H */