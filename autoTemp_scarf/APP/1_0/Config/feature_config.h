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
#ifndef __FEATURE_CONFIG_H
#define __FEATURE_CONFIG_H

 

//模式选择，二选一
//#define DEBUG_BUILD        1      //调试构建版本
#define PRODUCTION_BUILD    1      //量产构建版本，对外发布hex

#ifdef DEBUG_BUILD
    #define FEATURE_USER_LED1       0   //调试：屏蔽用户LED1
    #define FEATURE_UART1_DEBUG     1   //调试：打开串口1
#elif PRODUCTION_BUILD
    #define FEATURE_USER_LED1       1   //量产：开启用户LED1
    #define FEATURE_UART1_DEBUG     0   //量产：关闭串口1
#endif
 
/* APP层调试串口打印总开关: 1=全量英文日志(ACT/POSTURE/init等, 占ROM约1.3KB);
   0=关闭打印省code以便烧录。注意: MainFunction.c里2s一次的FilteredADC压力值打印不受此开关控制 */
#define PRS_LOG_EN   0

/* 获取 数据结构地址 */

/* =====  =====
 
 

/* =====  ===== */
 
		    

/*   数据结构地址 */	    
/**
 * @brief  
 */
 

//定义全局状态变量	    

/* 获取 数据结构地址 */
 



#endif	  // __FEATURE_CONFIG_H

