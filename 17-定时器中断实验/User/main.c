#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "time.h"

int main()
{
	u8 i=0;
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组分2组
	LED_Init();
	TIM4_Init(1000,36000-1);  //定时500ms
	
	while(1)
	{	
		i++;
		if(i%10==0)
			LED0=!LED0;

		delay_ms(10);
	}
}
