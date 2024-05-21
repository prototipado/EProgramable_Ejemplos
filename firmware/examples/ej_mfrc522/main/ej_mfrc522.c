/*! @mainpage Ejemplo MFRC522
 *
 * \section genDesc General Description
 *
 * Este proyecto ejemplifica el uso del dispositivo MFRC522.
 *
 * \section hardConn Hardware Connection
 *
 * |   	MRFC522		|   ESP-EDU		|
 * |:--------------:|:--------------|
 * | 	SDO/MISO 	|	GPIO_22		|
 * | 	3V3		 	| 	3V3			|
 * | 	SCK		 	| 	GPIO_20		|
 * | 	SDI/MOSI 	| 	GPIO_21		|
 * | 	RESET	 	| 	GPIO_18		|
 * | 	CS		 	| 	GPIO_9		|
 * | 	GND		 	| 	GND			|
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 17/05/2024 | Document creation		                         |
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
#include "led.h"
#include "uart_mcu.h"
#include "rfid_utils.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 1000
/*==================[internal data definition]===============================*/
unsigned int last_user_ID;
// RFID structs
MFRC522Ptr_t mfrcInstance;
/*==================[internal functions declaration]=========================*/
/**
 * Executed every time the card reader detects a user in
 */
void userTapIn() {

//	show card UID
	UartSendString(UART_PC,"\nCard uid bytes: ");
	for (uint8_t i = 0; i < mfrcInstance->uid.size; i++) {
		UartSendString(UART_PC," 0X");
		UartSendString(UART_PC, (char*)UartItoa(mfrcInstance->uid.uidByte[i], 16));
		UartSendString(UART_PC," ");
	}
	UartSendString(UART_PC,"\n\r");
	// Convert the uid bytes to an integer, byte[0] is the MSB
	last_user_ID =
		(int)mfrcInstance->uid.uidByte[3] |
		(int)mfrcInstance->uid.uidByte[2] << 8 |
		(int)mfrcInstance->uid.uidByte[1] << 16 |
		(int)mfrcInstance->uid.uidByte[0] << 24;

	UartSendString(UART_PC,"Card Read user ID: ");
	UartSendString(UART_PC, (char*)UartItoa(last_user_ID, 10));
	UartSendString(UART_PC,"\n\r");


}
/*==================[external functions definition]==========================*/
void app_main(void){
	
	LedsInit();
	serial_config_t UART_USB;
	UART_USB.baud_rate = 115200;
	UART_USB.port = UART_PC;
	UartInit(&UART_USB);
	setupRFID(&mfrcInstance);

	UartSendString(UART_PC,"Init MRFC522 test.\r\n");
	
    while(true){
		UartSendString(UART_PC,"Reading... \r\n");
		if (PICC_IsNewCardPresent(mfrcInstance)) {
			if (PICC_ReadCardSerial(mfrcInstance)) {
				LedOn(LED_1);
				userTapIn();
				LedOff(LED_1);
			}
		}
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
}
/*==================[end of file]============================================*/
