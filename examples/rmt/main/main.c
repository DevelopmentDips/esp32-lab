#include "driver/rmt_tx.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void rgb(rmt_channel_handle_t channel, rmt_encoder_handle_t encoder,
                uint8_t red, uint8_t green, uint8_t blue)
{
    const uint8_t grb[] = {green, red, blue};
    const rmt_transmit_config_t config = {0};
    ESP_ERROR_CHECK(rmt_transmit(channel, encoder, grb, sizeof(grb), &config));
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(channel, 100));
}

void app_main(void)
{
    rmt_channel_handle_t channel;
    const rmt_tx_channel_config_t config = {
        .gpio_num = 18,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 20000000,
        .mem_block_symbols = 64,
        .trans_queue_depth = 1,
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&config, &channel));

    rmt_encoder_handle_t encoder;
    const rmt_bytes_encoder_config_t bits = {
        .bit0 = {.level0 = 1, .duration0 = 7, .level1 = 0, .duration1 = 16},
        .bit1 = {.level0 = 1, .duration0 = 14, .level1 = 0, .duration1 = 12},
        .flags.msb_first = 1,
    };
    ESP_ERROR_CHECK(rmt_new_bytes_encoder(&bits, &encoder));
    ESP_ERROR_CHECK(rmt_enable(channel));

    rgb(channel, encoder, 255, 0, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
    rgb(channel, encoder, 0, 255, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
    rgb(channel, encoder, 0, 0, 255);
    vTaskDelay(pdMS_TO_TICKS(1000));
    rgb(channel, encoder, 0, 0, 0);
}
