/*
 * Scheduler.h
 *
 *  Created on: Jun 11, 2020
 *      Author: tuan.phan-huynh
 */

#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "Scheduler_Type.h"

#ifdef __cplusplus
extern "C" {
#endif  

/*******************************************************************************
 * Macro definition
 ******************************************************************************/
#define MAX_TASK                5U

/*******************************************************************************
 * Type definition
 ******************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/
extern void Scheduler_Init(void);
extern SchedulerReturnType Scheduler_AddTask(TaskType pTaskFunc, uint32_t period);
extern SchedulerReturnType Scheduler_Dispatcher(void);
extern void Scheduler_SynTick(void);

#ifdef __cplusplus
}
#endif  

#endif /* _SCHEDULER_H_ */
