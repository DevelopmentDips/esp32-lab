#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_task_wdt.h"

void app_main(void)
{
    //Inicia o TWDT (apenas 1x)
    const esp_task_wdt_config_t config = {.timeout_ms = 2000, .trigger_panic = true};
    esp_err_t err = esp_task_wdt_init(&config);
    if (err == ESP_ERR_INVALID_STATE) {
        err = esp_task_wdt_reconfigure(&config);
    }
    ESP_ERROR_CHECK(err);

    //Adiciona essa task (main) ao TWDT
    esp_task_wdt_add(NULL);


    //Loop1, que nao ocasiona o reinicio
    ESP_LOGI(__func__, "El bucle 1 no reiniciará el ESP32");
    for (int i = 0; i < 10; i++)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
        esp_task_wdt_reset(); //Alimenta o TWDT
    }

    //Loop2, que ocasiona o reinicio
    ESP_LOGW(__func__, "El bucle 2 reiniciará el ESP32");
    for (int i = 0; i < 10; i++)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}