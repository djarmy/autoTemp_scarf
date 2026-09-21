/**
 ****************************************************************************************************
 * @file        sys.h
 * @author      dxl
 * @version     V1.1
 * @date        2026-09-15
 * @brief       系统初始化代码(包括时钟配置/中断管理/GPIO设置等)
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
 * V1.0 20260915
 * 第一次发布
 * V1.0 20260915
 * 1, 将头文件包含路径改成相对路径,避免重复设置包含路径的麻烦
 ****************************************************************************************************
 */

#ifndef _SYS_H
#define _SYS_H
 

/**
 * SYS_SUPPORT_OS用于定义系统文件夹是否支持OS
 * 0,不支持OS
 * 1,支持OS
 */
#define SYS_SUPPORT_OS         0

#define      ON      1
#define      OFF     0
 #define      Write_Through()    do{ *(__IO uint32_t*)0XE000EF9C = 1UL << 2; }while(0);     /* Cache透写模式 */

/******************************************************************************************/

#define uint8_t  unsigned char
#define uint16_t unsigned int
#define uint32_t unsigned long



/* 1ms 基础延时:STC8G 1T架构,11.0592MHz 精确延时 */
void Delay1ms(void);

/* N ms延时函数:基于Delay1ms,精度更高 */
void Delay_nms(unsigned int n); 

#endif

