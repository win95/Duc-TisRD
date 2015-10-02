#include "sim5215.h"
#include "defines.h"
void init_sim5215()
{
	if(debug) TM_USART_Puts(USART2, "Start Init 3G \n");

	TM_USART_Puts(USART1, "AT\r");
	Delayms(1000);
	TM_USART_Puts(USART1, "AT+NETOPEN=\"TCP\"\r");
	Delayms(5000);
	TM_USART_Puts(USART1,"AT+TCPCONNECT=\"52.25.161.238\",6069\r");
	Delayms(5000);
	TM_USART_Puts(USART1, "AT+TCPCLOSE=\"TCP\"\r");
	Delayms(1000);
	TM_USART_Puts(USART1, "AT+NETCLOSE=\"TCP\"\r");
	//TM_USART_Puts(USART2, "AT\n");
	if(debug) TM_USART_Puts(USART2, "Complete Full\n");
}

void Connect_server()
{
	if(debug) TM_USART_Puts(USART2, "Start Init 3G \n");
	Delayms(10000);
	TM_USART_Puts(USART1,"AT\r");
	Delayms(1000);
	TM_USART_Puts(USART1, "AT+NETOPEN=\"TCP\"\r");
	Delayms(7000);
	TM_USART_Puts(USART1, "AT\r");
	Delayms(1000);	
	TM_USART_Puts(USART1,"AT+TCPCONNECT=\"52.25.161.238\",6069\r");
	Delayms(7000);
	TM_USART_Puts(USART1, "AT\r");
	Delayms(1000);	
	if(debug) TM_USART_Puts(USART2, "Complete Full\n");
}

int Check_server(int *i)
{
	TM_USART_Puts(USART1,"AT+TCPCONNECT=?");
	if((&i))
		return -1;
	else
		return 1;
}




void Disconnect_server()
{
	if(debug) TM_USART_Puts(USART2, "REStart disconnect\n");
	TM_USART_Puts(USART1, "AT\r");
	Delayms(3000);
	TM_USART_Puts(USART1, "AT+TCPCLOSE\r");
	Delayms(3000);
	TM_USART_Puts(USART1, "AT\r");
	Delayms(1000);
	TM_USART_Puts(USART1, "AT+NETCLOSE\r");
	Delayms(3000);
	TM_USART_Puts(USART1, "AT\r");
	Delayms(1000);	
	if(debug) TM_USART_Puts(USART2, "Begin Start \n");
}

void Tin(unsigned int i) // lua chon tin gui
{
switch(i)
{
case 1:
      TM_USART_Puts(USART1,"Khoi Dong \r");break;
case 2:
      TM_USART_Puts(USART1,"tao va may\r");break;
case 3:
      TM_USART_Puts(USART1,"Thong bao \r3");break;
case 4:
      TM_USART_Puts(USART1,"Thong bao 4");break;
default :break;
}
}
void ATCMGS(unsigned int i)  // lua chon so dien thoai
{
switch(i)
{
case 1:
TM_USART_Puts(USART1,"AT+CMGS=\"0988716092\"\r");break;
case 2:
TM_USART_Puts(USART1,"AT+CMGS=\"0988716092\"\r");break;
case 3:
TM_USART_Puts(USART1,"AT+CMGS=\"0988716092\"\r");break;
case 4:
TM_USART_Puts(USART1,"AT+CMGS=\"0988716092\"\r");break;
case 5:
TM_USART_Puts(USART1,"AT+CMGS=\"0988716092\"\r");break;
default : break;
}
}
void Sendmessge(unsigned int s,unsigned int t, char *data)
{

Delayms(500);
TM_USART_Puts(USART1,"AT+CMGF=1\r");
Delayms(500);
TM_USART_Putc(USART1,13);
Delayms(500);
ATCMGS(s);// lua chon sdt
Delayms(500);
TM_USART_Putc(USART1,13);
Delayms(500);
if(t) Tin(t);// lua chon tin nhan
else TM_USART_Puts(USART1,data);
Delayms(500);
TM_USART_Putc(USART1,26);
TM_USART_Putc(USART1,13);
Delayms(5000);
}