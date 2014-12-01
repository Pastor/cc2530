/******************************************************************************
XWWK-CC2530A模块串口测试程序
Filename:     CC2530UART2Test.c
Target:       cc2530
Revised:      2012-01
Revision:     1.0
Description:
После включения питания XWWK тестовые данные для отправки данных на последовательный порт!
   Затем подождите, покаПК получить последовательных данных
   Настройки последовательного порта : 19200,8,1 , N
******************************************************************************/

#include <iocc2530.h>
#include <string.h>
#define uint unsigned int
#define uchar unsigned char

Определяем порт управления огни
#define RLED P1_0
#define YLED P1_1

void initUART0(void);
void InitialAD(void);
void UartTX_Send_String(uchar *Data,int len);

uchar Recdata[30]="XWWK Test Data!";
uchar RXTXflag = 1;
uchar temp;
uint  datanumber = 0;
uint  stringlen;
/****************************************************************
Инициализации последовательного порта 0 функции				
****************************************************************/
void initUART0(void)
{
    CLKCONCMD &= ~0x40;                         //Установите источник системного тактового сигнала для 32MHz кристалла
    while(CLKCONSTA & 0x40);                    //В ожидании стабильности кристаллической
    CLKCONCMD &= ~0x47;                         //Установите Master System тактовая частота которого составляет 32MHz
   
    PERCFG = 0x00;				//Место 1 P0 порт
    P0SEL = 0x3c;				//Р0 используется как последовательный
    P2DIR &= ~0XC0;                             //P0 приоритетом, поскольку UART0  
    U0CSR |= 0x80;				//Режим последовательного UART установлен в
    U0GCR |= 9;				
    U0BAUD |= 59;				//Скорость передачи данных устанавливается в 19 200
    UTX0IF = 1;                                 //Флаг UART0 TX прерывания устанавливается начальная    
    U0CSR |= 0X40;				//Флаг UART0 TX прерывания устанавливается начальная
    IEN0 |= 0x84;				//Открыть мастер прервать получить прерывание
}
/****************************************************************
Строковые функции последовательного порта			
****************************************************************/
void UartTX_Send_String(uchar *Data,int len)
{
  int j;
  for(j=0;j<len;j++)
  {
    U0DBUF = *Data++;
    while(UTX0IF == 0);
    UTX0IF = 0;
  }
}
/****************************************************************
Основная функция						
****************************************************************/
void main(void)
{	
	P1DIR = 0x03; 				//Управления P1 LED
	RLED = 0;
	YLED = 0;				//выключить Led
	initUART0();
        stringlen = strlen((char *)Recdata);
	UartTX_Send_String(Recdata,30);	            
	while(1)
	{
          if(RXTXflag == 1)			     //принимающее устройство
          {
            YLED=!YLED;				     //Получите индикации состояния
            if( temp != 0)
            {
                if((temp!='#')&&(datanumber<50))     // Определяется как конец пакета , может принять максимум 50 символов
                {          
                 Recdata[datanumber++] = temp;
                }
                else
                {
                  RXTXflag = 3;                      //Передающее устройства
                }
                if(datanumber == 50)
                  RXTXflag = 3;
              temp  = 0;
            }
          }
          if(RXTXflag == 3)			//Отправить статуса
          {
            YLED = 0;                           //Off Зеленый светодиод
            RLED = 1;			        //Отправить индикации состояния
            U0CSR &= ~0x40;			//Не взыскание долгов
            UartTX_Send_String(Recdata,datanumber);
            U0CSR |= 0x40;			//позволило получить
            RXTXflag = 1;		        //Вернуться к принимающем государстве
            datanumber = 0;			//Указатель возврат 0
            RLED = 0;			        //Off Направление инструкций
          }
	}
}
/****************************************************************
Последовательный порт для приема характер : как только данные, передаваемые через последовательный порт CC2530 , а затем введите перерыв,полученные данные присваивается переменной температуре .
****************************************************************/
#pragma vector = URX0_VECTOR
 __interrupt void UART0_ISR(void)
 {
 	URX0IF = 0;				// флаг прерывания
	temp = U0DBUF;                          
 }
