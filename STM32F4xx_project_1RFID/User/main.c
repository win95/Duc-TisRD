/**
 *	Keil project for TIS
 *	@author		tran nhat duc
 *	@email		ducbk55@gmail.com
 *	@website	TIS.Vn
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
#include "tm_stm32f4_usart.h"
#include "tm_stm32f4_adc.h"
#include "tm_stm32f4_hd44780.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//============================================
//  Command List, preamble + length + command 
//============================================

unsigned char  Process=0;	
	
	
unsigned char flag_RFID1=0;
unsigned char flag_RFID2=0;
unsigned char flag_PC=0;

char SelectCard[4]= {0xBA,0x02,0x01,0xB9};
char *vip_id[5] = {
	"b7 7b 87 52",
	"04 30 8a aa 3f 3e 80",
	"01 12 34 45",
	"01 12 34 45",
	"01 12 34 45"
	};
char IDCAR1[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char IDCAR2[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};
int car_number =0;
int i=0;
int LEDStatus=0;
char BufferCom1[50];
char BufferCom2[50];
char UID1[20];
char UID2[20];
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
int send_485();
int recv_485();
unsigned char flag_485 = 0;
unsigned char flag_com1 = 0;
/* Khoi INPUT*/
unsigned char flag_W1D1 = 0;
unsigned char flag_W1D0 = 0;
unsigned char flag_W2D1 = 0;
unsigned char flag_W2D0 = 0;

unsigned char flag_R11 =0;
unsigned char flag_R21 =0;
unsigned char flag_R12 =0;
unsigned char flag_R22 =0;
unsigned char flag_R31 =0;
int check_vip(char * name);

/* value DIP switch*/
unsigned char value_dip =0;
/* Khoi OUTPUT*/

int turn_on_dk1();
int turn_off_dk1();

int	turn_on_dk2();
int turn_off_dk2();

int	turn_on_dk3();
int turn_off_dk3();

int	turn_on_dk4();
int turn_off_dk4();

int timerdk1 =0;
int timerdk2 =0;
int timerdk3 =0;
int timerdk4 =0;

int timer_dk1 =0;
int timer_dk2 =0;
int timer_dk3 =0;
int timer_dk4 =0;	

/* Create 2 callback functions for custom timers */
void CustomTIMER1_Task(void* UserParameters);
void CustomTIMER2_Task(void* UserParameters);
/* Pointers to custom timers */
TM_DELAY_Timer_t* CustomTimer1;
TM_DELAY_Timer_t* CustomTimer2;

void WaitPC(unsigned int t);
void ProcessAction(void);
/* end*/




int main(void) {
		char str[100];
	
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
		TM_USART_Puts(USART3, "khoi tao ngat DFMF_BIT4\n");
	}
	
	/*init interrup INPUT*/
		if (TM_EXTI_Attach(W1_D0_PORT, W1_D0_PIN, TM_EXTI_Trigger_Rising) == TM_EXTI_Result_Ok) {
		TM_USART_Puts(USART3, "khoi tao ngat W1_D0\n");
	}
		if (TM_EXTI_Attach(W1_D1_PORT, W1_D1_PIN, TM_EXTI_Trigger_Rising) == TM_EXTI_Result_Ok) {
		TM_USART_Puts(USART3, "khoi tao ngat W1_D1\n");
	}
		if (TM_EXTI_Attach(W2_D1_PORT, W2_D1_PIN, TM_EXTI_Trigger_Falling) == TM_EXTI_Result_Ok) {
		TM_USART_Puts(USART3, "khoi tao ngat W2_D1\n");
	}
		if (TM_EXTI_Attach(W2_D0_PORT, W2_D0_PIN, TM_EXTI_Trigger_Falling) == TM_EXTI_Result_Ok) {
		TM_USART_Puts(USART3, "khoi tao ngat W2_D0\n"); // W2D0
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
	TM_GPIO_Init(RELAY_DK3_PORT, RELAY_DK3_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	TM_GPIO_Init(RELAY_DK4_PORT, RELAY_DK4_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
/* Initialize USART6 at 115200 baud, TX: PC6, RX: PC7 , COM 1 gan cong tac nguon*/ 
	TM_USART_Init(USART6, TM_USART_PinsPack_1, 115200);
/* Initialize USART3 at 115200 baud, TX: PD8, RX: PD9 ,	COM 2 gan ethernet*/
	TM_USART_Init(USART3, TM_USART_PinsPack_3, 9600);
/* Initialize USART1 at 115200 baud, TX: PA9, RX: PA10, CONG 485 */
	TM_USART_Init(USART1, TM_USART_PinsPack_1, 9600);
	
/* int DIR 485 set = send , reset = recvice*/ 
	TM_GPIO_Init(CCU_DIR_PORT, CCU_DIR_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High);
	TM_GPIO_SetPinHigh(CCU_DIR_PORT,CCU_DIR_PIN);
/* Init 2 custom timers */
/* Timer1 has reload value each 500ms, enabled auto reload feature and timer is enabled */
	CustomTimer1 = TM_DELAY_TimerCreate(500, 1, 1, CustomTIMER1_Task, NULL);
	/* Timer1 has reload value each 1000ms, enabled auto reload feature and timer is enabled */
	CustomTimer2 = TM_DELAY_TimerCreate(100, 1, 1, CustomTIMER2_Task, NULL);
/* Init LCD*/
		TM_GPIO_Init(HD44780_RW_PORT, HD44780_RW_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
		TM_GPIO_SetPinLow(HD44780_RW_PORT,HD44780_RW_PIN);

		read_sw_add();
		timeout = value_dip;
		memset(str,'\0',0);
    //Initialize LCD 20 cols x 4 rows
    TM_HD44780_Init(16, 4);
    //Save custom character on location 0 in LCD
    TM_HD44780_CreateChar(0, &customChar[0]);    
    //Put string to LCD
    TM_HD44780_Puts(0, 0, "STM32F407VET\n\rCreartbyR&D-TIS"); /* 0 dong 1, 1 dong 2*/
    TM_HD44780_Puts(0, 2, "Welcome");
		Delayms(1000);
		TM_HD44780_Clear();
		sprintf(str,"Timer out %d", timeout);
		TM_HD44780_Puts(0, 0,str);
		Delayms(1000);
		TM_HD44780_Clear();
		TM_HD44780_Puts(0, 0,"----TIS8 PRO----");
		//TM_USART_Puts(USART3, "Welcome2");

	/*creat by duc*/
		TM_WATCHDOG_Reset();
		flag_RFID1=0;
	/*end by duc*/
	while (1) {
/*process 485*/
	if(flag_485){
	flag_485=0;
	if(LEDStatus==0) TM_USART_Puts(USART1, "/LED000>\r\n");
	if(LEDStatus==1) TM_USART_Puts(USART1, "/LED001>\r\n");
	if(LEDStatus==2) TM_USART_Puts(USART1, "/LED002>\r\n");
	}
/* xu li W1D0 - dk1*/
	if(flag_W1D0){
		turn_on_dk1();
	}
/* xu li W1D1 - dk2*/
	if(flag_W1D1){
		turn_on_dk2();
	}

if(Process!=1)
		TM_HD44780_Puts(0, 2,"Wait for Card"); /* 0 dong 1, 1 dong 2*/
if(flag_RFID1==1)
		{	
		
		Process=1;
		IDCAR1[0]=BufferCom1[4];
		IDCAR1[1]=BufferCom1[5];
		IDCAR1[2]=BufferCom1[6];
		IDCAR1[3]=BufferCom1[7];
		IDCAR1[4]=BufferCom1[8];
		IDCAR1[5]=BufferCom1[9];
		IDCAR1[6]=BufferCom1[10];
		
		if(BufferCom1[1]==0x08)	
			{
			sprintf(UID1,"%02x %02x %02x %02x,1",IDCAR1[0],IDCAR1[1],IDCAR1[2],IDCAR1[3]);
			}
		if(BufferCom1[1]==0x0B) 
			{
			sprintf(UID1,"%02x %02x %02x %02x %02x %02x %02x,1",IDCAR1[0],IDCAR1[1],IDCAR1[2],IDCAR1[3],IDCAR1[4],IDCAR1[5],IDCAR1[6]);
			}
		TM_HD44780_Puts(0, 2,"Waiting PC..."); /* 0 dong 1, 1 dong 2*/
		if(check_vip(UID1)){
			flag_PC=1;
			flag_R11=1;
			timerdk1 =0;
		}
//		WaitPC(200);
		else
		TM_USART_Puts(USART3,UID1);
//		}
		WaitPC(200);
		flag_RFID1=0;
		if(flag_PC)
		{
			TM_HD44780_Puts(0, 2,"Door opened.."); /* 0 dong 1, 1 dong 2*/
			flag_PC=0;
			ProcessAction();
		}
		else Process=0;
		flag_RFID1=0;
	}

		
/**/
timer_dk1 = timerdk1/2;	
if (timer_dk1 >= timeout){
			turn_off_dk1();
			flag_R11 =0;
			flag_W1D0=0;
			timerdk1=0;
			timer_dk1=0;
			flag_RFID1=0;
			flag_RFID2=0;
			Process=0;
		}
timer_dk2 = timerdk2/2;
if (timer_dk2 >= timeout){
			turn_off_dk2();
			flag_R31 =0;
			flag_W1D1=0;
			timerdk2=0;
			timer_dk2=0;
			Process=0;
		}
timer_dk3 = timerdk3;
if (timer_dk3 >= 1){
			turn_off_dk3();
			flag_R12 =0;
			timerdk3=0;
			timer_dk3=0;
		}
timer_dk4 = timerdk4;
if (timer_dk4 >= 1){
			turn_off_dk4();
			flag_R22 =0;
			timer_dk4=0;
			timerdk4=0;
		}
		TM_WATCHDOG_Reset();
}
}


void ProcessAction(void)
{
	if(strncmp(BufferCom2,"R11",3)==0)
	{
		flag_R11=1;
		flag_R12=1;
		timerdk1 =0;
		timerdk3 =0;
	}
	
	if(strncmp(BufferCom2,"R21",3)==0)
	{
		flag_R11=1;
		flag_R22=1;
		timerdk1 =0;
		timerdk4 =0;
	}
	
	if(strncmp(BufferCom2,"R12",3)==0)
	{
		flag_R12=1;
		timerdk3 =0;
	}
	
	if(strncmp(BufferCom2,"R11",3)==0)
	{
		flag_R22=1;
		timerdk4 =0;
	}
	if(strncmp(BufferCom2,"R31",3)==0)
	{
		flag_R31=1;
		timerdk2 =0;
	}
	///* xu li flag_R11 - dk1 tu com1*/
	if(flag_R11){
		turn_on_dk1();
	}
	///* xu li flag_R31 - dk2 tu com1*/
	if(flag_R31){
		turn_on_dk2();
	}
/* xu li flag_R21 - dk1 tu com1*/
	if(flag_R21){
		turn_on_dk1();
	}
/* xu li flag_R12 - dk3 tu com1*/
	if(flag_R12){
		turn_on_dk3();

	timer_dk3 = timerdk3;
	}
/* xu li flag_R22 - dk4 tu com1*/
	if(flag_R22){
		turn_on_dk4();
	}
	

	if(flag_R11||flag_W1D0||flag_R21) timer_dk1 = timerdk1/2;
	//else timer_dk1=0;
	if(flag_R31||flag_W1D1 ) timer_dk2 = timerdk2/2;
	//else timer_dk2=0;
	if(flag_R12 ) timer_dk3 = timerdk3;
	//else timer_dk3=0;
	if(flag_R22 ) timer_dk4 = timerdk4;
	//else timer_dk4=0;	
}

void WaitPC(unsigned int t)
{
	unsigned long int n=0;
	while(n<=t){
		Delayms(10);
	if(flag_PC==1)
		{
			break;
		}
	n++;
	}
}


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
		
		TM_USART_Putc(USART3,express);
		/* Toggle RED led */
//		TM_DISCO_LedToggle(LED_RED);
//		/* Check counter */
		if (++counter >= 10) {
			/* Detach external interrupt for GPIO_Pin_0 no matter on which GPIOx is connected */
			TM_EXTI_Detach(GPIO_Pin_0);
		}
	}
	
//	/* Handle external line 13 interrupts */
	
	if (GPIO_Pin == W1_D0_PIN) { /* run W1 D0 - dieu khien RELAY_DK1_PORT , RELAY_DK1_PIN*/	
		if(!flag_W1D1)
		{ 
		flag_W1D0 =1 ;
		flag_W1D1 =0;
		timeout = value_dip;
		timer_dk1 =0;
		}
		
	}
	if (GPIO_Pin == W1_D1_PIN) {	/* run W1 D1 - dieu khien RELAY_DK2_PORT , RELAY_DK2_PIN*/
		if(!flag_W1D0) {
		flag_W1D1 =1 ;
		flag_W1D0 =0 ;
		timeout = value_dip;
		timer_dk2 =0;
		}
		
	}
	if (GPIO_Pin == W2_D1_PIN) { // ngat cac cong 
		/* run w2 D1 */
	timerdk1=(timeout*2);
	timerdk2=(timeout*2);	
}
}
/* Called when Custom TIMER1 reaches zero */
void CustomTIMER1_Task(void* UserParameters) {
	flag_485=1;
		if(flag_R11||flag_R21||flag_W1D0){
			timerdk1++;
		}
		if(flag_R31||flag_W1D1){ 
			timerdk2++;	
		}
		if(flag_R12){ 
			timerdk3++;
		}
		if(flag_R22){
			timerdk4++;
		}
	}
void CustomTIMER2_Task(void* UserParameters) 
{
//	if(Process==0){
	TM_USART_BufferEmpty(USART6);
	TM_USART_Puts(USART6,SelectCard);
	}
//	}

void TM_USART6_ReceiveHandler(uint8_t c) {	// RFID
	static uint8_t cnt=0;
	if(c==0xBD)cnt=0;
	BufferCom1[cnt]=c;
	if((BufferCom1[3]==0x00)&&(cnt==BufferCom1[3]+1)&&cnt) flag_RFID1=1;
	if(cnt<48)cnt++;

}

void TM_USART3_ReceiveHandler(uint8_t c) {	// PC
	static uint8_t cnt=0;
	if(c=='R')cnt=0;
	BufferCom2[cnt]=c;
	if((cnt==2)&&(Process==1))flag_PC=1;
	if(cnt<48)cnt++;
	else cnt =0;
}


void read_sw_add(void)
	{
	unsigned int sw_add[8];
//	int k =0;
//	int i=0;
//	TM_USART_Puts(USART6, "TEST funtion read sw add \n");
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
		
	value_dip=6;
	//value_dip= 1*sw_add[0]+2*sw_add[1]+4*sw_add[2]+8*sw_add[3]+16*sw_add[4]+32*sw_add[5]+64*sw_add[6]+128*sw_add[7];
	
}
	
int turn_on_dk1(){
	TM_GPIO_SetPinHigh(RELAY_DK1_PORT,RELAY_DK1_PIN);
	LEDStatus=1;
}
int turn_off_dk1(){
	TM_GPIO_SetPinLow(RELAY_DK1_PORT,RELAY_DK1_PIN);
	LEDStatus=0;
}
int	turn_on_dk2(){
	TM_GPIO_SetPinHigh(RELAY_DK2_PORT,RELAY_DK2_PIN);
	LEDStatus=2;
}
int turn_off_dk2(){
	TM_GPIO_SetPinLow(RELAY_DK2_PORT,RELAY_DK2_PIN);
	LEDStatus=0;
}
int	turn_on_dk3(){
	TM_GPIO_SetPinHigh(RELAY_DK3_PORT,RELAY_DK3_PIN);
}
int turn_off_dk3(){
	TM_GPIO_SetPinLow(RELAY_DK3_PORT,RELAY_DK3_PIN);
}
int	turn_on_dk4(){
	TM_GPIO_SetPinHigh(RELAY_DK4_PORT,RELAY_DK4_PIN);
}
int turn_off_dk4(){
	TM_GPIO_SetPinLow(RELAY_DK4_PORT,RELAY_DK4_PIN);
}
int send_485(){
	TM_GPIO_SetPinHigh(CCU_DIR_PORT,CCU_DIR_PIN);
}
int recv_485(){
	TM_GPIO_SetPinLow(CCU_DIR_PORT,CCU_DIR_PIN);
}
int check_vip(char * name){
	int i =0;
	for(i=0;i<5;i++){
	if(strstr(name,vip_id[i]))
		return 1;
	}
	return 0;
}