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
#ifndef __INDICATORLED1_H
#define __INDICATORLED1_H

 			 
#include "./SYSTEM/sys/sys.h"
#include "STC8G.h"
#include "../Config/feature_config.h" /* 相对本文件: ConsumerUser/ -> ../Config/ */

/******************************************************************************************/
/*  引脚定义: LED1 = P3.1, 串电阻接GND, 输出高电平点亮, 低电平熄灭 */  
sbit INDICATOR_LED1_PIN = P3^1;         /* LED1正极侧, 高电平点亮 */

/*  "用户指示灯模式枚举LED1_Mode_t" 提示模式(可扩展: 以后新增告警快闪/呼吸等, 在此枚举追加即可) */
typedef enum
{
	LED1_MODE_OFF = 0,          /* 熄灭 */
	LED1_MODE_ON,               /* 常亮(单击暂停) */
	LED1_MODE_BLINK,            /* 限时对称闪烁, 到点自灭(双击20s/三连击60s) */
	LED1_MODE_BLINK_FOREVER,    /* 永久对称闪烁(自动止鼾运行: 亮2s灭2s) */
	LED1_MODE_BLINK_ASYM        /* 永久非对称闪烁(自检: 亮2s灭1s) */
} LED1_Mode_t;

/*	 
*/ 

/*   端口定义 */ 



 

/*  取反定义 */
 

/*  事件枚举状态机*/ 

/****************************************************************************************************************/
/* xx的寄存器地址 */
/* 详细说明参照xx数据手册 */



/****************************************************************************************************************/
/* 函数声明 */  
#if FEATURE_USER_LED1	 
	void indicatorLED1_init(void);       /* P3.1配推挽输出, 初始熄灭(量产生效; 调试构建P3.1留给TX) */
	void indicatorLED1_on(void);         /* 常亮 */
	void indicatorLED1_off(void);        /* 熄灭 */
	void indicatorLED1_read(void);       /* 调试: 打印引脚电平 */
	void indicatorLED1_tick(void);       /* 闪烁驱动, 必须放进10ms周期任务, 非阻塞 */
	void indicatorLED1_blink(unsigned int duration_ms, unsigned int period_ms);  /* 启动限时闪烁: duration_ms=总时长(到点自灭), period_ms=亮/灭各持续ms(半周期) */
	void indicatorLED1_blink_forever(unsigned int half_ms); /* 永久对称闪烁(运行态指示), 仅由on/off/切模式终止 */
	void indicatorLED1_blink_asym_forever(unsigned int on_ms, unsigned int off_ms); /* 永久非对称闪烁: on_ms亮/off_ms灭(自检提示) */
	unsigned char indicatorLED1_isBusy(void);  /* 1=闪烁窗口进行中(操作忙, 应拒绝新操作) */

#else
	/* 调试构建: LED1整体屏蔽, P3.1(TXD)不被占用, 全部调用零成本空转 */
	#define indicatorLED1_init()          do {} while(0)
	#define indicatorLED1_on()            do {} while(0)
	#define indicatorLED1_off()           do {} while(0)
	#define indicatorLED1_read()          do {} while(0)
	#define indicatorLED1_tick()          do {} while(0)
	#define indicatorLED1_blink(d, p)     do {} while(0)
	#define indicatorLED1_blink_forever(h)         do {} while(0)
	#define indicatorLED1_blink_asym_forever(o, f) do {} while(0)
	#define indicatorLED1_isBusy()        (0)
#endif
			   
  



#endif	  // __INDICATORLED1_H

