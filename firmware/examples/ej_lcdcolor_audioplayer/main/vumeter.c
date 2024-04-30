/**
 * @file vumeter.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2024-04-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*==================[inclusions]=============================================*/
#include "vumeter.h"
#include "ili9341.h"
/*==================[macros and definitions]=================================*/
#define BAR_WIDTH_PERC  90
#define STEP_HEIGHT     5
#define STEP_DIST       7
#define BAR_MAX         256
#define COLOR_1         ILI9341_GREEN
#define COLOR_2         ILI9341_YELLOW
#define COLOR_3         ILI9341_ORANGE
#define COLOR_4         ILI9341_RED
#define COLOR_TH_1      30
#define COLOR_TH_2      60
#define COLOR_TH_3      80
/*==================[internal data declaration]==============================*/
static uint16_t bars_width, bars_dist, bars_gap;
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void VumeterInit(vumeter_t * vum){
	ILI9341DrawFilledRectangle(vum->x_pos, vum->y_pos,
			vum->x_pos + vum->width, vum->y_pos + vum->height,
			vum->back_color);
    bars_width = (vum->width / vum->n_bars) * BAR_WIDTH_PERC / 100;
    bars_dist = (vum->width / vum->n_bars);
    bars_gap = bars_dist - bars_width;
}

void VumeterUpdate(vumeter_t * vum, uint8_t * values){
    uint16_t n_steps, color, step_start, bar_start;
    for (uint8_t i=0; i<vum->n_bars; i++){
        /* Delete previous bar */
        bar_start = vum->x_pos+i*bars_dist + bars_gap/2;
        ILI9341DrawFilledRectangle(bar_start, vum->y_pos,
			bar_start + bars_width, vum->y_pos + vum->height,
			vum->back_color);
        /* Draw bar's steps */
        n_steps = ((values[i] * vum->height) / BAR_MAX) / STEP_DIST;
        for(uint8_t j=0; j<n_steps; j++){
            step_start = vum->y_pos+vum->height-(STEP_DIST)*j;
            if(STEP_DIST*j < vum->height*COLOR_TH_1/100){
                color = vum->step_color_1;
            } else if(STEP_DIST*j < vum->height*COLOR_TH_2/100){
                color = vum->step_color_2;
            } else if(STEP_DIST*j < vum->height*COLOR_TH_3/100){
                color = vum->step_color_3;
            }else{
                color = vum->step_color_4;
            }
            ILI9341DrawFilledRectangle(bar_start, step_start,
                bar_start + bars_width, step_start - STEP_HEIGHT,
                color);
        }
    }
}
/*==================[end of file]============================================*/
