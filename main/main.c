#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

static const char *TAG = "esp32_lab";
static const gpio_num_t onboard_led = GPIO_NUM_2;
static httpd_handle_t server;

#define LED_BRIGHTNESS_PATH "/led/brillo/"

enum {
    LED_OFF = 0,
    LED_ON = 100,
    LED_BLINK = 101,
    LED_BRIGHTNESS_FROM_URI,
};

static _Atomic int led_setting = LED_BLINK;
static TaskHandle_t led_task;

static esp_err_t handle_led(httpd_req_t *request)
{
    if (request->method == HTTP_POST) {
        int setting = (int)(uintptr_t)request->user_ctx;
        if (setting == LED_BRIGHTNESS_FROM_URI) {
            const char *value = request->uri + sizeof(LED_BRIGHTNESS_PATH) - 1;
            const size_t length = strspn(value, "0123456789");
            if (length == 0 || length > 3 || value[length] != '\0' ||
                (setting = atoi(value)) > LED_ON) {
                return httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST,
                                           "El brillo debe ser un entero de 0 a 100");
            }
        }
        atomic_store(&led_setting, setting);
        xTaskNotifyGive(led_task);
    }

    const int setting = atomic_load(&led_setting);
    httpd_resp_set_type(request, "application/json");
    if (setting == LED_BLINK) {
        return httpd_resp_sendstr(request, "{\"modo\":\"parpadeo\"}");
    }

    char body[48];
    snprintf(body, sizeof(body), "{\"modo\":\"%s\",\"brillo\":%d}",
             setting == LED_OFF ? "apagado" : "encendido", setting);
    return httpd_resp_sendstr(request, body);
}

static esp_err_t get_button(httpd_req_t *request)
{
    const char *body = gpio_get_level(GPIO_NUM_0) == 0
        ? "{\"pulsado\":true}"
        : "{\"pulsado\":false}";
    httpd_resp_set_type(request, "application/json");
    return httpd_resp_sendstr(request, body);
}

static void start_http_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    const httpd_uri_t routes[] = {
        { .uri = "/boton", .method = HTTP_GET, .handler = get_button },
        { .uri = "/led", .method = HTTP_GET, .handler = handle_led },
        { .uri = "/led/apagar", .method = HTTP_POST, .handler = handle_led,
          .user_ctx = (void *)(uintptr_t)LED_OFF },
        { .uri = "/led/encender", .method = HTTP_POST, .handler = handle_led,
          .user_ctx = (void *)(uintptr_t)LED_ON },
        { .uri = LED_BRIGHTNESS_PATH "*", .method = HTTP_POST, .handler = handle_led,
          .user_ctx = (void *)(uintptr_t)LED_BRIGHTNESS_FROM_URI },
        { .uri = "/led/parpadear", .method = HTTP_POST, .handler = handle_led,
          .user_ctx = (void *)(uintptr_t)LED_BLINK },
    };

    esp_err_t error = httpd_start(&server, &config);
    if (error != ESP_OK) {
        ESP_LOGE(TAG, "Error al iniciar HTTP: %s", esp_err_to_name(error));
        return;
    }

    for (size_t i = 0; i < sizeof(routes) / sizeof(routes[0]); ++i) {
        error = httpd_register_uri_handler(server, &routes[i]);
        if (error != ESP_OK) {
            ESP_LOGE(TAG, "Error al registrar %s: %s", routes[i].uri, esp_err_to_name(error));
            httpd_stop(server);
            server = NULL;
            return;
        }
    }
}

static void network_event(void *arg, esp_event_base_t base, int32_t id,
                          void *data)
{
    if (base == WIFI_EVENT &&
        (id == WIFI_EVENT_STA_START || id == WIFI_EVENT_STA_DISCONNECTED)) {
        esp_err_t error = esp_wifi_connect();
        if (error != ESP_OK) {
            ESP_LOGW(TAG, "Error al conectar el Wi-Fi: %s", esp_err_to_name(error));
        }
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        const ip_event_got_ip_t *event = data;
        ESP_LOGI(TAG, "IP: " IPSTR, IP2STR(&event->ip_info.ip));
        if (server == NULL) {
            start_http_server();
        }
    }
}

void app_main(void)
{
    const ledc_timer_config_t led_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&led_timer));
    const ledc_channel_config_t led_channel = {
        .gpio_num = onboard_led,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&led_channel));

    const gpio_config_t button_config = {
        .pin_bit_mask = 1ULL << GPIO_NUM_0,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&button_config));

    static const char ssid[] = CONFIG_LAB_WIFI_SSID;
    static const char password[] = CONFIG_LAB_WIFI_PASSWORD;
    wifi_config_t wifi_config = {0};
    if (sizeof(ssid) == 1 || sizeof(ssid) - 1 > sizeof(wifi_config.sta.ssid) ||
        sizeof(password) - 1 > sizeof(wifi_config.sta.password) ||
        (sizeof(password) > 1 && sizeof(password) < 9)) {
        ESP_LOGE(TAG, "Configura un SSID y una contrasena Wi-Fi validos con menuconfig");
        return;
    }
    memcpy(wifi_config.sta.ssid, ssid, sizeof(ssid) - 1);
    memcpy(wifi_config.sta.password, password, sizeof(password) - 1);

    esp_err_t error = nvs_flash_init();
    if (error == ESP_ERR_NVS_NO_FREE_PAGES || error == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        error = nvs_flash_init();
    }
    ESP_ERROR_CHECK(error);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_init = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                               network_event, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                               network_event, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    led_task = xTaskGetCurrentTaskHandle();
    ESP_ERROR_CHECK(esp_wifi_start());

    const uint32_t full_duty = 1U << LEDC_TIMER_10_BIT;
    uint32_t blink_duty = 0;
    while (true) {
        const int setting = atomic_load(&led_setting);
        const uint32_t duty = setting == LED_BLINK
            ? blink_duty
            : (full_duty * setting + LED_ON / 2) / LED_ON;
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

        if (setting == LED_BLINK) {
            if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)) == 0) {
                blink_duty ^= full_duty;
            }
        } else {
            blink_duty = full_duty;
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }
    }
}
