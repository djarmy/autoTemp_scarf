#include <STC8G.H>
#include <intrins.h>
//#include "SegmentLCD.h"

#include "SYSTEM/usart/usart.h"
//#include "./1_0/MainFunction.h"	 

#define uint unsigned int
#define uchar unsigned char	


 
 


unsigned char sys_power_en = 1;    /* 上电即开机: 电池/USB供电冷启动直接进自动止鼾(长按3s才切STOP关机) */ 

 

/* 简易int转十进制字符串(含结尾\0), 供xx值打印用 */
void PRS_IntToStr(char *buf, int v)
{
	char tmp[6];
	unsigned char i = 0, j = 0;
	unsigned int uv;

	if (v < 0) { buf[j++] = '-'; uv = (unsigned int)(-v); }
	else       { uv = (unsigned int)v; }

	if (uv == 0) tmp[i++] = '0';
	while (uv) { tmp[i++] = (char)('0' + uv % 10); uv /= 10; }
	while (i)  { buf[j++] = tmp[--i]; }
	buf[j] = '\0';
}


void main(void)
{	
	UART_Init(); /* 1. 诊断: WDT_CONTR只读(bit7=上次WDT复位, bit5=WDT已使能), 用于最终排除看门狗 */ 
//	UART_SendString("helloworld\r\n"); 
//	MainFunctionInit();	/* 所有硬件初始化集中到APP层 */

	while (1)
	{	     		    
//		UART_SendString("10s......\r\n"); 
	} 
}
		 
	 

 

/* INT4 中断服务函数: P3.0下降沿触发,仅用于唤醒STOP模式,中断号16 */
void INT4_ISR(void) interrupt 16
{
    /* 空函数: 仅用于唤醒, INT4标志进入中断后自动清除 */
}

