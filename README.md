# esp32-lab

## Ejemplos del libro *ESP32 com IDF*

Los ejemplos de José V. S. Morais provienen del [repositorio oficial del libro](https://github.com/urbanze/livro-esp32-pro). Esta copia contiene las adaptaciones para el ESP32 clásico con ESP-IDF 6.1.1. Cada carpeta de `examples/` que contiene un proyecto se compila por separado.

Con el entorno de ESP-IDF activado, podés empezar por el ejemplo de C++ en un ESP32 clásico:

```bash
git clone https://github.com/DevelopmentDips/esp32-lab.git
cd esp32-lab/examples/cpp
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Reemplazá `/dev/ttyUSB0` por el puerto de tu placa. El ejemplo muestra `C++ com ESP32` por la consola serie. Para salir del monitor, usá `Ctrl+]`.

Para los ejemplos de red, configurá el SSID y la contraseña de tu red local. MQTT también requiere configurar el servidor y descargar el componente `espressif/mqtt` en la primera compilación.
