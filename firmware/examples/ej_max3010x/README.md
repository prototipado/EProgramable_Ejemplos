# Ejemplo MAX30102

Este proyecto ejemplifica el uso del dispositivo MAX30102.

En el mismo calcula cada 1 segundo la saturación de oxigeno y los latidos por minuto. El ejemplo utiliza el puerto serie virtual (USB) para la comunicación de mensajes.

## Cómo usar el ejemplo

### Hardware requerido

* ESP-EDU
* Módulo MAX30102

### Ejecutar la aplicación

1. Luego de grabar la placa, correr el `ESP-IDF. monitor Device`: ![monitor](https://raw.githubusercontent.com/microsoft/vscode-icons/2ca0f3225c1ecd16537107f60f109317fcfc3eb0/icons/dark/vm.svg)
2. Se podrá observar una salida con los valores como se muestra a continuación:

```PowerShell
.
.
.
****MAX30102 Test****
SPO2= 54, SPO2Valid= 1 
HR= 136, HRvalid= 1 
SPO2= 80, SPO2Valid= 1 
HR= 78, HRvalid= 1 
SPO2= 54, SPO2Valid= 1 
HR= -999, HRvalid= 0 
SPO2= -999, SPO2Valid= 0 
HR= 166, HRvalid= 1 
SPO2= 77, SPO2Valid= 1 
.
.
.
```
