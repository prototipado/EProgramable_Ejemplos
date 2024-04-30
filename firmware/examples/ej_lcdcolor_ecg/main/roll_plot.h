
#ifndef ROLL_PLOT_H_
#define ROLL_PLOT_H_
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup   
 ** @{ */
/** \addtogroup Rollplot Roll Plot
 ** @{ */

/** \brief Contains functions to create plots in a color LCD display.
 *
 * 
 * @author Albano Peñalva
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 04/04/2024 | Document creation		                         						|
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
	uint16_t x_pos;		/*!< x position of top left corner of plot */
	uint16_t y_pos;		/*!< y position of top left corner of plot */
	uint16_t width;		/*!< plot width */
    uint16_t height; 	/*!< plot height */
    uint16_t x_scale;	/*!< x scale in % (number of pixels drawn per 100 data samples) */
    uint16_t back_color;/*!< plot background color */
} plot_t;

/**
 * @brief 
 * 
 */
typedef struct{
	uint16_t y_scale;	/*!< y scale in % (number of pixels drawn per 100 data value) */
    uint16_t y_offset; 	/*!< y offset */
	uint16_t color;		/*!< plot color */
	uint16_t x_prev;	/*!< x position of last point drawn */
	uint16_t y_prev;	/*!< y position of last point drawn */
	plot_t * plot;		/*!< plot in which the signal'll be drawn */
} signal_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/**
 * @brief  		Initializes a plot
 * @param[in]  	plot: Structure with the plot configuration
 * @retval 		NONE
 */
void RTPlotInit(plot_t * plot);

/**
 * @brief  		Initializes parameters of a signal for a specific plot
 * @param[in]  	plot: Structure with the plot configuration
 * @param[in]  	signal: Structure with the signal configuration
 * @retval 		NONE
 */
void RTSignalInit(plot_t * plot, signal_t * signal);

/**
 * @brief		Write data to SPI port
 * @param[in]	plot: Structure with the plot configuration
 * @param[in]  	signal: Structure with the signal configuration
 * @param[in]	data: Data value to draw in plot
 * @return  	None
 * @note
 */
void RTPlotDraw(signal_t * signal, int16_t data);

#endif /* ROLL_PLOT_H_ */

/*==================[end of file]============================================*/
