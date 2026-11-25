#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <freertos/timers.h>

// Callback executado pela tarefa de serviço dos timers.
static void timer_callback(TimerHandle_t timer_id)
{
    //Obtem o ID do timer que ocorreu o estouro
    uint32_t id = (uint32_t) pvTimerGetTimerID(timer_id);

    if (id == 1)
    {
        puts("Timer X");
    }
    else if (id == 2)
    {
        puts("Timer Y");
    }
}

void app_main(void)
{
    //Cria os timers
    TimerHandle_t tmr_x = xTimerCreate("tmr_x", pdMS_TO_TICKS(500),  true, (void*)1, timer_callback);
    TimerHandle_t tmr_y = xTimerCreate("tmr_y", pdMS_TO_TICKS(1000), true, (void*)2, timer_callback);

    //Inicia os timers
    xTimerStart(tmr_x, pdMS_TO_TICKS(1000));
    xTimerStart(tmr_y, pdMS_TO_TICKS(1000));
}
