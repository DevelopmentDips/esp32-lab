idf_component_register(SRCS "main.c"
                       PRIV_REQUIRES esp_driver_gpio esp_driver_ledc esp_event esp_http_server
                                     esp_netif esp_wifi nvs_flash)
