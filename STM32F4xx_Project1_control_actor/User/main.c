/**
 *	Keil project for FatFS for SD cards
 *
 *	Before you start, select your target, on the right of the "Load" button
 *
 *	@author		Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@ide		Keil uVision 5
 *	@packs		STM32F4xx Keil packs version 2.2.0 or greater required
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 */
/* Include core modules */
#include "stm32f4xx.h"
//#include "stm32f4xx_hal.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_fatfs.h"
#include "tm_stm32f4_exti.h"
#include "tm_stm32f4_watchdog.h"
#include "myfunsion.h"
#include "tm_stm32f4_usart.h"
#include "tm_stm32f4_adc.h"
#include "tm_stm32f4_hd44780.h"
#include "sim5215.h"

#include <stdio.h>
#include <string.h>

#define start 0x01
#define stop 	0x0A
#define roledk1 	0x31
#define roledk2 	0x32

///* Fatfs object */
//FATFS FatFs;
///* File object */
//FIL fil;
//FRESULT fres;
///* creat by duc */
char buffer[100];
//FRESULT res;         											/* FatFs function common result code */
//UINT br, bw;         											/* File read/write count */
//char Temp[800];
//UINT		Written = 0;
//char gsm_data[10];
//uint32_t sys_time;
//int pointer, adc0;
//char data_conv[800];
//char SDPath[4];
//int check_in=0;
volatile uint8_t counter = 0;
/* Khoi Ban phim */
unsigned char express;
/*Khoi Switch*/
void read_sw_add(void);
/*Khoi LCD*/
	uint8_t customChar[] = {
		0x1F,	/*  xxx 11111 */
		0x11,	/*  xxx 10001 */
		0x11,	/*  xxx 10001 */
		0x11,	/*  xxx 10001 */
		0x11,	/*  xxx 10001 */
		0x11,	/*  xxx 10001 */
		0x11,	/*  xxx 10001 */
		0x1F	/*  xxx 11111 */
	};
unsigned char flag_LCD = 0;
char buffer_lcd[20];
/*time out*/
unsigned int timeout = 0	;
/*Khoi 485*/
void init_485();
int send_485();
int recv_485();
unsigned char flag_485 = 0;
/* Khoi INPUT*/
unsigned char flag_W1D1 = 0;
unsigned char flag_W1D0 = 0;
unsigned char flag_W2D1 = 0;
unsigned char flag_W2D0 = 0;

/* value DIP switch*/
unsigned char value_dip =0;
/* Khoi OUTPUT*/

int turn_on_dk1();
int turn_off_dk1();
int	turn_on_dk2();
int turn_off_dk2();

		int timerdk1 =0;
		int timerdk2 =0;

/* Create 2 callback functions for custom timers */
void CustomTIMER1_Task(void* UserParameters);

/* Pointers to custom timers */
TM_DELAY_Timer_t* CustomTimer1;

/* end*/




int main(void) {
		char buffer[100];
		char adc_str[15];
		int i = 0;
		int count_s = 0;
		int is_write = 1;  // 1: che do ghi the nho, 0: che do day len sv
		int timer_dk1 =0;
		int timer_dk2 =0;
	
	/* Free and total space */
	uint32_t total, free;
	
	/* Initialize system */
	SystemInit();
	
	/* Initialize delays */
	TM_DELAY_Init();
	/* Enable watchdog, 4 seconds before timeout */
	if (TM_WATCHDOG_Init(TM_WATCHDOG_Timeout_8s)) {
		/* Report to user */
		//printf("Reset occured because of Watchdog\n");
	}	
	/* Reset counter to 0 */
	TM_DELAY_SetTime(0);
	/* init DTMF*/
	TM_GPIO_Init(DTMF_BIT0_PORT, DTMF_BIT0_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
	TM_GPIO_Init(DTMF_BIT1_PORT, DTMF_BIT1_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
	TM_GPIO_Init(DTMF_BIT2_PORT, DTMF_BIT2_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
	TM_GPIO_Init(DTMF_BIT3_PORT, DTMF_BIT3_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
	/* DTMF*/
		if (TM_EXTI_Attach(DFMF_BIT4_PORT, DTMF_BIT4_PIN, TM_EXTI_Trigger_Rising) == TM_EXTI_Result_Ok) {
		TM_USART_Puts(USART6, "khoi tao ngat DFMF_BIT4\n");
	}
	
	/*init interrup INPUT*/
		if (TM_EXTI_Attach(W1_D0_PORT, W1_D0_PIN, TM_EXTI_Trigger_Rising) == TM_EXTI_Result_Ok) {
		TM_USART_Puts(USART6, "khoi tao ngat W1_D0\n");
	}
		if (TM_EXTI_Attach(W1_D1_PORT, W1_D1_PIN, TM_EXTI_Trigger_Rising) == TM_EXTI_Result_Ok) {
		TM_USART_Puts(USART6, "khoi tao ngat W1_D1\n");
	}
		if (TM_EXTI_Attach(W2_D1_PORT, W2_D1_PIN, TM_EXTI_Trigger_Rising) == TM_EXTI_Result_Ok) {
		TM_USART_Puts(USART6, "khoi tao ngat W2_D1\n");
	}
			if (TM_EXTI_Attach(W2_D0_PORT, W2_D0_PIN, TM_EXTI_Trigger_Rising) == TM_EXTI_Result_Ok) {
		TM_USART_Puts(USART6, "khoi tao ngat W2_D0\n");
	}
	/*init SWADD*/
	TM_GPIO_Init(ADD_BIT0_PORT, ADD_BIT0_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);	
	TM_GPIO_Init(ADD_BIT1_PORT, ADD_BIT1_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);
	TM_GPIO_Init(ADD_BIT2_PORT, ADD_BIT2_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);
	TM_GPIO_Init(ADD_BIT3_PORT, ADD_BIT3_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);
	TM_GPIO_Init(ADD_BIT4_PORT, ADD_BIT4_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);
	TM_GPIO_Init(ADD_BIT5_PORT, ADD_BIT5_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);
	TM_GPIO_Init(ADD_BIT6_PORT, ADD_BIT6_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);
	TM_GPIO_Init(ADD_BIT7_PORT, ADD_BIT7_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);
	/* init OUTPUT*/
	TM_GPIO_Init(RELAY_DK1_PORT, RELAY_DK1_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	TM_GPIO_Init(RELAY_DK2_PORT, RELAY_DK2_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	TM_GPIO_Init(RELAY_DK4_PORT, RELAY_DK4_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
/* Initialize USART6 at 115200 baud, TX: PC6, RX: PC7 , COM 1*/ 
	TM_USART_Init(USART6, TM_USART_PinsPack_1, 9600);
/* Initialize USART3 at 115200 baud, TX: PD5, RX: PD6 ,	COM 2 */
	TM_USART_Init(USART3, TM_USART_PinsPack_3, 9600);
/* Initialize USART1 at 115200 baud, TX: PA9, RX: PA10, CONG 485 */
	TM_USART_Init(USART1, TM_USART_PinsPack_1, 9600);
	
/* int DIR 485 set = send , reset = recvice*/ 
	TM_GPIO_Init(CCU_DIR_PORT, CCU_DIR_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High);
	TM_GPIO_SetPinHigh(CCU_DIR_PORT,CCU_DIR_PIN);
/* Init 2 custom timers */
/* Timer1 has reload value each 1000ms, enabled auto reload feature and timer is enabled */
	CustomTimer1 = TM_DELAY_TimerCreate(500, 1, 1, CustomTIMER1_Task, NULL);
/* Init LCD*/
	TM_GPIO_Init(HD44780_RW_PORT, HD44780_RW_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	TM_GPIO_SetPinLow(HD44780_RW_PORT,HD44780_RW_PIN);
    //Initialize LCD 20 cols x 4 rows
    TM_HD44780_Init(20, 4);
    
    //Save custom character on location 0 in LCD
    TM_HD44780_CreateChar(0, &customChar[0]);
    
    //Put string to LCD
    TM_HD44780_Puts(0, 0, "STM32F407VET\n\rCreartbyR&D-TIS"); /* 0 dong 1, 1 dong 2*/
    TM_HD44780_Puts(0, 2, "Welcome");
		Delayms(2000);
		TM_HD44780_Clear();
		//TM_HD44780_CursorOn();
    //TM_HD44780_Puts(0, 3, "line 4");
	TM_USART_Puts(USART6, "Hello world3\n");
	TM_USART_Puts(USART6, "TEST SW ADD TIME OUT\n");
	read_sw_add();
	timeout = value_dip;
		sprintf(buffer,"%d", timeout);
		TM_USART_Puts(USART6, buffer);
//	turn_on_dk1();
//	turn_on_dk2();
//	Delayms(3000);
//	turn_off_dk1();
//	turn_off_dk2();
//	TM_USART_Puts(USART1, "Hello world1\n");
//	TM_DISCO_LedOn(LED_GREEN | LED_RED);
	/* Initialize ADC1 on channel 0, this is pin PA0 */
//	TM_ADC_Init(ADC1, ADC_Channel_0);
//	/* Initialize ADC1 on channel 3, this is pin PA3 */
//	TM_ADC_Init(ADC1, ADC_Channel_3);
	//Sendmessge(1,1,"anhduc\r");
	
//		/* Mount drive */
//		if (f_mount(&FatFs, "SD:", 1) == FR_OK) {
//		/* Mounted OK, turn on RED LED */
//		//TM_DISCO_LedOn(LED_RED);
//		//
//		TM_USART_Puts(USART6, "Mount OK \n");
//		Delayms(5000);
//		f_mount(0, "0:", NULL);
//		}
//		if (f_mount(&FatFs, "SD:", 1) == FR_OK) {
//		/* Mounted OK, turn on RED LED */
//		//TM_DISCO_LedOn(LED_RED);
//		//
//		TM_USART_Puts(USART6, "Mount OK \n");
//		Delayms(5000);
//		/* Format name, I have on subfolder everything on my SD card */
//	//sprintf((char *)buffer, "/www%s", name);
//			fres = f_open(&fil, "SD:stfile.txt",  FA_CREATE_ALWAYS | FA_READ | FA_WRITE);

//		/* Try to open file */
////		if (f_open(&fil, "stfile.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE) == FR_OK) {
//			if (fres == FR_OK) {
//			/* File opened, turn off RED and turn on GREEN led */
//			//TM_DISCO_LedOn(LED_GREEN);
//			//TM_DISCO_LedOff(LED_RED);
//				sprintf(buffer,"%d",fres);
//			TM_USART_Puts(USART6,buffer);
//			TM_USART_Puts(USART6, "Ledoff(LED_RED)\n");
////			if(f_mkfs((TCHAR const*)SDPath, 0, 0) != FR_OK)
////      {
////        /* FatFs Format Error */
////        //Error_Handler();
////      }
////			/* If we put more than 0 characters (everything OK) */
//			if (f_puts("First string in my file\n", &fil) > 0) {
//				if (TM_FATFS_DriveSize(&total, &free) == FR_OK) {
//					/* Data for drive size are valid */
//				}
//				
//				/* Turn on both leds */
//				TM_USART_Puts(USART6, "LedOn(LED_RED_GREEN)\n");
//				
//			}
//			
//			/* Close file, don't forget this! */
//			f_close(&fil);
//		}	

//		/* Unmount drive, don't forget this! */
//		if(f_mount(0, "", 1)) TM_USART_Puts(USART6, "UN Mount OK \n");;
//	}
	
	
	/*creat by duc*/
//	if(debug) TM_USART_Puts(USART2, "Test UART2 debug\n");
//	if(debug) TM_USART_Puts(USART2, "\n");
//	//init_sim5215();
//	//Disconnect_server();
//	Connect_server();
//	Delayms(1000);
//	memset(buffer,'\0',0);
		/* Reset watchdog */
	TM_WATCHDOG_Reset();
	TM_HD44780_Clear();
	/*end by duc*/
	while (1) {
//		/*creat by duc*/
				if (TM_USART_Gets(USART6, buffer,sizeof(buffer))) {
					if(buffer[4]==roledk1)
					{
						if(!flag_W1D1){flag_W1D0=1;
						timerdk1 =0;
						timeout = buffer[5]-'0';
						}
						TM_USART_Puts(USART6,"nhan DK1 role 1");
					}
					else if(buffer[4]==roledk2)
					{
						if(!flag_W1D0){ flag_W1D1=1;
						timerdk2 =0;
						timeout = buffer[5]-'0';}
						TM_USART_Puts(USART6,"nhan DK2 role 2");
					}
					else{ 
						TM_USART_Puts(USART6,"nhan lenh sai");
						TM_USART_Putc(USART6,buffer[4]);
					}
				memset(buffer,'\0',0);
				}
//				if (TM_USART_Gets(USART2, buffer,sizeof(buffer))&&strstr(buffer,"\n")) {
////			/* Return string back */
//					TM_USART_Puts(USART1, buffer);
//					TM_USART_Putc(USART1,13);
//					memset(buffer,'\0',0);
//		}
//				if (TM_DELAY_Time() >= 2000) {
//			/* Reset time */
//			TM_DELAY_SetTime(0);
//		/* 							Read ADC1 Channel0					Read ADC1 Channel3 */
//		sprintf(adc_str, "%4d\n\r", TM_ADC_Read(ADC1, ADC_Channel_0));
//		TM_USART_Puts(USART1, "AT+TCPWRITE=20\r");
//		TM_USART_Puts(USART1, adc_str);
//		//TM_USART_Putc(USART1,26);
//			/* Put to USART2  */
//		if(debug) TM_USART_Puts(USART2,adc_str);
//			
//		}
/*process LCD*/
	if(flag_LCD){
		sprintf(buffer_lcd,"ROLE1:%d %3ds\n\rROLE2:%d %3ds",flag_W1D0,timer_dk1,flag_W1D1,timer_dk2);
		TM_HD44780_Puts(0, 0,buffer_lcd); /* 0 dong 1, 1 dong 2*/
		flag_LCD=0;
	}
	if(flag_485){
		if(flag_W1D0) TM_USART_Puts(USART1, "/LED001>\r\n");
		if(flag_W1D1)	TM_USART_Puts(USART1, "/LED002>\r\n");
		if(!flag_W1D0 && !flag_W1D1) TM_USART_Puts(USART1, "/LED000>\r\n");
		if(flag_W1D0) TM_USART_Puts(USART6, "/LED001>\r\n");
		if(flag_W1D1)	TM_USART_Puts(USART6, "/LED002>\r\n");
		if(!flag_W1D0 && !flag_W1D1) TM_USART_Puts(USART6, "/LED000>\r\n");
	flag_485=0;
	}
/* xu li W1D1 - dk2*/
	if(flag_W1D1){
		turn_on_dk2();
		if (timer_dk2 >= timeout){
		turn_off_dk2();
		flag_W1D1 =0;
		flag_W2D0 =0;
		}
			
	}
/* xu li W1D0 - dk1*/
	if(flag_W1D0){
		turn_on_dk1(timeout);
		if (timer_dk1 >= timeout){
			turn_off_dk1();
			flag_W1D0=0;
			flag_W2D0 =0;
		}
	}
/* xu li W2D1*/		
	if(flag_W2D1){
		turn_off_dk1();
		turn_off_dk2();
		flag_W2D1=0;
		flag_W1D0=0;
		flag_W1D1=0;
	}
	if(flag_W2D0){
		turn_on_dk1();
		turn_on_dk2();
		flag_W1D0=1;
		flag_W1D1=1;
		flag_W2D0=0;
	}
	if(flag_W1D0) timer_dk1 = timerdk1/2;
	else timer_dk1=0;
	if(flag_W1D1) timer_dk2 = timerdk2/2;
	else timer_dk2=0;
	
	TM_WATCHDOG_Reset();
		/*end*/
}
}
//void TM_USART1_ReceiveHandler(uint8_t c) {
//	
////Do your stuff here when byte is received
//	char *buffer;
//	if(debug)	TM_USART_Puts(USART2, buffer);
//}
//void TM_USART2_ReceiveHandler(uint8_t c) {
//	
////Do your stuff here when byte is received
//	char *buffer;
//	if(debug)	TM_USART_Puts(USART2, buffer);
//}
void TM_EXTI_Handler(uint16_t GPIO_Pin) {
	/* Handle external line 0 interrupts */
	if (GPIO_Pin == DTMF_BIT4_PIN) {
		
		if(TM_GPIO_GetInputPinValue(DTMF_BIT0_PORT, DTMF_BIT0_PIN)==0){ // Q1  =0
			if(TM_GPIO_GetInputPinValue(DTMF_BIT1_PORT, DTMF_BIT1_PIN)==0){//  Q2 =0
				if(TM_GPIO_GetInputPinValue(DTMF_BIT2_PORT, DTMF_BIT2_PIN)==0){// Q3 =0
					if(TM_GPIO_GetInputPinValue(DTMF_BIT3_PORT, DTMF_BIT3_PIN)==0)// Q4 =0 Q321=0 
						express = 'D';
					else express = '8';
						}
				else{ // Q1 =0,Q2=0,Q3=1
					if(TM_GPIO_GetInputPinValue(DTMF_BIT3_PORT, DTMF_BIT3_PIN)==0)
								express = '4';
					else express = '#';
						}
			}
			else //Q1=0,Q2=1,
			{
				if(TM_GPIO_GetInputPinValue(DTMF_BIT2_PORT, DTMF_BIT2_PIN)==0){// Q3 =0
					if(TM_GPIO_GetInputPinValue(DTMF_BIT3_PORT, DTMF_BIT3_PIN)==0)// Q4 =0  
						express = '2';
					else express = '0';
						}
				else{ // Q1 =0,Q2=1,Q3=1
					if(TM_GPIO_GetInputPinValue(DTMF_BIT3_PORT, DTMF_BIT3_PIN)==0)
								express = '6';
					else 	express = 'B';
				}
			}
		}
		else{					//Q1=1
				if(TM_GPIO_GetInputPinValue(DTMF_BIT1_PORT, DTMF_BIT1_PIN)==0){//  Q2 =0
					if(TM_GPIO_GetInputPinValue(DTMF_BIT2_PORT, DTMF_BIT2_PIN)==0){// Q3 =0
						if(TM_GPIO_GetInputPinValue(DTMF_BIT3_PORT, DTMF_BIT3_PIN)==0)// Q4 =0 
						express = '1';
						else express = '9';
						}
					else{ // Q1 =1,Q2=0,Q3=1
				if(TM_GPIO_GetInputPinValue(DTMF_BIT3_PORT, DTMF_BIT3_PIN)==0)
								express = '5';
					else express = 'A';
					}
				}
				else //Q1=1,Q2=1,
				{
				if(TM_GPIO_GetInputPinValue(DTMF_BIT2_PORT, DTMF_BIT2_PIN)==0){// Q3 =0
					if(TM_GPIO_GetInputPinValue(DTMF_BIT3_PORT, DTMF_BIT3_PIN)==0)// Q4 =0  
						express = '3';
					else express = '.';
						}
				else{ // Q1 =1,Q2=1,Q3=1
					if(TM_GPIO_GetInputPinValue(DTMF_BIT3_PORT, DTMF_BIT3_PIN)==0)
						express = '7';
					else express = 'C';
				}
			}
		}
		
		TM_USART_Putc(USART6,express);
		/* Toggle RED led */
//		TM_DISCO_LedToggle(LED_RED);
//		/* Check counter */
		if (++counter >= 10) {
			/* Detach external interrupt for GPIO_Pin_0 no matter on which GPIOx is connected */
			TM_EXTI_Detach(GPIO_Pin_0);
		}
	}
	
//	/* Handle external line 13 interrupts */
	
	if (GPIO_Pin == W1_D0_PIN) {
		/* run W1 D0 - dieu khien RELAY_DK1_PORT , RELAY_DK1_PIN*/
		if(!flag_W1D1){ flag_W1D0 =1 ;		timeout = value_dip;}
		TM_USART_Puts(USART6, "xu li W1 D0\n");
		timerdk1 =0;
	}
	if (GPIO_Pin == W1_D1_PIN) {
		/* run W1 D1 - dieu khien RELAY_DK2_PORT , RELAY_DK2_PIN*/
		if(!flag_W1D0) {flag_W1D1 =1 ; timeout = value_dip;}
		TM_USART_Puts(USART6, "xu li W1D1 \n");
		timerdk2 =0;
		//Delayms(10);
//		if (++counter >= 10) {
///* Detach external interrupt for GPIO_Pin_0 no matter on which GPIOx is connected */
//			TM_EXTI_Detach(W1_D1_PIN);
//		}	
	}
	if (GPIO_Pin == W2_D1_PIN) {
		/* run w2 D1 */
	flag_W2D1 =1 ;
	flag_W2D0 =0 ;
	flag_W1D0 =0 ;
	flag_W1D1 =0 ;
	
		TM_USART_Puts(USART6, "xu li W2D1 \n");
		//Delayms(10);
//		if (++counter >= 10) {
///* Detach external interrupt for GPIO_Pin_0 no matter on which GPIOx is connected */
//			TM_EXTI_Detach(W2_D1_PIN);
//		}	
}
	if (GPIO_Pin == W2_D0_PIN) {
		/* run w2 D1 */
	flag_W2D0 =1 ;
	flag_W2D1 =0 ;
	flag_W1D0 =1 ;
	flag_W1D1 =1 ;
	timerdk1 =0;
	timerdk2 =0;
			TM_USART_Puts(USART6, "xu li W2D1 \n");
		//Delayms(10);
//		if (++counter >= 10) {
///* Detach external interrupt for GPIO_Pin_0 no matter on which GPIOx is connected */
//			TM_EXTI_Detach(W2_D1_PIN);
//		}	
}	
//Delayms(10);
}
/* Called when Custom TIMER1 reaches zero */
void CustomTIMER1_Task(void* UserParameters) {
	flag_LCD=1;
	flag_485=1;
	if(flag_W1D0){ 
		timerdk1++;
	}
	if(flag_W1D1){ 
		timerdk2++;
	}
}
void read_sw_add(void)
	{
	unsigned int sw_add[8];
	int k =0;
	int i=0;
	TM_USART_Puts(USART6, "TEST funtion read sw add \n");
	if(GPIO_ReadInputDataBit(ADD_BIT0_PORT,ADD_BIT0_PIN)==0) sw_add[0] = 1;
			else sw_add[0] = 0;
	if(GPIO_ReadInputDataBit(ADD_BIT1_PORT,ADD_BIT1_PIN)==0) sw_add[1] = 1;
			else sw_add[1] = 0;
	if(GPIO_ReadInputDataBit(ADD_BIT2_PORT,ADD_BIT2_PIN)==0) sw_add[2] = 1;
			else sw_add[2] = 0;
	if(GPIO_ReadInputDataBit(ADD_BIT3_PORT,ADD_BIT3_PIN)==0) sw_add[3] = 1;
			else sw_add[3] = 0;
	if(GPIO_ReadInputDataBit(ADD_BIT4_PORT,ADD_BIT4_PIN)==0) sw_add[4] = 1;
			else sw_add[4] = 0;
	if(GPIO_ReadInputDataBit(ADD_BIT5_PORT,ADD_BIT5_PIN)==0) sw_add[5] = 1;
			else sw_add[5] = 0;
	if(GPIO_ReadInputDataBit(ADD_BIT6_PORT,ADD_BIT6_PIN)==0) sw_add[6] = 1;
			else sw_add[6] = 0;
	if(GPIO_ReadInputDataBit(ADD_BIT7_PORT,ADD_BIT7_PIN)==0) sw_add[7] = 1;
			else sw_add[7] = 0;
		

	value_dip= 1*sw_add[0]+2*sw_add[1]+4*sw_add[2]+8*sw_add[3]+16*sw_add[4]+32*sw_add[5]+64*sw_add[6]+128*sw_add[7];
	
}
	
int turn_on_dk1(){
	TM_GPIO_SetPinHigh(RELAY_DK1_PORT,RELAY_DK1_PIN);
}
int turn_off_dk1(){
	TM_GPIO_SetPinLow(RELAY_DK1_PORT,RELAY_DK1_PIN);
}
int	turn_on_dk2(){
	TM_GPIO_SetPinHigh(RELAY_DK2_PORT,RELAY_DK2_PIN);
}
int turn_off_dk2(){
	TM_GPIO_SetPinLow(RELAY_DK2_PORT,RELAY_DK2_PIN);
}
int send_485(){
	TM_GPIO_SetPinHigh(CCU_DIR_PORT,CCU_DIR_PIN);
}
int recv_485(){
	TM_GPIO_SetPinLow(CCU_DIR_PORT,CCU_DIR_PIN);
}