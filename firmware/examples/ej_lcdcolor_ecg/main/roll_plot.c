/**
 * @file roll_plot.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2024-04-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*==================[inclusions]=============================================*/
#include "roll_plot.h"
#include "ili9341.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void RTPlotInit(plot_t * plot){
	ILI9341DrawFilledRectangle(plot->x_pos, plot->y_pos,
			plot->x_pos + plot->width, plot->y_pos + plot->height,
			plot->back_color);

}

void RTSignalInit(plot_t * plot, signal_t * signal){
	signal->x_prev = plot->x_pos * 100;
	signal->y_prev = plot->y_pos + plot->height - signal->y_offset;
	signal->plot = plot;
}

void RTPlotDraw(signal_t * signal, int16_t data){
    int16_t x_act, y_act, blanck_act;
    plot_t * plot = signal->plot;
    /* next point to draw */
    y_act = plot->y_pos + plot->height - (data * signal->y_scale) / 100 - signal->y_offset;
    /* it can exceed plot limits */
    if (y_act < plot->y_pos){
        y_act = plot->y_pos;
    }
    if (y_act > (plot->y_pos + plot->height)){
        y_act = plot->y_pos + plot->height;
    }
    /* when reach right limit it start again from left */
    x_act = signal->x_prev + plot->x_scale;
    if ((x_act / 100) < (plot->x_pos + plot->width)){
        ILI9341DrawLine(signal->x_prev / 100, signal->y_prev, x_act / 100, y_act, signal->color);
    } else{
        x_act = plot->x_pos * 100;
    }
    /* to erase previous plot*/
    if (plot->x_scale >= 100){
        blanck_act = x_act + plot->x_scale;
    } else {
        blanck_act = x_act + 100;
    }
    if ((blanck_act / 100) < (plot->x_pos + plot->width)){
        ILI9341DrawFilledRectangle((x_act / 100) + 1, plot->y_pos, blanck_act / 100,
        plot->y_pos + plot->height, plot->back_color);
    } else{
        ILI9341DrawFilledRectangle(plot->x_pos, plot->y_pos, plot->x_pos,
        plot->y_pos + plot->height, plot->back_color);
    }
    /* Update previously drawn point */
    signal->x_prev = x_act;
    signal->y_prev = y_act;
}

/*==================[end of file]============================================*/
