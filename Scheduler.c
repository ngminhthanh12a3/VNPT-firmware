/*
 * Scheduler.c
 *
 *  Created on: Jun 11, 2020
 *      Author: tuan.phan-huynh
 */

#include "Scheduler.h"

/*******************************************************************************
 * Function declaration
 ******************************************************************************/
void Scheduler_SynTick(void);
uint32_t Scheduler_GetElapseTick(uint32_t startTick, uint32_t endTick);

TaskSchedulerType gstSchedulerTask[MAX_TASK];
uint8_t gucNumTask;
static volatile uint32_t gulSchedulerTickCount;

void Scheduler_Init(void)
{
    uint8_t lucIndex;
    lucIndex = 0;
    gucNumTask = 0;
    for (lucIndex = 0; lucIndex < MAX_TASK; lucIndex++)
    {
        gstSchedulerTask[lucIndex].pTask = 0;
        gstSchedulerTask[lucIndex].ulPeriod = 0;
        gstSchedulerTask[lucIndex].ulTickCount = 0;
        gstSchedulerTask[lucIndex].enRunStatus = DEACTIVE;
    }
}

SchedulerReturnType Scheduler_AddTask(TaskType pTaskFunc, uint32_t period)
{
    SchedulerReturnType lenReturnValue;
    lenReturnValue = SCH_SUCCESS;
    if (MAX_TASK <= gucNumTask)
    {
        lenReturnValue = SCH_FAILED;
    }
    else
    {
        gstSchedulerTask[gucNumTask].pTask = pTaskFunc;
        gstSchedulerTask[gucNumTask].ulPeriod = period;
        gstSchedulerTask[gucNumTask].enRunStatus = ACTIVE;
        gucNumTask++;
    }

    return lenReturnValue;
}

SchedulerReturnType Scheduler_Dispatcher(void)
{
    uint8_t lucIndex;
    lucIndex = 0;
    SchedulerReturnType lenReturnValue;
    uint32_t lulElapse;

    lenReturnValue = SCH_SUCCESS;

    if (0 == gucNumTask)
    {
        lenReturnValue = SCH_FAILED;
    }
    else
    {
        for (lucIndex = 0; lucIndex < gucNumTask; lucIndex++)
        {
            lulElapse = Scheduler_GetElapseTick(gstSchedulerTask[lucIndex].ulTickCount, gulSchedulerTickCount);
            if ((ACTIVE == gstSchedulerTask[lucIndex].enRunStatus) && (lulElapse > gstSchedulerTask[lucIndex].ulPeriod) )
            {
                gstSchedulerTask[lucIndex].ulTickCount = gulSchedulerTickCount;
                gstSchedulerTask[lucIndex].pTask();
//                gstSchedulerTask[lucIndex].enRunStatus = DEACTIVE;
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    return lenReturnValue;
}

void Scheduler_SynTick(void)
{
    gulSchedulerTickCount++;
}

uint32_t Scheduler_GetElapseTick(uint32_t startTick, uint32_t endTick)
{
    uint32_t lulElapseTick;
    lulElapseTick = 0;
    if (endTick > startTick)
    {
        lulElapseTick = endTick - startTick;
    }
    else
    {
        lulElapseTick = (0xFFFFFFFFUL - startTick) + (endTick + 1);
    }
    return lulElapseTick;
}
