#ifndef VUMETER_H_
#define VUMETER_H_
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup   
 ** @{ */
/** \addtogroup Vumeter Vumeter
 ** @{ */

/** \brief Contains functions to create a vumeter in a color LCD display.
 * 
 * @author Albano Peñalva
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 12/04/2024 | Document creation		                         						|
 * 
 **/

/*==================[inclusions]=============================================*/
#include <stdint.h>
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/
/**
 * @brief 
 * 
 */
typedef struct{
	uint16_t x_pos;				/*!< x position of top left corner of plot */
	uint16_t y_pos;				/*!< y position of top left corner of plot */
	uint16_t width;				/*!< plot width */
    uint16_t height; 			/*!< plot height */
    uint16_t n_bars;			/*!< number of bars */
    uint16_t step_color_1;		/*!< number of bars */
    uint16_t step_color_2;		/*!< number of bars */
    uint16_t step_color_3;		/*!< number of bars */
    uint16_t step_color_4;		/*!< number of bars */
    uint16_t back_color;		/*!< plot background color */
} vumeter_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief 
 * 
 * @param vum       Structure with the plot configuration
 */
void VumeterInit(vumeter_t * vum);

/**
 * @brief 
 * 
 * @param vum       Structure with the plot configuration
 * @param values    height of each vumeter bar (from 0 to 256)
 */
void VumeterUpdate(vumeter_t * vum, uint8_t * values);

#endif /* ROLL_PLOT_H_ */

/*==================[end of file]============================================*/
