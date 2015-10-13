/**
 *	Keil project for TIS
 *	@author		tran nhat duc
 *	@email		ducbk55@gmail.com
 *	@website	TIS.Vn
 *	@ide		Keil uVision 5
 *	@packs		STM32F4xx Keil packs version 2.2.0 or greater required
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 */
#ifndef TM_DEFINES_H
#define TM_DEFINES_H

/*Define use usart ISR*/
#define TM_USART6_USE_CUSTOM_IRQ 	//com2
#define TM_USART3_USE_CUSTOM_IRQ 	// com1
#define TM_USART2_USE_CUSTOM_IRQ	// com extention

/* SIDO for SDCARD*/
#define FATFS_USE_SDIO	1
/* Enable Card detect pin */
#define FATFS_USE_DETECT_PIN 0
/* Use writeprotect pin */
#define FATFS_USE_WRITEPROTECT_PIN		0

/* define DTMF*/
#define DTMF_BIT0_PORT				GPIOD
#define DTMF_BIT0_PIN					GPIO_PIN_15
#define DTMF_BIT1_PORT				GPIOD
#define DTMF_BIT1_PIN					GPIO_PIN_7
#define DTMF_BIT2_PORT				GPIOD
#define DTMF_BIT2_PIN					GPIO_PIN_7
#define DTMF_BIT3_PORT				GPIOD
#define DTMF_BIT3_PIN					GPIO_PIN_7

#define DFMF_BIT4_PORT 				GPIOA
#define DTMF_BIT4_PIN					GPIO_PIN_0

/* define Sw*/
#define ADD_BIT0_PORT					GPIOD
#define ADD_BIT0_PIN					GPIO_PIN_0
#define ADD_BIT1_PORT					GPIOD
#define ADD_BIT1_PIN					GPIO_PIN_1
#define ADD_BIT2_PORT					GPIOD
#define ADD_BIT2_PIN 					GPIO_PIN_4
#define ADD_BIT3_PORT					GPIOB
#define ADD_BIT3_PIN 					GPIO_PIN_5
#define ADD_BIT4_PORT					GPIOB
#define ADD_BIT4_PIN 					GPIO_PIN_6
#define ADD_BIT5_PORT					GPIOB
#define ADD_BIT5_PIN 					GPIO_PIN_7
#define ADD_BIT6_PORT					GPIOB
#define ADD_BIT6_PIN 					GPIO_PIN_8
#define ADD_BIT7_PORT					GPIOA
#define ADD_BIT7_PIN 					GPIO_PIN_12

/* define RELAY OUTPUT*/
#define RELAY_DK1_PORT				GPIOC
#define RELAY_DK1_PIN					GPIO_PIN_0
#define RELAY_DK2_PORT				GPIOC
#define RELAY_DK2_PIN					GPIO_PIN_2
#define RELAY_DK3_PORT				GPIOD
#define RELAY_DK3_PIN					GPIO_PIN_10
#define RELAY_DK4_PORT				GPIOD
#define RELAY_DK4_PIN					GPIO_PIN_11
/* define Buzzer */
#define BUZZER_PORT						GPIOB
#define BUZZER_PIN						GPIO_PIN_9
/* defile weigand INPUT*/
#define W1_D0_PORT 						GPIOA
#define W1_D0_PIN							GPIO_PIN_3
#define W1_D1_PORT 						GPIOA
#define W1_D1_PIN							GPIO_PIN_4
#define W2_D0_PORT 						GPIOA
#define W2_D0_PIN							GPIO_PIN_6
#define W2_D1_PORT 						GPIOA
#define W2_D1_PIN							GPIO_PIN_8

/*define 485 DIR*/
#define CCU_DIR_PORT 					GPIOA
#define CCU_DIR_PIN						GPIO_PIN_11

/* Control LCD */
/* RS - Register select pin */
#define HD44780_RS_PORT			GPIOE
#define HD44780_RS_PIN			GPIO_Pin_8
/* E - Enable pin */
#define HD44780_E_PORT			GPIOE
#define HD44780_E_PIN				GPIO_Pin_10
/*RW - Enable pin */
#define HD44780_RW_PORT			GPIOE
#define HD44780_RW_PIN			GPIO_Pin_9

/* D4 - Data 4 pin */
#define HD44780_D4_PORT			GPIOE
#define HD44780_D4_PIN			GPIO_Pin_4
/* D5 - Data 5 pin */
#define HD44780_D5_PORT			GPIOE
#define HD44780_D5_PIN			GPIO_Pin_5
/* D6 - Data 6 pin */
#define HD44780_D6_PORT			GPIOE
#define HD44780_D6_PIN			GPIO_Pin_6
/* D7 - Data 7 pin */
#define HD44780_D7_PORT			GPIOE
#define HD44780_D7_PIN			GPIO_Pin_7


#endif
