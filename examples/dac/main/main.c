#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/dac_oneshot.h"
#include "esp_rom_sys.h"

void app_main(void)
{
    dac_oneshot_handle_t dac;
    const dac_oneshot_config_t config = {.chan_id = DAC_CHAN_0};
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&config, &dac)); //Habilita o DAC1

    while (1)
    {
        //Gera a forma de onda 'dente de serra'
        for (int i = 0; i < 256; i++)
        {
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(dac, i));
            esp_rom_delay_us(5);
        }
    }
}
