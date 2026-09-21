/**
 ****************************************************************************************************
 * @file        sys.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-09-06
 * @brief       系统初始化代码(包括时钟配置/中断管理/GPIO设置等)
 * @license     Copyright (c) 2020-2032, xx科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:xx开发板
 * 在线视频:www.xx.com
 * 技术论坛:www.xxx.com
 * 公司网址:www.xx.com
 * 购买地址:xx.taobao.com
 *
 * 修改说明
 * V1.0 20260915
 * 第一次发布
 * V1.0 20260915
 * 1, 将头文件包含路径改成相对路径,避免重复设置包含路径的麻烦
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"

/**
 * @brief       判断I_Cache是否打开
 * @param       无
 * @retval      返回值:0 关闭，1 打开
 */
uint8_t get_icahce_sta(void)
{	
	return 0; 
}

/**
 * @brief       判断D_Cache是否打开
 * @param       无
 * @retval      返回值:0 关闭，1 打开
 */
uint8_t get_dcahce_sta(void)
{	
	return 0; 		 
}

/**
 * @brief       设置中断向量表偏移地址
 * @param       baseaddr: 基址
 * @param       offset: 偏移量
 * @retval      无
 */
//void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset)
//{
//    /* 设置NVIC的向量表偏移寄存器,VTOR低9位保留,即[8:0]保留 */
//	(void)baseaddr;
//	(void)offset; 
//}

/**
 * @brief       执行: WFI指令(执行完该指令进入低功耗状态, 等待中断唤醒)
 * @param       无
 * @retval      无
 */
void sys_wfi_set(void)
{	  
}

/**
 * @brief       关闭所有中断(但是不包括fault和NMI中断)
 * @param       无
 * @retval      无
 */
void sys_intx_disable(void)
{							 
}

/**
 * @brief       开启所有中断
 * @param       无
 * @retval      无
 */
void sys_intx_enable(void)
{						    
}

/**
 * @brief       设置栈顶地址
 * @note        左侧的红X, 属于MDK误报, 实际是没问题的
 * @param       addr: 栈顶地址
 * @retval      无
 */
//void sys_msr_msp(uint32_t addr)
//{								    
//}

/**
 * @brief       使能STM32H7的L1-Cache, 同时开启D cache的强制透写
 * @param       无
 * @retval      无
 */
void sys_cache_enable(void)
{			 
}

/**
 * @brief       时钟设置函数
 * @param       plln: PLL1倍频系数(PLL倍频), 取值范围: 4~512.
 * @param       pllm: PLL1预分频系数(进PLL之前的分频), 取值范围: 2~63.
 * @param       pllp: PLL1的p分频系数(PLL之后的分频), 分频后作为系统时钟, 取值范围: 2~128.(且必须是2的倍数)
 * @param       pllq: PLL1的q分频系数(PLL之后的分频), 取值范围: 1~128.
 * @note
 *
 *              Fvco: VCO频率
 *              Fsys: 系统时钟频率, 也是PLL1的p分频输出时钟频率
 *              Fq:   PLL1的q分频输出时钟频率
 *              Fs:   PLL输入时钟频率, 可以是HSI, CSI, HSE等.
 *              Fvco = Fs * (plln / pllm);
 *              Fsys = Fvco / pllp = Fs * (plln / (pllm * pllp));
 *              Fq   = Fvco / pllq = Fs * (plln / (pllm * pllq));
 *
 *              外部晶振为25M的时候, 推荐值: plln = 160, pllm = 5, pllp = 2, pllq = 4.
 *              得到:Fvco = 25 * (160 / 5) = 800Mhz
 *                   Fsys = pll1_p_ck = 800 / 2 = 400Mhz
 *                   Fq   = pll1_q_ck = 800 / 4 = 200Mhz
 *
 *              H743默认需要配置的频率如下:
 *              CPU频率(rcc_c_ck) = sys_d1cpre_ck = 400Mhz
 *              rcc_aclk = rcc_hclk3 = 200Mhz
 *              AHB1/2/3/4(rcc_hclk1/2/3/4) = 200Mhz
 *              APB1/2/3/4(rcc_pclk1/2/3/4) = 100Mhz
 *              pll2_p_ck = (25 / 25) * 440 / 2) = 220Mhz
 *              pll2_r_ck = FMC时钟频率 = ((25 / 25) * 440 / 2) = 220Mhz
 *
 * @retval      错误代码: 0, 成功; 1, 错误;
 */
//uint8_t sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq)
//{
//    
//    return 0;
//}

#ifdef  USE_FULL_ASSERT

/**
 * @brief       当编译提示出错的时候此函数用来报告错误的文件和所在行
 * @param       file：指向源文件
 * @param       line：指向在文件中的行数
 * @retval      无
 */
void assert_failed(uint8_t *file, uint32_t line)
{ 
    while (1)
    {
    }
}
#endif


/* ------------------------------------------------------------------- */
/* 1ms 基础延时:STC8G 1T架构,11.0592MHz 精确延时 */
void Delay1ms(void)        //@11.0592MHz
{
	unsigned char i, j;
	i = 12;
	j = 135;
	do {
		while (--j);
	} while (--i);
}

/* N ms延时函数:基于Delay1ms,精度更高 */
void Delay_nms(unsigned int n)
{
	while (n--) {
		Delay1ms();
	}
}




