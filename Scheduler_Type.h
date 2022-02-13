/*
 * Scheduler_Type.h
 *
 *  Created on: Jun 11, 2020
 *      Author: tuan.phan-huynh
 */

#ifndef TASKSCHEDULER_SCHEDULER_TYPE_H_
#define TASKSCHEDULER_SCHEDULER_TYPE_H_

#include <stdint.h>

typedef void (*TaskType)(void);

typedef enum
{
    SCH_FAILED  = 0U,
    SCH_SUCCESS = 1U,
} SchedulerReturnType;

typedef enum
{
    DEACTIVE    = 0U,
    ACTIVE      = 1U
} RunType;

typedef struct
{
    TaskType pTask;
    uint32_t ulPeriod;
    uint32_t ulTickCount;
    RunType enRunStatus;
} TaskSchedulerType;

#endif /* TASKSCHEDULER_SCHEDULER_TYPE_H_ */
