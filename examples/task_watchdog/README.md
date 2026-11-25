# Task Watchdog
Demonstração e utilização do Task Watchdog com ESP32.

## Saída de informações
```
I (343) app_main: El bucle 1 no reiniciará el ESP32
W (5347) app_main: El bucle 2 reiniciará el ESP32
E (7346) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (7346) task_wdt:  - main (CPU 0)
E (7346) task_wdt: Tasks currently running:
E (7346) task_wdt: CPU 0: IDLE
E (7346) task_wdt: CPU 1: IDLE
E (7346) task_wdt: Aborting.

... (log de boot omitido)
```