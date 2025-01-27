#ifndef TOP_TASK_H
#define TOP_TASK_H

#include "FreeRTOS.h"
#include "filter_task.h"
#include "task.h"
#include "DriverLib.h"
#include "utils.h"
#include <stdio.h>
#include <stdbool.h>

#define TOP_DELAY_MS 1000

void vStartTopTask(void);

#endif