/*! @mainpage Ejemplo DSP
 *
 * @section genDesc General Description
 *
 * Este proyecto ejemplifica el uso de las funcionalidades para Procesamiento 
 * Digital de Señales (DSP).
 * En el mismo se toma una señal de ECG almacenada en memoria y se la utiliza 
 * para probar las funcionalidades de creación y utilización de filtros 
 * digitales, así como las funcionalidades para el cálculo de la Transformada 
 * Rápida de Fourier (FFT) de dicha señal.
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
#include <iir_filter.h>
#include <fft.h>
/*==================[macros and definitions]=================================*/
#define BUFFER_SIZE 256
#define SAMPLE_FREQ	200
/*==================[internal data definition]===============================*/
float ecg[BUFFER_SIZE] = {
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
float freq[BUFFER_SIZE];
float ecg_filt[BUFFER_SIZE];
float ecg_fft[BUFFER_SIZE/2];
float f[BUFFER_SIZE/2];
/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void){
    /* Filtro pasa bajo de orden 4 con frecuencia de corte en 40Hz */
	LowPassInit(SAMPLE_FREQ, 40, ORDER_4);
    /* Filtro pasa altos de orden 4 con frecuencia de corte en 1Hz */
	HiPassInit(SAMPLE_FREQ, 1, ORDER_4);
    /* Se aplican ambos filtros a la señal */
	LowPassFilter(ecg, ecg_filt, BUFFER_SIZE);
	HiPassFilter(ecg_filt, ecg_filt, BUFFER_SIZE);
    /* Se imprimen por consola los valores de la señal sin filtrar y filtrada */
    printf("****Filtros****\n");
    printf("ECG, ECG_filtrado\n");
	for(int16_t i=0; i<BUFFER_SIZE; i++){
		printf("%f, %f\n", ecg[i], ecg_filt[i]);
	}

    /* Inicialización del módulo para cálculo de la FFT */
    FFTInit();
    /* Creación de vector con valores de frecuencia */
    FFTFrequency(SAMPLE_FREQ, BUFFER_SIZE, f);
    /* Cálculo de la magnitud de la FFT */
    FFTMagnitude(ecg, ecg_fft, BUFFER_SIZE);
    /* Se imprimen por consola los valores de frequencia y magnitud correspondiente */
    printf("****FFT****\n");
    printf("Frecuencia, Magnitud\n");
	for(int16_t i=0; i<BUFFER_SIZE/2; i++){
		printf("%f, %f\n", f[i], ecg_fft[i]);
	}
}
/*==================[end of file]============================================*/