#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    adc_oneshot_unit_handle_t adc;
    const adc_oneshot_unit_init_cfg_t unit = {.unit_id = ADC_UNIT_1};
    const adc_oneshot_chan_cfg_t channel = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit, &adc));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, ADC_CHANNEL_0, &channel));

    adc_cali_handle_t calibration;
    const adc_cali_line_fitting_config_t config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
        .default_vref = 1100,
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&config, &calibration));

    while (1) {
        int total = 0;
        for (int i = 0; i < 100; ++i) {
            int raw;
            ESP_ERROR_CHECK(adc_oneshot_read(adc, ADC_CHANNEL_0, &raw));
            total += raw;
            esp_rom_delay_us(30);
        }
        int voltage;
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(calibration, total / 100, &voltage));
        ESP_LOGI(__func__, "Read mV: [%d]", voltage);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
