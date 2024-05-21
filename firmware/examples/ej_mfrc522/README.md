# Ejemplo MRFC522

Este proyecto ejemplifica el uso del dispositivo MRFC522.

En el mismo se pregunta cada 1 segundo por la presencia de algún tag. De acercarse alguno se podrá ver por el terminal el ID del mismo. El ejemplo utiliza el puerto serie (UART_PC) para la comunicación de mensajes.

## Cómo usar el ejemplo

### Hardware requerido

* ESP-EDU
* Módulo RFID-RC522
* Tags

### Ejecutar la aplicación

1. Luego de grabar la placa, correr el `Serial Monitor`
2. Se podrá observar una salida con los valores como se muestra a continuación:

```PowerShell
.
.
.
Reading... 

Card uid bytes:  0Xf6  0X84  0X19  0X9e 
Card Read user ID: 4135852446
Reading... 
Reading...
Reading... 
Reading... 
Reading... 

Card uid bytes:  0X83  0X33  0Xbd  0Xb 
Card Read user ID: 2201206027
Reading... 
.
.
.
```