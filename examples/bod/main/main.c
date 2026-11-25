#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_private/rtc_ctrl.h"
#include "soc/rtc_cntl_reg.h"
#include <stdlib.h>

void IRAM_ATTR bod_isr(void*z)
{
    //Limpa o bit de BOD_INT
    REG_WRITE(RTC_CNTL_INT_CLR_REG, RTC_CNTL_BROWN_OUT_INT_CLR_M);

    //Faz algo especifico do usuario
    for (int i = 0; i < 5; i++)
    {
        ESP_EARLY_LOGE(__func__, "BOD Trigged [%d]", i);
    }
    
    //Aborta (reinicia) o sistema
    abort();
}

void app_main(void)
{
    //Registra a funcao de ISR para o bit 7 (BOD_INT) do reg. 0x3FF4803C
    ESP_ERROR_CHECK(rtc_isr_register(bod_isr, NULL, RTC_CNTL_BROWN_OUT_INT_ENA_M, RTC_INTR_FLAG_IRAM));
}
