#ifndef UART_CMD_TASK_H
#define UART_CMD_TASK_H

#include "FreeRTOS.h"
#include "filter_task.h"
#include "DriverLib.h"

#define CMD_BUFFER_SIZE 10

void vStartUartCmdTask(void);

#endif