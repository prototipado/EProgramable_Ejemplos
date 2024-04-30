
/*! @mainpage Ejemplo LCD color - Reproductor de audio
 *
 * @section genDesc General Description
 *
 *
 * @section hardConn Hardware Connection
 *
 * |   	Speaker		|   ESP-EDU		|
 * |:--------------:|:--------------|
 * | 	IN+      	|	CH0 		|
 * | 	IN-		 	| 	GND			|
 * 
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
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "timer_mcu.h"
#include "gpio_mcu.h"
#include "rtc_mcu.h"
#include "analog_io_mcu.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "switch.h"
#include "ili9341.h"

#include "vumeter.h"
#include "song.h"

#include "fft.h"
/*==================[macros and definitions]=================================*/
#define SAMPLE_FREQ	        8000        /* 8 kSPS */
#define T_SENIAL            125         /* 0.125 ms */
#define CHUNK               1024 
#define MAX_DAC             256        /* DAC: 8 bits*/
#define VUM_BARS            16
#define COLOR_MAIN_1        0x3e98
#define COLOR_MAIN_2        0x5419
#define COLOR_MAIN_3        0x6ab8
#define COLOR_MAIN_4        0x71b9
#define COLOR_BG_1          0x0884
/*==================[internal data definition]===============================*/
TaskHandle_t plot_task_handle = NULL;
static float fft[CHUNK/2];
static float chunk[CHUNK];
static uint32_t song_index = 0;
static bool reset = false;
/*==================[internal functions declaration]=========================*/
/**
 * @brief Función ejecutada en la interrupción de la tecla 1.
 * Inicializa el Timer.
 * 
 */
void FuncSwitchStart(void *param){
    reset = false;
    TimerStart(TIMER_B);
}

/**
 * @brief Función ejecutada en la interrupción del Timer. 
 * Reproduce la señal de audio mediante el DAC.
 * 
 */
void FuncTimerSenial(void* param){
    AnalogOutputWrite(song[song_index]);
    song_index++;
    if(song_index%CHUNK == 0){
        /* Graficar cada 1024 (CHUNK) muestras reproducidas */
        xTaskNotifyGive(plot_task_handle);
    }
    if(song_index == N_SONG){
        song_index = 0;
        TimerStop(TIMER_B);
        reset = true;
        /* Resetear pantalla */
        xTaskNotifyGive(plot_task_handle);
    }
}

/**
 * @brief Calcula la altura de cada una de las barras del vúmetro a partir
 * del análisis de un segmento de la señal.
 * 
 * @param song Puntero a segmento de la señal
 * @param bars Puntero a array con la altura de las barras
 */
void Song2Bars(const uint8_t* song, uint8_t* bars){
    float aux;
    uint16_t steps;

    /* Restar continua */
    for(uint16_t i=0; i<CHUNK; i++){
        chunk[i] = song[i] - (MAX_DAC/2);
    }
    /* Calculo de FFT */
    FFTMagnitude(chunk, fft, CHUNK);
    /* Calcular la altura de las barras a partir de los valores de la FFT */
    steps = (CHUNK / 2) / VUM_BARS;
    for(uint8_t i=0; i<VUM_BARS; i++){
        aux = 0;
        for(uint16_t j=0; j<steps; j++){
            /* Acumulado para cada barra */
            aux += fft[steps*i+j] / steps;
        }
        aux = aux * 100;      /* ajustar en pantalla */
        if(aux < 255){
            bars[i] = (uint8_t) aux;
        }else{
            bars[i] = 255;
        }
    }
}

/**
 * @brief Tarea encargada de la graficación en el display LCD.
 * 
 * @param pvParameter 
 */
static void PlotTask(void *pvParameter){
    vumeter_t* vum = (vumeter_t*)pvParameter; 
    static uint16_t progress_bar, progress_bar_index = 0;
    static uint8_t bars[VUM_BARS];
    progress_bar = N_SONG / CHUNK;
    
    while(true){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(!reset){
            if(song_index-CHUNK == 0){
                /* Título canción */
                uint16_t width, height;
                ILI9341GetStringSize(SONG_NAME, &font_22, &width, &height);
                ILI9341DrawString(120-width/2, 45, SONG_NAME, &font_22, COLOR_MAIN_1, COLOR_BG_1);
                ILI9341GetStringSize(SONG_ARTIST, &font_19, &width, &height);
                ILI9341DrawString(120-width/2, 75, SONG_ARTIST, &font_19, COLOR_MAIN_2, COLOR_BG_1);
                ILI9341DrawIcon(105, 255, ICON_PAUSE, &icon_30, COLOR_MAIN_1, COLOR_BG_1);
            }
            /* Vúmetro */
            Song2Bars(&song[song_index-CHUNK], bars);
            VumeterUpdate(vum, bars);
            /* Progress bar */
            ILI9341DrawFilledCircle(20+200*progress_bar_index/progress_bar, 223, 7, COLOR_BG_1);
            progress_bar_index++;
            ILI9341DrawFilledRectangle(20, 220, 20+200*progress_bar_index/progress_bar, 226, COLOR_MAIN_2);
            ILI9341DrawFilledCircle(20+200*progress_bar_index/progress_bar, 223, 7, COLOR_MAIN_3);
        }else{
            /* Resetear pantalla */
            ILI9341DrawFilledCircle(20+200*progress_bar_index/progress_bar, 223, 7, COLOR_BG_1);
            ILI9341DrawFilledRectangle(20, 220, 20+200*progress_bar_index/progress_bar, 226, COLOR_BG_1);
            ILI9341DrawRectangle(20, 220, 220, 226, COLOR_MAIN_1);
            ILI9341DrawIcon(107, 255, ICON_PLAY, &icon_30, COLOR_MAIN_1, COLOR_BG_1);
            ILI9341DrawFilledRectangle(0, 45, 240, 100, COLOR_BG_1);
            VumeterInit(vum);
            progress_bar_index = 0;
        }     
    }
}
/*==================[external functions definition]==========================*/
void app_main(void){
    /* Configuración de timer */
    timer_config_t timer_senial = {
        .timer = TIMER_B,
        .period = T_SENIAL,
        .func_p = FuncTimerSenial,
        .param_p = NULL
    };
    TimerInit(&timer_senial);
    /* DAC */
    AnalogOutputInit();
    /* FFT */
    FFTInit();

    /* Configuración de display */
    ILI9341Init(SPI_1, GPIO_9, GPIO_18);
	ILI9341Rotate(ILI9341_Portrait_2);
	ILI9341Fill(0x0884);
    /* Vúmmetro */
    static vumeter_t v = {
        .x_pos = 20,
        .y_pos = 100,
        .width = 200,
        .height = 100,
        .n_bars = VUM_BARS,
        .step_color_1 = COLOR_MAIN_1,
        .step_color_2 = COLOR_MAIN_2,
        .step_color_3 = COLOR_MAIN_3,
        .step_color_4 = COLOR_MAIN_4,
        .back_color = COLOR_BG_1
    };
    VumeterInit(&v);
    /* Iconos */
    ILI9341DrawString(10, 8, "10:20", &font_22, COLOR_MAIN_2, COLOR_BG_1);
    ILI9341DrawIcon(180, 8, ICON_WIFI_3, &icon_22, COLOR_MAIN_2, COLOR_BG_1);
    ILI9341DrawIcon(210, 8, ICON_BAT_3, &icon_22, COLOR_MAIN_2, COLOR_BG_1);
    /* Progress bar */
    ILI9341DrawRectangle(20, 220, 220, 226, COLOR_MAIN_1);
    /* Botones */
    ILI9341DrawIcon(107, 255, ICON_PLAY, &icon_30, COLOR_MAIN_1, COLOR_BG_1);
    ILI9341DrawIcon(163, 259, ICON_FAST_FOWARD, &icon_22, COLOR_MAIN_2, COLOR_BG_1);
    ILI9341DrawIcon(55, 259, ICON_REWIND, &icon_22, COLOR_MAIN_2, COLOR_BG_1);
    ILI9341DrawIcon(204, 259, ICON_JUMP_END, &icon_22, COLOR_MAIN_2, COLOR_BG_1);
    ILI9341DrawIcon(14, 259, ICON_JUMP_START, &icon_22,COLOR_MAIN_2, COLOR_BG_1);
    ILI9341DrawCircle(120, 270, 29, COLOR_MAIN_1);
    ILI9341DrawCircle(120, 270, 28, COLOR_MAIN_1);
    ILI9341DrawCircle(120, 270, 27, COLOR_MAIN_1);
    ILI9341DrawCircle(67, 270, 19, COLOR_MAIN_2);
    ILI9341DrawCircle(67, 270, 18, COLOR_MAIN_2);
    ILI9341DrawCircle(25, 270, 19, COLOR_MAIN_2);
    ILI9341DrawCircle(25, 270, 18, COLOR_MAIN_2);
    ILI9341DrawCircle(173, 270, 19, COLOR_MAIN_2);
    ILI9341DrawCircle(173, 270, 18, COLOR_MAIN_2);
    ILI9341DrawCircle(215, 270, 19, COLOR_MAIN_2);
    ILI9341DrawCircle(215, 270, 18, COLOR_MAIN_2);

    /* Teclas */
    SwitchesInit();
    SwitchActivInt(SWITCH_1, FuncSwitchStart, NULL);
    
    /* Tarea para actualizar pantalla */
    xTaskCreate(&PlotTask, "Plot", 32768, &v, 5, &plot_task_handle);
}

/*==================[end of file]============================================*/
