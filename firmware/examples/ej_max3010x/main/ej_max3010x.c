/*! @mainpage Ejemplo MAX30102
 *
 * @section genDesc General Description
 *
 * Este proyecto ejemplifica el uso del dispositivo MAX30102.
 *
 * \section hardConn Hardware Connection
 *
 * |   	MAX30102	|   ESP-EDU		|
 * |:--------------:|:--------------|
 * | 	SDA     	|	SDA 		|
 * | 	3V3		 	| 	3V3			|
 * | 	SCL		 	| 	SCL 		|
 * | 	GND		 	| 	GND			|
 * 
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 21/05/2024 | Document creation		                         |
 *
 * @author Juan Ignacio Cerrudo (juan.cerrudo@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <iir_filter.h>
#include <max3010x.h>
#include "spo2_algorithm.h"
#include "led.h"
/*==================[macros and definitions]=================================*/
#define BUFFER_SIZE 256
#define SAMPLE_FREQ	100
#define CONFIG_BLINK_PERIOD 100
/*==================[internal data definition]===============================*/
float dato_filt;
float dato;

uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
int32_t bufferLength=100; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid
/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void){
    /* Filtro pasa altos de orden 4 con frecuencia de corte en 1Hz */
	HiPassInit(SAMPLE_FREQ, 1, ORDER_4);
    LedsInit();
    MAX3010X_begin();
	MAX3010X_setup( 30, 1 , 2, SAMPLE_FREQ, 69, 4096);
    /* Se imprimen por consola los valores de frequencia y magnitud correspondiente */
    printf("****MAX30102 Test****\n");

    while(1){
        uint8_t i;
	    for (i = 25; i < 100; i++)
	    {
		    redBuffer[i - 25] = redBuffer[i];
		    irBuffer[i - 25] = irBuffer[i];
	    }

	    //take 25 sets of samples before calculating the heart rate.
	    for ( i = 75; i < 100; i++)
	    {
		    while (MAX3010X_available() == false) //do we have new data?
			    MAX3010X_check(); //Check the sensor for new data
            
		    redBuffer[i] = MAX3010X_getRed();
		    irBuffer[i] = MAX3010X_getIR();
		    MAX3010X_nextSample(); //We're finished with this sample so move to next sample
		            
            //send samples and calculation result to terminal program through UART
	     	dato = (float)redBuffer[i];
			HiPassFilter(&dato, &dato_filt, 1);
	        //printf("%ld,%2.2f,%ld\n", redBuffer[i], dato_filt, heartRate);
             
	}

	//After gathering 25 new samples recalculate HR and SP02
	maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
    printf("HR= %ld, HRvalid= %d \n", heartRate, validHeartRate);
    printf("SPO2= %ld, SPO2Valid= %d \n", spo2, validSPO2);
	LedToggle(LED_1);
    //vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
/*==================[end of file]============================================*/