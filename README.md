# esp32-lab

## Ejemplos del libro *ESP32 com IDF*

Los ejemplos de José V. S. Morais están en el [repositorio oficial del libro](https://github.com/urbanze/livro-esp32-pro). Cada carpeta de `examples/` que contiene un proyecto se compila por separado.

Con el entorno de ESP-IDF activado, podés empezar por el ejemplo de C++ en un ESP32 clásico:

```bash
git clone https://github.com/urbanze/livro-esp32-pro.git
cd livro-esp32-pro/examples/cpp
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Reemplazá `/dev/ttyUSB0` por el puerto de tu placa. El ejemplo muestra `C++ com ESP32` por la consola serie. Para salir del monitor, usá `Ctrl+]`.

Los ejemplos originales se escribieron para ESP-IDF 4.4. Si usás ESP-IDF 6.1.1, algunos requieren adaptar el código.
