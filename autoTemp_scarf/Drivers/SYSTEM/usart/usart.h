/**
 ****************************************************************************************************
 * @file        usart.h
 * @author      xxx团队(xxx)
 * @version     V1.1
 * @date        2023-03-02
 * @brief       串口初始化代码(一般是串口1)，支持printf
 * @license     Copyright (c) 2020-2032, xxx科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:xxx开发板
 * 在线视频:www.xx.com
 * 技术论坛:www.xx.com
 * 公司网址:www.xx.com
 * 购买地址:xx.taobao.com
 *
 * 修改说明
 * V1.0 20220420
 * 第一次发布
 * V1.1 20230607
 * 修改SYS_SUPPORT_OS部分代码, 包含头文件改成:"os.h"
 * 删除USART_UX_IRQHandler()函数的超时处理和修改HAL_UART_RxCpltCallback()
 *
 ****************************************************************************************************
 */

#ifndef _USART_H
#define _USART_H

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"
#include <STC8G.H>
#include "../../../APP/1_0/Config/feature_config.h"  /* 量产/调试功能开关(相对本文件物理路径) */   

/*******************************************************************************************************/
/* 引脚和串口 定义 */

  

/*******************************************************************************************************/
#if FEATURE_UART1_DEBUG
	void UART_Init(void);
	void UART_SendChar(unsigned char ch);
	void UART_SendString(const char *str);
#else
	/* 量产构建: 串口1整体编译屏蔽, P3.1(TXD)释放为普通IO。
	   所有调用点零成本替换为空语句, 实参(含格式串)不进ROM;
	   usart.c里的实体已被#if FEATURE_UART1_DEBUG整体屏蔽, 不占程序空间。
	   注意必须用 do{}while(0) 语句形式, 不能用 ((void)0):
	   Keil C51对(void)0表达式误报C275, do-while是语句不触发,
	   且在 if(x) MACRO(); 单分支语境下同样安全(需调用点保留分号, 本工程全部带分号)。 */
	#define UART_Init()            do {} while(0)
	#define UART_SendChar(ch)      do {} while(0)
	#define UART_SendString(str)   do {} while(0)
#endif

#endif



