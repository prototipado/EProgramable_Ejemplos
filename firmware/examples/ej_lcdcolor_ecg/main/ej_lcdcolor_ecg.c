
/*! @mainpage Ejemplo graficación de señales en display LCD color.
 *
 * @section genDesc General Description
 *
 * Este proyecto ejemplifica el uso de la pantalla LCD color para 
 * graficar una señal temporal. Emula la interfaz del Monito de 
 * ECG Portable BeC: [bececg.com](https://bececg.com/).
 * También se ejemplifica el uso del reloj de tiempo real (RTC).
 * 
 * @section hardConn Hardware Connection
 *
 * |   	Display		|   ESP-EDU		|
 * |:--------------:|:--------------|
 * | 	SDO/MISO 	|	GPIO_22		|
 * | 	LED		 	| 	3V3			|
 * | 	SCK		 	| 	GPIO_20		|
 * | 	SDI/MOSI 	| 	GPIO_21		|
 * | 	DC/RS	 	| 	GPIO_9		|
 * | 	RESET	 	| 	GPIO_18		|
 * | 	CS		 	| 	GPIO_19		|
 * | 	GND		 	| 	GND			|
 * | 	VCC		 	| 	3V3			|
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 05/04/2024 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sys/time.h"

#include "iir_filter.h"
#include "timer_mcu.h"
#include "gpio_mcu.h"
#include "rtc_mcu.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "switch.h"
#include "ili9341.h"
#include "roll_plot.h"
#include "heart_pic.h"
/*==================[macros and definitions]=================================*/
#define BUFFER_SIZE         256
#define SAMPLE_FREQ	        200
#define T_SENIAL            4000 
#define CHUNK               16 
#define LIGHT_BLUE_COLOR    0x0B2F
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
TaskHandle_t plot_task_handle = NULL;
uint8_t frecuencia_cardiaca = 71;
/*==================[internal functions declaration]=========================*/
/**
 * @brief Función ejecutada en la interrupción del Timer
 * 
 */
void FuncTimerSenial(void* param){
    xTaskNotifyGive(plot_task_handle);
}

/**
 * @brief Tarea encargada de filtrar la señal y graficarla en
 * el display LCD.
 * 
 */
static void PlotTask(void *pvParameter){
    static uint8_t indice = 0;
    static char freq[] = "000";
    static char hour_min[] = "00:00";
    static bool beat = true;
    rtc_t actual_time;

    /* Configuración de área de gráfica */
    plot_t plot1 = {
        .x_pos = 0,
        .y_pos = 160,
        .width = 240,
        .height = 100,
        .x_scale = 30,
        .back_color = ILI9341_WHITE
	};
	RTPlotInit(&plot1); 
    /* Configuración de señal a graficar */
    signal_t ecg1 = {
        .y_scale = 40,
        .y_offset = 50,
        .color = ILI9341_RED,
        .x_prev = 0,
        .y_prev = 0
	};
	RTSignalInit(&plot1, &ecg1);

    while(true){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        /* Filtrado de señal */
        HiPassFilter(&ecg[indice], ecg_filt, CHUNK);
        LowPassFilter(ecg_filt, ecg_filt, CHUNK);

        /* Graficación de señal */
        for(uint8_t i=0; i<CHUNK; i++){
            RTPlotDraw(&ecg1, ecg_filt[i]);
        }
        indice += CHUNK;

        if(indice == 0){
            /* Actualización de datos en display */
            ILI9341DrawString(20, 60, freq, &font_89, ILI9341_WHITE, ILI9341_WHITE);
            ILI9341DrawString(10, 8, hour_min, &font_30, LIGHT_BLUE_COLOR, LIGHT_BLUE_COLOR);
            sprintf(freq, "%03i", frecuencia_cardiaca);
            RtcRead(&actual_time);
            sprintf(hour_min, "%02i:%02i", actual_time.hour%MAX_HOUR, actual_time.min%MAX_MIN);
            ILI9341DrawString(20, 60, freq, &font_89, LIGHT_BLUE_COLOR, ILI9341_WHITE);
            ILI9341DrawString(10, 8, hour_min, &font_30, ILI9341_WHITE, LIGHT_BLUE_COLOR);
            if(beat){
                ILI9341DrawPicture(170, 65, HEART_WIDTH, HEART_HEIGHT, heart);
            }else{
                ILI9341DrawFilledRectangle(170, 65, 170+HEART_WIDTH, 65+HEART_HEIGHT, ILI9341_WHITE);
            }
            beat = !beat;
        }
    }
}
/*==================[external functions definition]==========================*/
void app_main(void){
    /* Configuración de timer */
    timer_config_t timer_senial = {
        .timer = TIMER_B,
        .period = T_SENIAL*CHUNK,
        .func_p = FuncTimerSenial,
        .param_p = NULL
    };
    TimerInit(&timer_senial);

    /* Configuración de display */
    ILI9341Init(SPI_1, GPIO_9, GPIO_18);
	ILI9341Rotate(ILI9341_Portrait_2);
	ILI9341Fill(ILI9341_WHITE);
    ILI9341DrawFilledRectangle(0, 0, 240, 40, LIGHT_BLUE_COLOR);
    ILI9341DrawFilledRectangle(0, 280, 240, 320, LIGHT_BLUE_COLOR);
    ILI9341DrawString(10, 290, "TIME10S", &font_22, ILI9341_WHITE, LIGHT_BLUE_COLOR);
    ILI9341DrawString(178, 290, "00:04", &font_22, ILI9341_WHITE, LIGHT_BLUE_COLOR);
    ILI9341DrawString(178, 120, "bpm", &font_22, LIGHT_BLUE_COLOR, ILI9341_WHITE);
    ILI9341DrawString(20, 60, "000", &font_89, LIGHT_BLUE_COLOR, ILI9341_WHITE);
    ILI9341DrawIcon(170, 8, ICON_BLUETOOTH, &icon_30, ILI9341_WHITE, LIGHT_BLUE_COLOR);
    ILI9341DrawIcon(200, 8, ICON_BAT_3, &icon_30, ILI9341_WHITE, LIGHT_BLUE_COLOR);

    /* Filtros */
    LowPassInit(SAMPLE_FREQ, 30, ORDER_2);
    HiPassInit(SAMPLE_FREQ, 1, ORDER_2);

    /* Tarea para actualizar pantalla */
    xTaskCreate(&PlotTask, "Plot", 4096, NULL, 5, &plot_task_handle);

    /* Configuración inicial de RTC */
    // rtc_t config_time = {
    //     .year = 2024,
    //     .month = 4,
    //     .mday = 8,
    //     .hour = 12,
    //     .min = 32,
    //     .sec = 0
    // };
    // RtcConfig(&config_time);

    TimerStart(timer_senial.timer);
}

/*==================[end of file]============================================*/
