/**
 ****************************************************************************************************
 * @file        key.h
 * @author      dxl
 * @version     V1.0
 * @date        2026-09-11
 * @brief         驱动代码( "单刀单掷矩形按钮BX-TS-26-3635TT" SPST_button1);
 					( )
					[ 
					]
 						   
 * @license     Copyright (c) 2026-2032, JX有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:xx开发板
 * 在线视频:www.xx.com
 * 技术论坛:www.xx.com
 * 公司网址:www.xx.com
 * 购买地址:xx.xx.com
 *
 * 修改说明
 * V1.0 2026911
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __KEY_H
#define __KEY_H

#include "./SYSTEM/sys/sys.h"
#include "STC8G.h"


/******************************************************************************************/
/* 引脚 定义 */

#define KEY_DEBOUNCE_MS     40    //消抖时间
#define KEY_LONG_PRESS_MS   3000  //长按判定时间
#define COMBO_WAIT_MS       500   //连击窗口等待时间
#define KEY_PRESS_MAX_MS    5000  //按下计时上限

/*	我推荐的使用命名:
	sys_power_en 系统电源 / 工作使能：1 开机，0 关机休眠
	flag_shutting_down 正在关机标志
	sys_timer_cnt 系统备用计数，按需改名
	key_action_code 按键动作命令码，给主循环解析
*/ 

/*   端口定义 */
/* SPST_button1: P3.0, 按钮另一端接GND, 低电平有效(0=按下,1=松开) */
sbit SPST_button1_PIN = P3^0;
 
 

/*  取反定义 */
 

/* 按键事件枚举状态机*/
typedef enum
{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_SHORT_CLICK,    //单击
    KEY_EVENT_DOUBLE_CLICK,   //双击
    KEY_EVENT_TRIPLE_CLICK,   //三击
	KEY_EVENT_Quad_CLICK,	  //四连击
    KEY_EVENT_LONG_PRESS,     //长按
}KeyEvent_t;					 // "按键事件枚举KeyEvent_t"
extern KeyEvent_t xdata g_key_event; //全局事件变量，主循环读取(实体定义在key.c)

extern volatile bit g_task_tick_flag;
extern volatile unsigned int xdata g_key_tick;  /* Timer0 1ms节拍计数(实体在key.c第10行), 供LED等非阻塞计时模块extern引用 */

/****************************************************************************************************************/
/* xx的寄存器地址 */
/* 详细说明参照xx数据手册 */



/****************************************************************************************************************/
/* 函数声明 */ 
/* 
 
*/
void SPST_button1_init(void);

/*
	 
*/
void SPST_button1_on(void);
/*
	 
*/
void SPST_button1_off(void);
/*  */
void SPST_button1_read(void);

void SPST_button1_wakeReset(void);  /* "按键弱唤醒重置" STOP唤醒后复位扫描状态机(实体在key.c), 清除跨STOP的long_sent等残留 */ 

/* 
	单刀单掷按钮按照顺序进入对应设定的功能：
		长按3s状态: 如果当前已经开机，就进行关机处理;
		长按3s状态: 如果当前不是开机，就进行开机处理;
		短按单击状态: 持续20s 气泵机泄漏气口E_airPump_DC1 开始工作,在此期间，电磁阀线圈绕组共计6个统一开始工作,第20s后都停止工作; 
		双连击: 持续20s 先电磁阀线圈绕组共计6个打开后，再打开气泵机进气口E_airPump_DJ1 开始工作,第20s后停止工作，6个绕组和气泵机停止; 
		三连击: 打印;
		四连击: 打印;
*/
void SPST_button1_scan(void);

#endif	   // __KEY_H


