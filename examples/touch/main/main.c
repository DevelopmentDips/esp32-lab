#include <inttypes.h>

#include "driver/touch_sens.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    touch_sensor_sample_config_t sample =
        TOUCH_SENSOR_V1_DEFAULT_SAMPLE_CONFIG(5.0, TOUCH_VOLT_LIM_L_0V5, TOUCH_VOLT_LIM_H_1V7);
    const touch_sensor_config_t config = TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(1, &sample);
    touch_sensor_handle_t sensor;
    ESP_ERROR_CHECK(touch_sensor_new_controller(&config, &sensor));

    const touch_channel_config_t channel_config = {
        .abs_active_thresh = {0},
        .charge_speed = TOUCH_CHARGE_SPEED_7,
        .init_charge_volt = TOUCH_INIT_CHARGE_VOLT_DEFAULT,
        .group = TOUCH_CHAN_TRIG_GROUP_BOTH,
    };
    touch_channel_handle_t channel;
    ESP_ERROR_CHECK(touch_sensor_new_channel(sensor, 0, &channel_config, &channel));
    ESP_ERROR_CHECK(touch_sensor_enable(sensor));

    while (1) {
        uint32_t value;
        ESP_ERROR_CHECK(touch_sensor_trigger_oneshot_scanning(sensor, 100));
        ESP_ERROR_CHECK(touch_channel_read_data(channel, TOUCH_CHAN_DATA_TYPE_RAW, &value));
        ESP_LOGI(__func__, "Touch: [%" PRIu32 "]", value);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
