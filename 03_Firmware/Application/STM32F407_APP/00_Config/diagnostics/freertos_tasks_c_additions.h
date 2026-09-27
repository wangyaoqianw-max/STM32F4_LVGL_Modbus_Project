#ifndef FREERTOS_TASKS_C_ADDITIONS_H
#define FREERTOS_TASKS_C_ADDITIONS_H

#if (configRECORD_STACK_HIGH_ADDRESS != 1)
#error "CmBacktrace requires configRECORD_STACK_HIGH_ADDRESS == 1"
#endif

/* 此文件由 FreeRTOS tasks.c 扩展点包含，可访问当前任务 TCB。 */
uint32_t *vTaskStackAddr(void)
{
    return pxCurrentTCB->pxStack;
}

uint32_t vTaskStackSize(void)
{
    return (uint32_t)(pxCurrentTCB->pxEndOfStack - pxCurrentTCB->pxStack + 1);
}

char *vTaskName(void)
{
    return pxCurrentTCB->pcTaskName;
}

#endif
