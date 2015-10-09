//============================================
//Test   SL025x
//PIC C source code
//PIC16F887(8MHz) + PICC 9.70
//============================================
#include <htc.h>
#include <pic16f887.h>
#include <string.h>

__CONFIG(DEBUGEN&LVPDIS&FCMEN&IESOEN&BOREN&UNPROTECT&DUNPROTECT&MCLREN&PWRTEN&WDTDIS&INTIO);

//============================================
//  PIN define
//============================================
#define RX_PIN TRISC7
#define TX_PIN TRISC6
#define Buzzer RB2
#define CARDIN_SL025x	RC0

//============================================
//  Uart status words define
//============================================
#define UARTSTATUS_FREE 0
#define UARTSTATUS_TX 1
#define UARTSTATUS_TXSUCC 2
#define UARTSTATUS_RXSUCC 3
#define UARTSTATUS_RXERR 4

//============================================
//  Card type define
//============================================
#define CARDTYPE_S50 0
#define CARDTYPE_S70 1
#define CARDTYPE_ProX 2
#define CARDTYPE_Pro 3
#define CARDTYPE_UL 4
#define CARDTYPE_DES 5
#define CARDTYPE_TAG 6
#define CARDTYPE_ICODE 7

//============================================
//  global variable define
//============================================
bank1 unsigned int g_iTimer;
bank1 unsigned char g_cTimer_Buzzer;
unsigned char g_bOverTime;
bit g_bUartTxHead;
bit g_bUartRxHead;
const unsigned char *g_pUartTxDat;
unsigned char g_cUartTxCheckSum;
unsigned char g_cUartRxCnt;
unsigned char g_cUartStatus;
bank1 unsigned char g_cRxBuf[40];
bank1 unsigned char g_cUartTxCnt;
bank1 unsigned char g_cUartTxDataLen;
bank1 unsigned char g_cCardType;

//============================================
//  Command List, preamble + length + command 
//============================================
const unsigned char SelectCard[] = {0xBA,0x02,0x01 };       
const unsigned char LoginSector0[] = {0xBA,0x0A,0x02,0x00,0xAA,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  
const unsigned char LoginSector1[] = {0xBA,0x0A,0x02,0x01,0xAA,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
const unsigned char ReadBlock1[] = {0xBA,0x03,0x03,0x01};                                     
const unsigned char WriteBlock1[] = {0xBA,0x13,0x04,0x01,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};                                
const unsigned char ReadValue[] = {0xBA,0x03,0x05,0x05};  
const unsigned char InitializeValue[]  = {0xBA,0x07,0x06,0x04,0x00,0x00,0x00,0x01};                                
const unsigned char IncrementValue[] = {0xBA,0x07,0x08,0x04,0x00,0x00,0x00,0x20};                                
const unsigned char DecrementValue[] = {0xBA,0x07,0x09,0x04,0x03,0x00,0x00,0x00};                                          
const unsigned char CopyValue[] = {0xBA,0x04,0x0A,0x04,0x05};
const unsigned char ReadULPage5[] = {0xBA,0x03,0x10,0x05};                                          
const unsigned char WriteULPage5[] = {0xBA,0x07,0x11,0x05,0x11,0x22,0x33,0x44};
const unsigned char TurnOnRedLed[] = {0xBA,0x03,0x40,0x01};     
const unsigned char TurnOffRedLed[] = {0xBA,0x03,0x40,0x00};             

//============================================
//  procedure define
//============================================
void Init_Hardware(void);
void BuzzerOn(void);
void Start_Time(unsigned int ms);
void SendBuf_UART(const unsigned char *dat,unsigned char len);
void Test_SL025x(void);

//============================================
//  initialize system hardware config
//  timer,uart,port
//============================================
void Init_Hardware(void)
{	
  OSCCON=0x71;		//8M
  while(HTS==0);		
  ANSEL=0x00;
  ANSELH=0x00;  
  TRISB2=0;
  OPTION=0x07;
  TMR0=0xb1;
  T0IE=1;
  RX_PIN=1;
  TX_PIN=1;
  Buzzer=0;
  TXSTA=0x04;
  RCSTA=0x90;
  BAUDCTL=0x08;
  SPBRG=0x10;
  SPBRGH=0;
  RCIE=1;
  TXEN=1;
  PEIE=1;
  GIE=1;
}

//============================================
//  interrupt service
//  timer interrupt
//  uart rx interrupt
//============================================
void interrupt isr(void)
{
  unsigned char dat;
  unsigned char CheckSum;
  unsigned char counter;
  if(T0IF)
  {
    T0IF=0;
    TMR0=0xb1;
    if(g_iTimer!=0)
    {
      g_iTimer--;
      if(g_iTimer==0) g_bOverTime=1;
    }
    if(g_cTimer_Buzzer!=0)
    {
      g_cTimer_Buzzer--;
      if(g_cTimer_Buzzer==0) Buzzer=0;
    }	
  }
  if(RCIF)
  {
    dat=RCREG;
    {
      g_cRxBuf[g_cUartRxCnt]=dat;
      g_cUartRxCnt++;
      if(g_cUartRxCnt==g_cRxBuf[1]+2)
      {
        CheckSum=0;
        for(counter=0;counter<g_cUartRxCnt;counter++) CheckSum=CheckSum^g_cRxBuf[counter];
        if(CheckSum==0) g_cUartStatus=UARTSTATUS_RXSUCC;
        else g_cUartStatus=UARTSTATUS_RXERR;
      }
    }
  }
}

//============================================
//  trun on buzzer about 100mS
//============================================
void BuzzerOn(void)
{
  Buzzer=1;
  g_cTimer_Buzzer=10;
}

//============================================
//  start timer
//============================================
void Start_Time(unsigned int ms)
{
  g_iTimer=ms;
  g_bOverTime=0;
}

//============================================
//  send buffer by uart
//============================================
void SendBuf_UART(const unsigned char *dat,unsigned char len)
{
  unsigned char tmp;
  g_cUartStatus=UARTSTATUS_TX;
  g_pUartTxDat=dat;
  g_cUartTxDataLen=len;
  g_cUartTxCnt=0;
  g_cUartTxCheckSum=0;
  g_bUartTxHead=0;
  while(g_cUartTxCnt!=g_cUartTxDataLen)
  {
    tmp=g_pUartTxDat[g_cUartTxCnt];
    g_cUartTxCheckSum=g_cUartTxCheckSum^tmp;
    g_cUartTxCnt++;
    while(TXIF==0);
    TXREG=tmp;
  }
  g_cUartStatus=UARTSTATUS_TXSUCC;
  g_cUartRxCnt=0;
  while(TXIF==0);
  TXREG=g_cUartTxCheckSum;
}

#define RCVCMD_SL025x g_cRxBuf[2]
#define RCVSTA_SL025x g_cRxBuf[3]

void Test_SL025x(void)
{
  unsigned long int lPurseValue;
  g_cCardType=0xff;
  if(CARDIN_SL025x) return;
  SendBuf_UART(SelectCard,sizeof(SelectCard));	
  Start_Time(30);
  while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
  if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x01)||(RCVSTA_SL025x!=0)) return;
  if(g_cRxBuf[g_cRxBuf[1]]==1) g_cCardType=CARDTYPE_S50;        //Mifare 1k 4 byte UID
  else if(g_cRxBuf[g_cRxBuf[1]]==2) g_cCardType=CARDTYPE_S50;   //Mifare 1k 7 byte UID
  else if(g_cRxBuf[g_cRxBuf[1]]==3) g_cCardType=CARDTYPE_UL;    //Ultralight 7 byte UID
  else if(g_cRxBuf[g_cRxBuf[1]]==4) g_cCardType=CARDTYPE_S70;   //Mifare 4k 4 byte UID
  else if(g_cRxBuf[g_cRxBuf[1]]==5) g_cCardType=CARDTYPE_S70;   //Mifare 4k 7 byte UID
  else if(g_cRxBuf[g_cRxBuf[1]]==6) g_cCardType=CARDTYPE_DES;   //DesFire 7 byte UID
  switch(g_cCardType)
  {
    case CARDTYPE_S50:
    case CARDTYPE_S70:
      //Verify password of sector0   
      SendBuf_UART(LoginSector0,sizeof(LoginSector0));
      Start_Time(30);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x02)||(RCVSTA_SL025x!=0x02)) return;
      //Write data to block1			
      SendBuf_UART(WriteBlock1,sizeof(WriteBlock1));
      Start_Time(30);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x04)||(RCVSTA_SL025x!=0)) return;
      if(memcmp(&WriteBlock1[4],&g_cRxBuf[4],16)!=0) return;
      //Read data from block1
      SendBuf_UART(ReadBlock1,sizeof(ReadBlock1));
      Start_Time(30);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x03)||(RCVSTA_SL025x!=0)) return;
      if(memcmp(&WriteBlock1[4],&g_cRxBuf[4],16)!=0) return;
      //Verify password of sector1
      SendBuf_UART(LoginSector1,sizeof(LoginSector1));
      Start_Time(30);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x02)||(RCVSTA_SL025x!=0x02)) return;
      //Initialize block4 to one purse, and value = 0x01000000
      SendBuf_UART(InitializeValue,sizeof(InitializeValue));
      Start_Time(30);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x06)||(RCVSTA_SL025x!=0)) return;
      if(memcmp(&InitializeValue[4],&g_cRxBuf[4],4)!=0) return;
      //Increment
      SendBuf_UART(IncrementValue,sizeof(IncrementValue));
      Start_Time(30);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x08)||(RCVSTA_SL025x!=0)) return;
      //Decrement
      SendBuf_UART(DecrementValue,sizeof(DecrementValue));
      Start_Time(30);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x09)||(RCVSTA_SL025x!=0)) return;
      //Backup purse to blcok5
      SendBuf_UART(CopyValue,sizeof(CopyValue));
      Start_Time(30);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x0a)||(RCVSTA_SL025x!=0)) return;
      //Read purse value from blcok5			   
      SendBuf_UART(ReadValue,sizeof(ReadValue));
      Start_Time(30);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x05)||(RCVSTA_SL025x!=0)) return;
      //Check value	 
      lPurseValue=g_cRxBuf[7];
      lPurseValue=(lPurseValue<<8)+g_cRxBuf[6];
      lPurseValue=(lPurseValue<<8)+g_cRxBuf[5];
      lPurseValue=(lPurseValue<<8)+g_cRxBuf[4];
      if(lPurseValue!=0x01000000+0x20000000-0x00000003) return;
      BuzzerOn();
      //Glare Red_Led to indicate working ok
      SendBuf_UART(TurnOnRedLed,sizeof(TurnOnRedLed));
      Start_Time(30);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      Start_Time(30);
      while(g_bOverTime==0);
      SendBuf_UART(TurnOffRedLed,sizeof(TurnOffRedLed));
      Start_Time(30);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      break;
    case CARDTYPE_UL:
      SendBuf_UART(WriteULPage5,sizeof(WriteULPage5));
      Start_Time(30);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x11)||(RCVSTA_SL025x!=0)) return;
      SendBuf_UART(ReadULPage5,sizeof(ReadULPage5));
      Start_Time(30);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      if((g_cUartStatus!=UARTSTATUS_RXSUCC)||(RCVCMD_SL025x!=0x10)||(RCVSTA_SL025x!=0)) return;
      if (memcmp(&WriteULPage5[4],&g_cRxBuf[4],4)!=0) return;
      BuzzerOn();
      //Glare Red_Led to indicate working ok
      SendBuf_UART(TurnOnRedLed,sizeof(TurnOnRedLed));
      Start_Time(30);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      Start_Time(30);
      while(g_bOverTime==0);
      SendBuf_UART(TurnOffRedLed,sizeof(TurnOffRedLed));
      Start_Time(30);
      while((g_cUartStatus!=UARTSTATUS_RXERR)&&(g_cUartStatus!=UARTSTATUS_RXSUCC)&&(g_bOverTime==0));
      break;
    default:
      break;
    }
}

void main(void)
{
  Init_Hardware();
  g_iTimer=0;
  while(1) Test_SL025x();
}

