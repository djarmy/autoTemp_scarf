/**
 ****************************************************************************************************
 * @file        usart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2023-03-02
 * @brief       串口初始化代码(一般是串口1)，支持printf
 * @license     Copyright (c) 2020-2032, xx电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:xx开发板
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

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"

/* 量产构建(FEATURE_UART1_DEBUG=0): usart.h已把UART_Init/UART_SendChar/UART_SendString
   定义为空宏, 本文件整个实现必须一起屏蔽, 否则宏名与函数定义冲突(C307/C141/C231)。
   调试构建: 正常编译全部实现。宏经usart.h -> feature_config.h传入。 */
#if FEATURE_UART1_DEBUG

#include <stdio.h> // 必须包含此头文件以声明FILE类型

 

/* 如果使用os,则包括下面的头文件即可. */
#if SYS_SUPPORT_OS
#include "os.h"   /* os 使用 */
#endif

/******************************************************************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if 1
 

#else
/* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

 		 

/******************************************************************************************/
/* 打印 */


/**
 * @brief 初始化UART串口1，模式1，波特率9600，11.0592MHz
 * @note 定时器1作为波特率发生器，8位自动重装；串口1映射P3.0(RX),P3.1(TX)
 */
void UART_Init(void) {
    // 串口引脚切换：P_SW1=0x00，串口1使用P3.0(RXD) P3.1(TXD)
    P_SW1 = 0x00;

    // 1. 定时器1配置：模式2，8位自动重装
    TMOD &= 0x0F;
    TMOD |= 0x20;

    // 11.0592，9600，SMOD=0，TH1=0xFD
    TH1 = 0xFD;
    TL1 = 0xFD;
    TR1 = 1;

    // ========== STC8G关键配置 ==========
//    AUXR |= 0x40;    	 	// T1x12=1：定时器1【关闭12分频，1T模式】
    AUXR &= 0xFE;   		 // 串口1波特率源：选择定时器1（AUXR BIT0=0，STC8G！！）

    // 2. 串口1配置：SCON = 0x50 方式1，REN=1允许接收
    SCON = 0x50;
    PCON &= ~0x80;  		// SMOD=0，波特率不加倍
	ET1=0;    				// 禁止 Timer1 中断！ 波特率发生器不需要中断
    // 如需开启串口接收中断，打开下面两行
    // ES = 1;
    // EA = 1;
}




/**
 * @brief 通过UART发送一个字符
 * @param ch 要发送的字符
 */
void UART_SendChar(unsigned char ch) 
{
	unsigned int timeout = 0;
    SBUF = ch;      // 将字符写入发送缓冲区，硬件自动开始发送
    while (!TI) { if (++timeout > 2000) break; }     // 等待发送完成中断标志置位	 超时保护:Timer1被VCC毛刺打停时防止永久挂死
    TI = 0;         // 必须软件清零发送中断标志
}
 
 
/* ===== UART_SendString 防死等版本(替换原 TI 忙等) =====
   背景: 泵/线圈关断毛刺会清零TR1(MAIN.c:84注释), TI永不置位,
         UART_SendString会永久卡死在任务内, 主循环的TR0/TR1/EA看门狗轮不到执行。
   本版本: 单字节超时 -> 检查TR1/EA -> 现场UART_Init自救重发, 最多3次, 仍不行放弃本次发送。 */
void UART_SendString(char *s)
{
    unsigned int  guard;          /* 单字节超时计数 */
    unsigned char retry;          /* 自救重试次数 */

    while (*s)
    {
        retry = 0;
        TI = 0;
        SBUF = *s;
        guard = 0;
        while (!TI)
        {
            if (++guard > 10000)                    /* ~10ms(11.0592MHz)未发完1字节=波特率时钟死了 */
            {
                if (!TR1 && retry < 3)              /* TR1被毛刺清零: 现场自救, 不依赖主循环 */
                {
                    UART_Init();                    /* 重配SCON/TMOD等, 顺带修掉SCON被毛刺打坏的情况 */
                    TR1 = 1;
                    if (!EA) EA = 1;                /* EA若同时被清零, 一并恢复 */
                    UART_SendString("UART: TR1 lost, recovered\r\n"); /* 诊断打印, 确认根因后可删 */
                    retry++;
                    TI = 0;
                    SBUF = *s;                      /* 重发当前字节 */
                    guard = 0;
                }
                else
                {
                    return;                         /* 放弃本次发送, 任务不卡死, 主循环得以运转 */
                }
            }
        }
        s++;
    }
}


 

 
/**/


 
/**
 * @brief Keil C51 重定向putchar，用于printf输出到串口
 * @param ch 待发送字符
 * @return 字符
 */
char putchar(char ch)
{
    UART_SendChar((unsigned char)ch);
    return ch;
}


 

#endif /* FEATURE_UART1_DEBUG */ 





