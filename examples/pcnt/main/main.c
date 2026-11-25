#include <stdint.h>

#include "driver/pulse_cnt.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    pcnt_unit_handle_t unit;
    const pcnt_unit_config_t limits = {.low_limit = INT16_MIN, .high_limit = INT16_MAX};
    ESP_ERROR_CHECK(pcnt_new_unit(&limits, &unit));

    pcnt_channel_handle_t channel;
    const pcnt_chan_config_t pins = {.edge_gpio_num = 0, .level_gpio_num = -1};
    ESP_ERROR_CHECK(pcnt_new_channel(unit, &pins, &channel));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(channel, PCNT_CHANNEL_EDGE_ACTION_HOLD,
                                                PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    const pcnt_glitch_filter_config_t filter = {.max_glitch_ns = 12780};
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(unit, &filter));
    ESP_ERROR_CHECK(pcnt_unit_enable(unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(unit));
    ESP_ERROR_CHECK(pcnt_unit_start(unit));

    while (1) {
        int count;
        ESP_ERROR_CHECK(pcnt_unit_get_count(unit, &count));
        ESP_ERROR_CHECK(pcnt_unit_clear_count(unit));
        ESP_LOGI(__func__, "Ctr: [%d]", count);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
