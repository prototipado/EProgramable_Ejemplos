# Ejemplo DSP

Este proyecto ejemplifica el uso de las funcionalidades para Procesamiento Digital de Señales (DSP).

En el mismo se toma una señal de ECG almacenada en memoria y se la utiliza para probar las funcionalidades de creación y utilización de filtros digitales, así como las funcionalidades para el cálculo de la Transformada Rápida de Fourier (FFT) de dicha señal.

## Cómo usar el ejemplo

### Hardware requerido

* ESP-EDU

### Configurar el proyecto

Para utilziar las funcionalidades de DSP es necesario agregar la capa middelware al proyecto, modificando el archivo CMakeLists.txt (ubicado en la raiz del proyecto) con el siguiente contenido:

```cmake
cmake_minimum_required(VERSION 3.16)

list(APPEND EXTRA_COMPONENT_DIRS "../../drivers")
list(APPEND EXTRA_COMPONENT_DIRS "../../middelware")

include_directories(${PROJECT_NAME} ../../drivers)
include_directories(${PROJECT_NAME} ../../middelware)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ej_dsp)
```

En este proyecto ya ha sido agregado, por lo tanto no es necesaria ninguna acción extra para probarlo.

### Ejecutar la aplicación

1. Luego de grabar la placa, correr el `ESP-IDF. monitor Device`: ![monitor](https://raw.githubusercontent.com/microsoft/vscode-icons/2ca0f3225c1ecd16537107f60f109317fcfc3eb0/icons/dark/vm.svg)
2. Se podrá observar una salida con los valores de ECG filtrado y su FFT, como se muestra a continuación:

```PowerShell
.
.
.
****Filtros****
ECG, ECG_filtrado
76.000000, 3.398153
76.000000, 19.369621
77.000000, 48.600708
77.000000, 71.989883
76.000000, 72.831650
83.000000, 58.618057
85.000000, 47.453522
78.000000, 45.676758
76.000000, 45.785522
85.000000, 40.570312
93.000000, 33.229980
85.000000, 30.598633
79.000000, 31.145996
86.000000, 27.710449
93.000000, 20.131592
93.000000, 15.530273
85.000000, 16.065430
87.000000, 15.621582
94.000000, 10.343262
98.000000, 4.736816
93.000000, 3.771484
87.000000, 4.625977
95.000000, 1.600586
104.000000, -4.077148
.
.
.
```
