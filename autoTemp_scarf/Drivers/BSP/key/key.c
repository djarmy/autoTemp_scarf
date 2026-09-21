#include "./BSP/key/key.h"
#include "./SYSTEM/usart/usart.h"
extern void TaskInfoMark(void);     /* APP层协作式调度器1ms节拍标记 */

/* ===== 全局变量实体定义(与key.h的extern对应,只能在此定义一次) ===== */
KeyEvent_t xdata g_key_event = KEY_EVENT_NONE;

/* ===== 模块内部变量 ===== */
static unsigned char xdata key_enabled = 0;          /* on()置1后扫描才生效 */
volatile unsigned int xdata g_key_tick = 0;          /* Timer0 1ms节拍 非阻塞"按键定时器0计数节拍" */  
volatile bit g_task_tick_flag = 0;                   /* Timer0 1ms节拍标志(实体定义,与key.h的extern对应) */

/* 按键扫描状态机状态(原scan()内static上移为文件域: STOP唤醒后需由wakeReset显式复位,
   否则long_sent=1/key_state=1跨STOP残留, 唤醒后第一次长按被吞, 表现为第二次长按无效) */
static unsigned char xdata key_raw = 1;       /* 上次原始电平(1=松开) */
static unsigned char xdata key_stable = 1;    /* 消抖后稳定电平 */
static unsigned char xdata key_state = 0;     /* 0空闲 1按住中 2释放连击窗口 */
static unsigned char xdata click_cnt = 0;     /* 连击计数 */
static unsigned char xdata long_sent = 0;     /* 长按已发送上报标志 */
static unsigned int  xdata edge_tick = 0;
static unsigned int  xdata press_tick = 0;
static unsigned int  xdata release_tick = 0;
 
/* Timer0 1ms中断服务函数(1T @11.0592MHz) */
void Timer0_ISR(void) interrupt 1
{
	TH0 = 0xD4;        /* 手动重装初值！16位模式没有硬件自动重装 */
	TL0 = 0xCD;
	g_key_tick++;
	g_task_tick_flag = 1 ; /* 只置标志,不直接调TaskInfoMark();  最小化ISR栈帧 1ms节拍: 递减各任务RunTime,到点置RunFlag  安全性：TaskInfoMark仅6个uint16递减比较，耗时<10μs，适合ISR。*/
 

	/* ===== 执行器反电动势毛刺自愈(必须放ISR: 主循环即使卡死在UART查询,本中断仍可执行) ===== */
	IE       = 0x82;    /* EA=1,ET0=1; 同时强制ES/ET1/EX1/ET2/EX0=0(本工程正常运行只用T0中断) */
	TR0      = 1;       /* 节拍定时器运行 */
	TR1      = 1;       /* UART1波特率定时器运行 */
	TMOD    &= 0x7E;    /* 【根因】清GATE1(bit7)和GATE0(bit0),保留T1=模式2定时器(0x20)、T0=模式0 */
	AUXR    &= 0xFD;    /* 清S1ST2(bit1),强制UART1波特率源=Timer1,防止被切到未运行的Timer2 */
	INTCLKO &= 0xB3;    /* 清EX4(bit6)/EX3(bit3)/EX2(bit2),正常运行禁止外部中断(睡眠唤醒流程除外) */
	PCON    &= 0xFC;    /* 清IDL/PD,防误进待机/掉电 */
}

/* 按键初始化:P3.0准双向输入(内部上拉) + Timer0 1ms节拍 */
void SPST_button1_init(void)
{
    P3M0 &= ~0x01;                 /* P3.0 准双向口模式(00),内部上拉 */
    P3M1 &= ~0x01;
    SPST_button1_PIN = 1;          /* 确保释放电平 */

    AUXR |= 0x80;                  /* Timer0 设为1T模式 */
    TMOD &= 0xF0;                  /* T0 模式0:16位定时器，无硬件自动重装，中断内软件重装初值 */
    TH0 = 0xD4;                    /* 65536 - 11059 = 0xD4CD,1ms */
    TL0 = 0xCD;
    ET0 = 1;                       /* 使能T0中断 */
    TR0 = 1;                       /* 启动T0 */
    UART_SendString("T0 cfg done, before EA\r\n");
    g_task_tick_flag = 0;          /* 开中断前清零节拍标志,避免首拍误触发 */
    EA  = 1;                       /* 开总中断 */ 
    UART_SendString("after EA\r\n");

    g_key_event = KEY_EVENT_NONE;
    key_enabled = 0;               /* 默认关闭,由on()开启 */

    /* 以当前引脚电平为基准复位扫描状态机:
       SYS_powerOn_work在"长按开机事件处理中"被调用, 此刻按键通常仍按住,
       若简单清成"已松开", 松手沿会被误判成一次单击 -> 开机后立刻进入PAUSED。
       故按住时按"长按已上报"建模(long_sent=1), 松手时被静默吞掉 */
    edge_tick = g_key_tick;
    if(SPST_button1_PIN == 0)
    {
        key_raw = 0;  key_stable = 0;
        key_state = 1; click_cnt = 0; long_sent = 1;
        press_tick = g_key_tick;
    }
    else
    {
        key_raw = 1;  key_stable = 1;
        key_state = 0; click_cnt = 0; long_sent = 0;
    }
}

/* STOP唤醒后专用复位(由SYS_powerOff_sleep唤醒段调用):
   唤醒源就是本次按键的下降沿, 唤醒时按键正按住 -> 按"一次全新按下"建模,
   press_tick从唤醒时刻起算, 继续按住满3s即上报长按开机;
   短按松手会进连击窗口, 但sys_power_en=0时单击/连击事件在分发层全部被忽略, 无副作用     */
void SPST_button1_wakeReset(void)
{
    g_key_event = KEY_EVENT_NONE;
    edge_tick = g_key_tick;
    if(SPST_button1_PIN == 0)
    {
        key_raw = 0;  key_stable = 0;      /* 硬件唤醒沿已等效消抖, 直接认作稳定按下 */
        key_state = 1; click_cnt = 0; long_sent = 0;
        press_tick = g_key_tick;           /* 关键: 长按3s从唤醒时刻重新计, 不用STOP前冻结值 */
    }
    else
    {
        key_raw = 1;  key_stable = 1;
        key_state = 0; click_cnt = 0; long_sent = 0;
    }
}

/* 开启按键扫描 */
void SPST_button1_on(void)
{
    key_enabled = 1;
    UART_SendString("KEY1 scan ENABLE\r\n");
}

/* 关闭按键扫描 */
void SPST_button1_off(void)
{
    key_enabled = 0;
    UART_SendString("KEY1 scan DISABLE\r\n");
}

/* 查询并打印按键原始电平 */
void SPST_button1_read(void)
{
    if (SPST_button1_PIN == 0)
        UART_SendString("KEY1: PRESSED  (P3.0=LOW)\r\n");
    else
        UART_SendString("KEY1: RELEASED (P3.0=HIGH)\r\n");
}

/* 非阻塞按键扫描状态机,主循环反复调用
   检测: 单击/双击/三击/四连击/长按3s, 结果写入 g_key_event */
void SPST_button1_scan(void)
{ 
    unsigned char level;
    unsigned int now;

    if (!key_enabled) return;

    now = g_key_tick;
    level = SPST_button1_PIN;               /* 0=按下 1=松开 */

    /* 消抖:电平变化后稳定KEY_DEBOUNCE_MS(40ms)才确认边沿 */
    if (level != key_raw) {
        key_raw = level;
        edge_tick = now;
    }
    if ((unsigned int)(now - edge_tick) >= KEY_DEBOUNCE_MS && level != key_stable) {
        key_stable = level;
        if (key_stable == 0) {              /* 确认按下沿 */
            press_tick = now;
            long_sent = 0;
            key_state = 1;
        } else {                            /* 确认松开沿 */
            if (long_sent) {                /* 长按事件已发,这次释放不计点击 */
                long_sent = 0;
                click_cnt = 0;
                key_state = 0;
            } else {
                click_cnt++;
                release_tick = now;
                key_state = 2;
            }
        }
    }

    /* 按住达到KEY_LONG_PRESS_MS(3s): 长按 */
    if (key_state == 1 && !long_sent &&
        (unsigned int)(now - press_tick) >= KEY_LONG_PRESS_MS) {
        long_sent = 1;
        g_key_event = KEY_EVENT_LONG_PRESS;
    }

    /* 释放后等待COMBO_WAIT_MS(500ms)连击窗口,超时则按次数定事件 */
    if (key_state == 2 &&
        (unsigned int)(now - release_tick) >= COMBO_WAIT_MS) {
        if (click_cnt == 1)
            g_key_event = KEY_EVENT_SHORT_CLICK;
        else if (click_cnt == 2)
            g_key_event = KEY_EVENT_DOUBLE_CLICK;
        else if (click_cnt == 3)
            g_key_event = KEY_EVENT_TRIPLE_CLICK;
        else if (click_cnt >= 4)
            g_key_event = KEY_EVENT_Quad_CLICK;
        click_cnt = 0;
        key_state = 0;
    }
    /* KEY_PRESS_MAX_MS(5000)为预留上限,本版未使用 */
}
