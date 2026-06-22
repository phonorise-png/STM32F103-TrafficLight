#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "time.h"
#include "traffic_light.h"

int main()
{
	/* 系统初始化 */
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  /* 中断优先级分组为2 */
	
	/* 初始化交通灯系统 */
	TrafficLight_Init();
	
	/* 初始化定时器TIM2用于10ms中断 */
	/* 计算方式：
	 * TIM2时钟 = 72MHz（APB1）
	 * 预分频 = 72 - 1 = 71 （分频后1MHz）
	 * 自动装载值 = 10 - 1 = 9  （1MHz计数10次 = 10us？需要改为10000次 = 10ms）
	 * 实际：预分频 = 720 - 1 = 719（分频后100KHz）
	 *      自动装载值 = 1000 - 1 = 999 （100KHz计数1000次 = 10ms）
	 */
	TIM2_Init(1000 - 1, 720 - 1);  /* 配置TIM2为10ms中断 */
	
	while(1)
	{	
		/* 主循环，所有工作在中断中完成 */
		/* 不使用软件延时 */
	}
	
	return 0;
}
