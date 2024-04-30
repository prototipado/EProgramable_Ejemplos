/*! @mainpage Ejemplo Bluetooth - Filter
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
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

#include "led.h"
#include "neopixel_stripe.h"
#include "ble_mcu.h"
#include "timer_mcu.h"

#include "iir_filter.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 500
#define LED_BT	            LED_1
#define BUFFER_SIZE         256
#define SAMPLE_FREQ	        200
#define T_SENIAL            4000 
#define CHUNK               4 
/*==================[internal data definition]===============================*/
float ecg[] = {
     76,  76,  77,  77,  76,  83,  85,  78,  76,  85,  93,  85,  79,
     86,  93,  93,  85,  87,  94,  98,  93,  87,  95, 104,  99,  91,
     93, 102, 104,  99,  96, 101, 106, 102,  96,  97, 104, 106,  97,
     94, 100, 103, 101,  91,  95, 103, 100,  94,  90,  98, 104,  94,
     87,  93,  99,  97,  87,  86,  96,  98,  90,  83,  90,  96,  89,
     81,  80,  87,  92,  82,  78,  84,  89,  80,  72,  78,  82,  82,
     73,  72,  81,  82,  79,  69,  77,  82,  81,  76,  68,  78,  80,
     76,  73,  78,  82,  82,  75,  72,  86,  84,  78,  76,  85,  95,
     88,  81,  83,  93,  90,  86,  83,  88,  93,  86,  82,  82,  92,
     89,  82,  82,  88,  94,  84,  82,  90,  98,  94,  87,  91,  95,
     98,  93,  90,  97, 104, 105,  96,  93, 107, 116, 118, 127, 148,
    181, 208, 231, 252, 241, 198, 139,  76,  43,  32,  29,  42,  65,
     86,  90,  88,  93, 101, 107, 102,  98, 103, 110, 104,  98,  99,
    107, 109,  96,  95, 103, 107, 102,  95,  95, 102, 105,  94,  94,
    102, 102,  99,  94,  96, 102,  99,  90,  92, 100, 102,  95,  90,
     98, 104,  97,  89,  94, 102, 103,  97,  93, 100, 105, 102,  93,
     97, 104, 104, 100,  96, 108, 111, 104,  99, 101, 108, 102,  96,
     97, 104, 104,  97,  89,  91, 100,  91,  81,  79,  85,  86,  73,
     69,  75,  79,  75,  68,  68,  76,  76,  69,  67,  74,  81,  77,
     71,  72,  82,  82,  76,  77,  76,  76,  75
};
static float ecg_filt[CHUNK];
TaskHandle_t fft_task_handle = NULL;
bool filter = false;
/*==================[internal functions declaration]=========================*/
void read_data(uint8_t * data, uint8_t length){
    switch(data[0]){
        case 'A':
            filter = true;
            break;
        case 'a':
            filter = false;
            break;
    }
}

void FuncTimerSenial(void* param){
    xTaskNotifyGive(fft_task_handle);
}

static void FftTask(void *pvParameter){
    char msg[128];
    char msg_chunk[24];
    static uint8_t indice = 0;
    while(true){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(filter){
            HiPassFilter(&ecg[indice], ecg_filt, CHUNK);
            LowPassFilter(ecg_filt, ecg_filt, CHUNK);
        } else{
            memcpy(ecg_filt, &ecg[indice], CHUNK*sizeof(float));
        }
        strcpy(msg, "");
        for(uint8_t i=0; i<CHUNK; i++){
            sprintf(msg_chunk, "*G%.2f*", ecg_filt[i]);
            strcat(msg, msg_chunk);
        }
        indice += CHUNK;

        BleSendString(msg);
    }
}
/*==================[external functions definition]==========================*/
void app_main(void){
    uint8_t blink = 0;
    static neopixel_color_t color;
    ble_config_t ble_configuration = {
        "ESP_EDU_1",
        read_data
    };
    timer_config_t timer_senial = {
        .timer = TIMER_B,
        .period = T_SENIAL*CHUNK,
        .func_p = FuncTimerSenial,
        .param_p = NULL
    };

    NeoPixelInit(BUILT_IN_RGB_LED_PIN, BUILT_IN_RGB_LED_LENGTH, &color);
    NeoPixelAllOff();
    TimerInit(&timer_senial);
    LedsInit();  
    LowPassInit(SAMPLE_FREQ, 30, ORDER_2);
    HiPassInit(SAMPLE_FREQ, 1, ORDER_2);
    BleInit(&ble_configuration);

    xTaskCreate(&FftTask, "FFT", 4096, NULL, 5, &fft_task_handle);
    TimerStart(timer_senial.timer);

    while(1){
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        switch(BleStatus()){
            case BLE_OFF:
                NeoPixelAllOff();
            break;
            case BLE_DISCONNECTED:
                if(blink%2){
                    NeoPixelAllColor(NEOPIXEL_COLOR_BLUE);
                }else{
                    NeoPixelAllOff();
                }
                blink++;
            break;
            case BLE_CONNECTED:
                NeoPixelAllColor(NEOPIXEL_COLOR_BLUE);
            break;
        }
    }
}

/*==================[end of file]============================================*/
