/* Include core modules */
#include "stm32f4xx.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_usart.h"
#include <stdio.h>
#include <string.h>

void init_sim5215();
void Disconnect_server();
void Connect_server();
int Check_server(int *i);
void Tin(unsigned int i);
void ATCMGS(unsigned int i);
void Sendmessge(unsigned int t,unsigned int s , char *data);
//

