#include "time.h"
#include "traffic_light.h"

/*******************************************************************************
* 函 数 名         : TIM2_Init
* 函数功能		   : TIM2初始化函数，用于10ms中断计时
* 输    入         : per:自动装载值
*                    psc:预分频系数
* 输    出         : 无
*******************************************************************************/
void TIM2_Init(u16 per, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  /* 启用TIM2时钟 */
	
	TIM_TimeBaseInitStructure.TIM_Period = per;           /* 自动装载值 */
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;        /* 预分频系数 */
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  /* 向上计数模式 */
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  /* 启用定时器更新中断 */
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;           /* 定时器中断通道 */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; /* 抢占优先级 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        /* 子优先级 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           /* IRQ通道启用 */
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);  /* 启用定时器 */
}

/*******************************************************************************
* 函 数 名         : TIM2_IRQHandler
* 函数功能		   : TIM2中断处理函数
* 输    入         : 无
* 输    出         : 无
* 说    明		   : 每10ms执行一次，用于交通灯状态更新和时序控制
*******************************************************************************/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update))
	{
		/* 调用交通灯更新函数 */
		TrafficLight_Update();
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

/*******************************************************************************
* 函 数 名         : TIM4_Init
* 函数功能		   : TIM4初始化函数
* 输    入         : per:自动装载值
*                    psc:预分频系数
* 输    出         : 无
*******************************************************************************/
void TIM4_Init(u16 per, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  /* 启用TIM4时钟 */
	
	TIM_TimeBaseInitStructure.TIM_Period = per;           /* 自动装载值 */
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;        /* 预分频系数 */
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  /* 向上计数模式 */
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);  /* 启用定时器更新中断 */
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;           /* 定时器中断通道 */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; /* 抢占优先级 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        /* 子优先级 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           /* IRQ通道启用 */
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM4, ENABLE);  /* 启用定时器 */
}

/*******************************************************************************
* 函 数 名         : TIM4_IRQHandler
* 函数功能		   : TIM4中断处理函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update))
	{
		/* TIM4中断处理代码 */
	}
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
}
