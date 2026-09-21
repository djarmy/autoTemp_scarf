/***********************************************************************
*                    xx科技有限公司
*						  版权所有  侵权必究
************************************************************************
*创建者：dxl
*创建时间：2026/9/12
************************************************************************
*修改者：
*修改时间：
************************************************************************/
 
#include "./1_0/ConsumerUser/indicatorLED1.h"
									    
/* 实例化应用运行数据结构, 全局唯一, 各业务层通过  访问 */
 

/* 实例化xx功能数据结构，全局唯一, 各业务层通过   访问 */  
 
/* 实例化xx功能数据结构，并获取地址 */

										    
#include "./BSP/key/key.h"              /* g_key_tick: Timer0 1ms节拍 */
#include "./SYSTEM/usart/usart.h"
   

/* 模块状态(放xdata, 避开data段OVERLAY覆盖区)（static 变量不能定义在头文件） */
#if FEATURE_USER_LED1

/* 模块状态(xdata, 唯一实体, 仅本模块函数访问; 不可放.h否则每包含一次生成一份副本) */
static unsigned char xdata s_led_mode     = LED1_MODE_OFF;
static unsigned int  xdata s_blink_remain = 0;  /* 限时 "眨眼闪烁剩余时长"(每10ms减10); 0=永久闪烁 */
static unsigned int  xdata s_blink_on     = 600;/* 当前眨眼闪烁亮段ms */
static unsigned int  xdata s_blink_off    = 600;/* 当前眨眼闪烁灭段ms(对称模式=on) */
static unsigned int  xdata s_toggle_at    = 0;  /* 上次电平翻转时刻(g_key_tick刻度) */

#define LED_TICK_MS  10U   /* indicatorLED1_tick调用周期=槽0的10ms, 改任务周期时必须同步  "用户指示灯LED1_任务调度节拍周期" */

void indicatorLED1_init(void)
{
    P3M0 |= 0x02;      /* P3.1 推挽输出: bit1 M0=1 (量产: 串口1已屏蔽, P3.1作LED普通IO) */
    P3M1 &= ~0x02;	   /*              bit1 M1=0 */
    INDICATOR_LED1_PIN = 0;
    s_led_mode = LED1_MODE_OFF;
    s_blink_remain = 0;
}

void indicatorLED1_on(void)
{
    s_led_mode = LED1_MODE_ON;
    INDICATOR_LED1_PIN = 1;
}

void indicatorLED1_off(void)
{
    s_led_mode = LED1_MODE_OFF;
    s_blink_remain = 0;
    INDICATOR_LED1_PIN = 0;
}

void indicatorLED1_read(void)
{
    UART_SendString(INDICATOR_LED1_PIN ? "LED1: ON\r\n" : "LED1: OFF\r\n");
}

/* 限时对称闪烁: duration_ms到点自动熄灭; 亮/灭各period_ms */
void indicatorLED1_blink(unsigned int duration_ms, unsigned int period_ms)
{
    s_blink_remain = duration_ms;
    s_blink_on  = period_ms;
    s_blink_off = period_ms;
    s_toggle_at = g_key_tick;
    INDICATOR_LED1_PIN = 1;           /* 立即点亮, 给用户即时反馈 */
    s_led_mode = LED1_MODE_BLINK;
}

/* 永久对称闪烁(自动止鼾运行指示): half_ms亮 half_ms灭, 直到切模式 */
void indicatorLED1_blink_forever(unsigned int half_ms)
{
    s_blink_remain = 0;               /* 0=永久 */
    s_blink_on  = half_ms;
    s_blink_off = half_ms;
    s_toggle_at = g_key_tick;
    INDICATOR_LED1_PIN = 1;
    s_led_mode = LED1_MODE_BLINK_FOREVER;
}

/* 永久非对称闪烁(四击自检): on_ms亮 off_ms灭 */
void indicatorLED1_blink_asym_forever(unsigned int on_ms, unsigned int off_ms)
{
    s_blink_remain = 0;
    s_blink_on  = (on_ms  == 0) ? 1U : on_ms;
    s_blink_off = (off_ms == 0) ? 1U : off_ms;
    s_toggle_at = g_key_tick;
    INDICATOR_LED1_PIN = 1;
    s_led_mode = LED1_MODE_BLINK_ASYM;
}

unsigned char indicatorLED1_isBusy(void)
{
    return (s_led_mode == LED1_MODE_BLINK && s_blink_remain != 0) ? 1 : 0;
}

/* 10ms周期任务(槽0): 非阻塞倒计时+翻转, 禁止Delay   "用户指示灯LED1_节拍槽函数" */
void indicatorLED1_tick(void)
{
    unsigned int now = g_key_tick;
    unsigned int half;

    switch(s_led_mode)
    {
        case LED1_MODE_OFF:
            INDICATOR_LED1_PIN = 0;
            break;
        case LED1_MODE_ON:
            INDICATOR_LED1_PIN = 1;
            break;
        case LED1_MODE_BLINK:
            /* 限时窗口先扣剩余, 到0熄灭(<=10ms时直接归零, 防无符号下翻) */
            if(s_blink_remain > LED_TICK_MS)
                s_blink_remain -= LED_TICK_MS;
            else
            {
                s_blink_remain = 0;
                s_led_mode = LED1_MODE_OFF;
                INDICATOR_LED1_PIN = 0;
                break;
            }
             /* case末尾 没有 break，fall through穿透: 本拍继续做翻转判定 */   //  故意不写break，直接顺序往下执行case

        case LED1_MODE_BLINK_FOREVER:
        case LED1_MODE_BLINK_ASYM:
            /* 当前亮->用亮段阈值; 当前灭->用灭段阈值(对称模式两者相等) */
            half = INDICATOR_LED1_PIN ? s_blink_on : s_blink_off;
            if((unsigned int)(now - s_toggle_at) >= half)
            {
                s_toggle_at = now;
                INDICATOR_LED1_PIN = !INDICATOR_LED1_PIN;
            }
            break;
    }
}

#endif /* FEATURE_USER_LED1 */

