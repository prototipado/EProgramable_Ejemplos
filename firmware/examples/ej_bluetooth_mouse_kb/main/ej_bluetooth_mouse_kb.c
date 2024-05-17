/*! @mainpage Ejemplo de uso del ESP32 como teclado o mouse bluettoth.
 *
 * @section genDesc General Description
 *
 * Este proyecto ejemplifica el uso del Bluetooth Low Energy para simular un dispositivo HID.
 * A partir de las señales generadas por el joystick analógico, emula un mouse (movimiento y 
 * click izquierdo). Además emula las teclas "barra espaciadora" y "flecha abajo" a partir 
 * de las teclas de la ESP-EDU (teclas "TECLA_1 y "TECLA_2" respectivamente).
 *
 * @section hardConn Hardware Connection
 *
 * |   	Joystick	|   ESP-EDU		|
 * |:--------------:|:--------------|
 * | 	GND      	|	GND 		|
 * | 	+5V		 	| 	3V3			|
 * | 	VRX		 	| 	CH0 		|
 * | 	VRY      	| 	CH1 		|
 * | 	SW	     	| 	GPIO_23		|
 * 
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 02/04/2024 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio_mcu.h"
#include "led.h"
#include "ble_hid_mcu.h"
#include "switch.h"
#include "analog_io_mcu.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 500
#define DELAY_MEASURE       50
#define LED_BT	            LED_1
/*==================[internal data definition]===============================*/
TaskHandle_t joystick_task_handle = NULL;
analog_input_config_t adc_x, adc_y;
bool click = false;
/*==================[internal functions declaration]=========================*/
/**
 * @brief 
 * 
 */
void FuncTecla1(void){
    keyboard_cmd_t space = HID_KEY_SPACEBAR;
    BleHidSendKeyboard(0, &space, 1);
}
/**
 * @brief 
 * 
 */
void FuncTecla2(void){
    keyboard_cmd_t down = HID_KEY_DOWN_ARROW;
    BleHidSendKeyboard(0, &down, 1);
}
/**
 * @brief 
 * 
 */
void FuncTecJoy(void){
    click = true;
}
/**
 * @brief 
 * 
 * @param analog_data 
 * @return int8_t 
 */
void UpdateMouse(int8_t * pos, uint16_t analog_data){
    if(analog_data < 50){
        *pos = - 30;
    }else if(analog_data < 1000){
        *pos = - 10;
    }else if(analog_data < 2300){
        *pos = 0;
    }else if(analog_data < 3250){
        *pos = 10;
    }else{
        *pos = 30;
    }
}
/**
 * @brief 
 * 
 * @param pvParameter 
 */
void JoystickTask(void *pvParameter){
    static int8_t mouse_x = 0, mouse_y = 0;
    uint16_t joystick_x, joystick_y;
    while(true){
        if(BleHidStatus() == BLE_CONNECTED){
            AnalogInputReadSingle(adc_y.input, &joystick_y);
            AnalogInputReadSingle(adc_x.input, &joystick_x);
            UpdateMouse(&mouse_x, joystick_x);
            UpdateMouse(&mouse_y, joystick_y);
            if(click){
                BleHidSendMouse(HID_MOUSE_LEFT, mouse_x, mouse_y);
                click = false;
            }else{
                BleHidSendMouse(0, mouse_x, mouse_y);
            }
        }
		vTaskDelay(DELAY_MEASURE / portTICK_PERIOD_MS);
    }
}
/*==================[external functions definition]==========================*/
void app_main(void){
    LedsInit();
    GPIOInit(GPIO_23, GPIO_INPUT);
    GPIOActivInt(GPIO_23, FuncTecJoy, false, NULL);
    SwitchesInit();
    SwitchActivInt(SWITCH_1, FuncTecla1, 0);
    SwitchActivInt(SWITCH_2, FuncTecla2, 0);
    BleHidInit("EP_HID");
    adc_x.input = CH1;
    adc_x.mode = ADC_SINGLE;
    AnalogInputInit(&adc_x);
    adc_y.input = CH3;
    adc_y.mode = ADC_SINGLE;
    AnalogInputInit(&adc_y);

    xTaskCreate(&JoystickTask, "JOYSTICK", 4096, NULL, 5, &joystick_task_handle);

    while(1){
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        switch(BleHidStatus()){
            case BLE_OFF:
                LedOff(LED_BT);
            break;
            case BLE_DISCONNECTED:
                LedToggle(LED_BT);
            break;
            case BLE_CONNECTED:
                LedOn(LED_BT);
            break;
        }
    }
}

/*==================[end of file]============================================*/
