////////////////////////////////////////////////////////////
//Test SL025x
//ARM C source code
//LM3S600(6MHz) + EWARM 5.30
////////////////////////////////////////////////////////////   
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_uart.h"
#include "hw_ints.h"
#include "string.h"
#include "sysctl.h"
#include "gpio.h"
#include "systick.h"
#include "interrupt.h"
#include "uart.h"

//============================================
//	Uart bound define
//============================================
#define  BOUND4800         4800
#define  BOUND9600         9600
#define  BOUND14400        14400
#define  BOUND19200        19200
#define  BOUND28800        28800
#define  BOUND38400        38400
#define  BOUND57600        57600
#define  BOUND115200       115200

//============================================
//	Uart Status define
//============================================
#define UARTSTATUS_FREE    0
#define UARTSTATUS_TX      1
#define UARTSTATUS_TXSUCC  2
#define UARTSTATUS_RXSUCC  3
#define UARTSTATUS_RXERR   4

//============================================
//  Command List, preamble + length + command 
//============================================
const unsigned char __packed SelectCard[]=      {0xBA,0x02,0x01 };       
const unsigned char __packed LoginSector0[]=    {0xBA,0x0A,0x02,0x00,0xAA,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  
const unsigned char __packed LoginSector1[]=    {0xBA,0x0A,0x02,0x01,0xAA,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
const unsigned char __packed ReadBlock1[]=      {0xBA,0x03,0x03,0x01};                                     
const unsigned char __packed WriteBlock1[]=     {0xBA,0x13,0x04,0x01,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};                                
const unsigned char __packed ReadValue[]=       {0xBA,0x03,0x05,0x05};  
const unsigned char __packed InitializeValue[]=	{0xBA,0x07,0x06,0x04,0x00,0x00,0x00,0x01};                                
const unsigned char __packed IncrementValue[]=  {0xBA,0x07,0x08,0x04,0x00,0x00,0x00,0x20};                                
const unsigned char __packed DecrementValue[]=  {0xBA,0x07,0x09,0x04,0x03,0x00,0x00,0x00};                                          
const unsigned char __packed CopyValue[]=       {0xBA,0x04,0x0A,0x04,0x05};
const unsigned char __packed ReadULPage5[]=     {0xBA,0x03,0x10,0x05};                                          
const unsigned char __packed WriteULPage5[]=    {0xBA,0x07,0x11,0x05,0x11,0x22,0x33,0x44};
const unsigned char __packed TurnOnRedLed[]=    {0xBA,0x03,0x40,0x01};     
const unsigned char __packed TurnOffRedLed[]=   {0xBA,0x03,0x40,0x00};             

//============================================
//  global variable define
//============================================
unsigned char g_cCardType;
unsigned long int g_iTimer;
unsigned char g_bOverTime;
unsigned char g_cUartTxCnt;
unsigned char g_cUartTxDataLen;
unsigned char *g_pUartTxDat;
unsigned char g_cUartTxCheckSum;
unsigned char g_cUartRxCnt;
unsigned char g_cUartRxBuf[60];
unsigned char g_cUartStatus;

//============================================
//  i/o port define
//============================================
#define CARDIN_SL025 GPIO_PORTD_BASE,GPIO_PIN_2

//============================================
//  procedure define
//============================================
void InitializeSystem(); 
void Start_Time(unsigned long int ms);
void Stop_Time();
void Test_SL025x();

void main()
{ InitializeSystem();
  g_iTimer=0;
  while(1) Test_SL025x();
}

//============================================
//	system clock interrupt procedure
//============================================
void SysTickIntHandler(void)
{ if(g_iTimer!=0)
  { g_iTimer--;
    if(g_iTimer==0) g_bOverTime=1;
  }
}

//============================================
//  initialize system hardware config
//  system clock,uart0
//============================================
void InitializeSystem()
{	
  //init sysclock
  //period=1ms
  SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_6MHZ);
  SysTickPeriodSet(SysCtlClockGet()/1000);
  SysTickEnable();
  SysTickIntEnable();
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  
  //CARD_IN PD2
  GPIODirModeSet(GPIO_PORTD_BASE,GPIO_PIN_2,GPIO_DIR_MODE_IN);
  GPIOPadConfigSet(GPIO_PORTD_BASE,GPIO_PIN_2,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
  
  //init Uart
  //baud=115200
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1);
  UARTDisable(UART0_BASE);
  UARTIntClear(UART0_BASE,UART_INT_OE|UART_INT_BE|UART_INT_PE|UART_INT_FE|UART_INT_RT|UART_INT_TX|UART_INT_RX);
  UARTIntDisable(UART0_BASE,UART_INT_OE|UART_INT_BE|UART_INT_PE|UART_INT_FE|UART_INT_RT|UART_INT_TX|UART_INT_RX);
  UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(),BOUND115200,(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));
  UARTFIFOLevelSet(UART0_BASE,UART_FIFO_TX1_8,UART_FIFO_RX1_8);
  HWREG(UART0_BASE+UART_O_LCRH) &= ~(UART_LCRH_FEN);
  UARTIntEnable(UART0_BASE,UART_INT_RX|UART_INT_TX);
  IntEnable(INT_UART0);
  g_cUartStatus=UARTSTATUS_FREE;
  g_cUartRxCnt=0;
  
  //interrupt enable
  IntMasterEnable();
}

//============================================
//  start timer
//============================================
void Start_Time(unsigned long int ms)
{ g_iTimer=ms;
  g_bOverTime=0;
}

//============================================
//  stop timer
//============================================
void Stop_Time()
{ g_iTimer=0;
}

void UARTISR_TX025x()
{ unsigned char tmp;
  if(g_cUartTxCnt==g_cUartTxDataLen)
  { if(g_cUartStatus==UARTSTATUS_TX)
    { g_cUartStatus=UARTSTATUS_TXSUCC;
      g_cUartRxCnt=0;
      UARTCharPutNonBlocking(UART0_BASE,g_cUartTxCheckSum);
    }
  }
  else
  { tmp=g_pUartTxDat[g_cUartTxCnt];
    g_cUartTxCheckSum=g_cUartTxCheckSum^tmp;
    g_cUartTxCnt++;
    UARTCharPutNonBlocking(UART0_BASE,tmp);
  }
}

void UARTISR_RX025x(unsigned char dat)
{ unsigned char CheckSum;
  unsigned char counter;
  g_cUartRxBuf[g_cUartRxCnt]=dat;
  g_cUartRxCnt++;
  if(g_cUartRxCnt==g_cUartRxBuf[1]+2)
  { CheckSum=0;
    for(counter=0;counter<g_cUartRxCnt;counter++) CheckSum=CheckSum^g_cUartRxBuf[counter];
    if(CheckSum==0) g_cUartStatus=UARTSTATUS_RXSUCC;
    else g_cUartStatus=UARTSTATUS_RXERR;
  }
}

void UART0_ISR(void)
{ unsigned long flag_int;
  unsigned char tmp;
  flag_int=UARTIntStatus(UART0_BASE,true);
  UARTIntClear(UART0_BASE,flag_int);
  if((flag_int&UART_INT_TX)==UART_INT_TX) UARTISR_TX025x();
  if((flag_int&UART_INT_RX)==UART_INT_RX) 
  { tmp=UARTCharGetNonBlocking(UART0_BASE);
    UARTISR_RX025x(tmp);
  }
}

//============================================
//  send buffer by uart0
//============================================
void SendBuf_UART(unsigned char *dat,unsigned char len)
{ g_cUartStatus=UARTSTATUS_TX;
  g_pUartTxDat=dat;
  g_cUartTxDataLen=len;
  g_cUartTxCnt=0;
  g_cUartTxCheckSum=0;
  UARTISR_TX025x();
}

//============================================
//  SL025x1 test procedure
//============================================
void Test_SL025x()
{	
  #define RCVCMD_SL025x g_cUartRxBuf[2]
  #define RCVSTA_SL025x g_cUartRxBuf[3]
  unsigned long int lPurseValue;
  unsigned char g_cErr;
  g_cCardType=0xff;
  
  if((GPIOPinRead(CARDIN_SL025)&GPIO_PIN_2)==GPIO_PIN_2) return;
  
  //Get the unique serial number of Mifare Card 
  SendBuf_UART((unsigned char *)(SelectCard),sizeof(SelectCard));	
  Start_Time(300);
  while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
  Stop_Time();	
  if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x01)||(RCVSTA_SL025x!=0)) return;
  g_cCardType=g_cUartRxBuf[g_cUartRxBuf[1]];
  
  switch(g_cCardType)
  { 
    case 1:    //Mifare 1k 4 BYTE UID
    case 2:    //Mifare 1k 7 BYTE UID
    case 4:    //Mifare 4k 4 BYTE UID
    case 5:    //Mifare 4k 7 BYTE UID 
      //Verify password of sector0   
      SendBuf_UART((unsigned char *)(LoginSector0),sizeof(LoginSector0));
      Start_Time(300);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      Stop_Time();	
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x02)||(RCVSTA_SL025x!=0x02)) return;
      
      //Write data to block1			
      SendBuf_UART((unsigned char *)(WriteBlock1),sizeof(WriteBlock1));
      Start_Time(300);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      Stop_Time();	
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x04)||(RCVSTA_SL025x!=0)) return;
      if(memcmp(&WriteBlock1[4],&g_cUartRxBuf[4],16)!=0) return;
      
      //Read data from block1
      SendBuf_UART((unsigned char *)(ReadBlock1),sizeof(ReadBlock1));
      Start_Time(300);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      Stop_Time();	
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x03)||(RCVSTA_SL025x!=0)) return;
      if(memcmp(&WriteBlock1[4],&g_cUartRxBuf[4],16)!=0) return;
      
      //Verify password of sector1
      SendBuf_UART((unsigned char *)(LoginSector1),sizeof(LoginSector1));
      Start_Time(300);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      Stop_Time();	
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x02)||(RCVSTA_SL025x!=0x02)) return;
      
      //Initialize block4 to one purse, and value = 0x01000000
      SendBuf_UART((unsigned char *)(InitializeValue),sizeof(InitializeValue));
      Start_Time(300);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      Stop_Time();	
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x06)||(RCVSTA_SL025x!=0)) return;
      if(memcmp(&InitializeValue[4],&g_cUartRxBuf[4],4)!=0) return;
      
      //Increment
      SendBuf_UART((unsigned char *)(IncrementValue),sizeof(IncrementValue));
      Start_Time(300);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      Stop_Time();	
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x08)||(RCVSTA_SL025x!=0)) return;
      
      //Decrement
      SendBuf_UART((unsigned char *)(DecrementValue),sizeof(DecrementValue));
      Start_Time(300);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      Stop_Time();	
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x09)||(RCVSTA_SL025x!=0)) return;
      
      //Backup purse to blcok5
      SendBuf_UART((unsigned char *)(CopyValue),sizeof(CopyValue));
      Start_Time(300);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      Stop_Time();	
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x0a)||(RCVSTA_SL025x!=0)) return;
      
      //Read purse value from blcok5			   
      SendBuf_UART((unsigned char *)(ReadValue),sizeof(ReadValue));
      Start_Time(300);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      Stop_Time();	
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x05)||(RCVSTA_SL025x!=0)) return;
      
      //Check value	 
      lPurseValue=g_cUartRxBuf[7];
      lPurseValue=(lPurseValue<<8)+g_cUartRxBuf[6];
      lPurseValue=(lPurseValue<<8)+g_cUartRxBuf[5];
      lPurseValue=(lPurseValue<<8)+g_cUartRxBuf[4];
      if(lPurseValue!=0x01000000+0x20000000-0x00000003) return;
      g_cErr=0;
      break;
    

    case 3:      //Mifare_UltraLight
      SendBuf_UART((unsigned char *)(WriteULPage5),sizeof(WriteULPage5));
      Start_Time(300);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      Stop_Time();	
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x11)||(RCVSTA_SL025x!=0)) return;
      SendBuf_UART((unsigned char *)(ReadULPage5),sizeof(ReadULPage5));
      Start_Time(300);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      Stop_Time();	
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x10)||(RCVSTA_SL025x!=0)) return;
      if (memcmp(&WriteULPage5[4],&g_cUartRxBuf[4],4)!=0) return;
      g_cErr=0;
      break;
    
    
    case 2:      //Mifare_Pro
    case 5:      //Mifare_ProX
    case 6:      //Mifare_DesFire 
      g_cErr=4;
      break;
    
    
    default:
      g_cErr=3;
      break;
  }
  if(g_cErr!=0) return;
  
  //Glare Red_Led to indicate working ok
  SendBuf_UART((unsigned char *)(TurnOnRedLed),sizeof(TurnOnRedLed));
  Start_Time(300);
  while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
  Stop_Time();
  Start_Time(300);
  while(g_bOverTime==0);
  Stop_Time();
  SendBuf_UART((unsigned char *)(TurnOffRedLed),sizeof(TurnOffRedLed));
  Start_Time(300);
  while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
  Stop_Time();
}


