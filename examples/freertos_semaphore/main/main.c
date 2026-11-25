#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <freertos/semphr.h>


SemaphoreHandle_t SMF;

//Tarefa responsavel por liberar o semaforo
void task(void*z)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(2000));
        ESP_LOGI(__func__, "Liberando el recurso");
        xSemaphoreGive(SMF); //Libera o semaforo
    }
}

void app_main(void)
{
    SMF = xSemaphoreCreateBinary(); //Cria o semaforo
    xTaskCreatePinnedToCore(task, "task", 4096, NULL, 1, NULL, 0);

    while (1)
    {
        //Tenta obter o semaforo durante 500ms
        if (xSemaphoreTake(SMF, pdMS_TO_TICKS(500)))
        {
            //Sucesso ao obter dentro do tempo maximo
            ESP_LOGI(__func__, "Recurso obtenido");
        }
        else
        {
            //Falha ao obter dentro do tempo maximo
            ESP_LOGE(__func__, "No se pudo obtener el recurso");
        }
    }
}