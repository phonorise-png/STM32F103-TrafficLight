#include "traffic_light.h"
#include "led.h"

/* 全局交通灯结构体 */
static TrafficLight_t traffic_light;

/* 外部中断消抖延迟计数器 */
static u16 debounce_counter = 0;
static u8 debounce_flag = 0;

/*******************************************************************************
* 函 数 名         : TrafficLight_Init
* 函数功能		   : 交通灯初始化函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void TrafficLight_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 初始化GPIO时钟 */
	RCC_APB2PeriphClockCmd(GREEN_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(YELLOW_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(RED_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  /* KEY0引脚 PA15 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   /* 启用AFIO时钟用于EXTI重映射 */
	
	/* 初始化绿灯 (PA0) */
	GPIO_InitStructure.GPIO_Pin = GREEN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GREEN_PORT, &GPIO_InitStructure);
	GPIO_SetBits(GREEN_PORT, GREEN_PIN);  /* 初始化为高电平（熄灭） */
	
	/* 初始化黄灯 (PA1) */
	GPIO_InitStructure.GPIO_Pin = YELLOW_PIN;
	GPIO_Init(YELLOW_PORT, &GPIO_InitStructure);
	GPIO_SetBits(YELLOW_PORT, YELLOW_PIN);
	
	/* 初始化红灯 (PA2) */
	GPIO_InitStructure.GPIO_Pin = RED_PIN;
	GPIO_Init(RED_PORT, &GPIO_InitStructure);
	GPIO_SetBits(RED_PORT, RED_PIN);
	
	/* 初始化KEY0按键 (PA15)，配置为下降沿中断 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  /* 上拉输入 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 配置EXTI中断源 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource15);
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  /* 下降沿 */
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* 配置NVIC中断优先级 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  /* 抢占优先级 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         /* 子优先级 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 初始化交通灯状态 */
	traffic_light.current_state = STATE_GREEN;
	traffic_light.time_counter = 0;
	traffic_light.time_limit = 1500;  /* 绿灯15秒 (15000ms / 10ms = 1500) */
	traffic_light.yellow_toggle = 0;
	traffic_light.emergency_flag = 0;
	
	/* 设置绿灯常亮 */
	TrafficLight_SetLed(1, 0, 0);
}

/*******************************************************************************
* 函 数 名         : TrafficLight_Update
* 函数功能		   : 交通灯状态更新函数（在定时器中断中调用）
* 输    入         : 无
* 输    出         : 无
* 说    明		   : 在10ms定时器中断中调用
*******************************************************************************/
void TrafficLight_Update(void)
{
	/* 处理外部中断消抖 */
	if(debounce_flag)
	{
		debounce_counter++;
		if(debounce_counter > 50)  /* 消抖延迟500ms */
		{
			debounce_counter = 0;
			debounce_flag = 0;
		}
	}
	
	/* 如果在紧急模式，执行紧急模式逻辑 */
	if(traffic_light.emergency_flag)
	{
		/* 红灯常亮，黄灯快速闪烁 */
		traffic_light.time_counter++;
		if(traffic_light.time_counter % 10 == 0)  /* 每100ms切换一次黄灯 */
		{
			traffic_light.yellow_toggle = !traffic_light.yellow_toggle;
			if(traffic_light.yellow_toggle)
				TrafficLight_SetLed(0, 1, 1);  /* 黄灯亮，红灯亮 */
			else
				TrafficLight_SetLed(0, 0, 1);  /* 黄灯灭，红灯亮 */
		}
		return;
	}
	
	/* 正常模式状态机 */
	traffic_light.time_counter++;
	
	if(traffic_light.time_counter >= traffic_light.time_limit)
	{
		traffic_light.time_counter = 0;
		
		/* 状态转移 */
		switch(traffic_light.current_state)
		{
			case STATE_GREEN:
				/* 绿灯 15秒 -> 黄灯闪烁 3秒 */
				traffic_light.current_state = STATE_YELLOW;
				traffic_light.time_limit = 300;  /* 3秒 (3000ms / 10ms = 300) */
				traffic_light.yellow_toggle = 0;
				break;
			
			case STATE_YELLOW:
				/* 黄灯闪烁 3秒 -> 红灯 10秒 */
				traffic_light.current_state = STATE_RED;
				traffic_light.time_limit = 1000;  /* 10秒 (10000ms / 10ms = 1000) */
				TrafficLight_SetLed(0, 0, 1);  /* 红灯常亮 */
				break;
			
			case STATE_RED:
				/* 红灯 10秒 -> 绿灯 15秒 */
				traffic_light.current_state = STATE_GREEN;
				traffic_light.time_limit = 1500;
				TrafficLight_SetLed(1, 0, 0);  /* 绿灯常亮 */
				break;
			
			default:
				break;
		}
	}
	
	/* 黄灯闪烁处理 */
	if(traffic_light.current_state == STATE_YELLOW)
	{
		/* 黄灯500ms亮、500ms灭 */
		if((traffic_light.time_counter % 100) < 50)  /* 500ms周期，前50*10ms亮 */
		{
			TrafficLight_SetLed(0, 1, 0);  /* 黄灯亮 */
		}
		else
		{
			TrafficLight_SetLed(0, 0, 0);  /* 黄灯灭 */
		}
	}
}

/*******************************************************************************
* 函 数 名         : TrafficLight_EnterEmergency
* 函数功能		   : 进入紧急模式
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void TrafficLight_EnterEmergency(void)
{
	if(!traffic_light.emergency_flag)
	{
		traffic_light.emergency_flag = 1;
		traffic_light.time_counter = 0;
		traffic_light.yellow_toggle = 0;
		/* 红灯常亮，黄灯熄灭 */
		TrafficLight_SetLed(0, 0, 1);
	}
}

/*******************************************************************************
* 函 数 名         : TrafficLight_ExitEmergency
* 函数功能		   : 退出紧急模式，恢复正常时序（从绿灯开始）
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void TrafficLight_ExitEmergency(void)
{
	traffic_light.emergency_flag = 0;
	traffic_light.current_state = STATE_GREEN;
	traffic_light.time_counter = 0;
	traffic_light.time_limit = 1500;  /* 绿灯15秒 */
	traffic_light.yellow_toggle = 0;
	TrafficLight_SetLed(1, 0, 0);  /* 绿灯常亮 */
}

/*******************************************************************************
* 函 数 名         : TrafficLight_IsEmergency
* 函数功能		   : 检查是否在紧急模式
* 输    入         : 无
* 输    出         : 1 = 紧急模式，0 = 正常模式
*******************************************************************************/
u8 TrafficLight_IsEmergency(void)
{
	return traffic_light.emergency_flag;
}

/*******************************************************************************
* 函 数 名         : TrafficLight_AllLedOff
* 函数功能		   : 熄灭所有交通灯
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void TrafficLight_AllLedOff(void)
{
	GPIO_SetBits(GREEN_PORT, GREEN_PIN);
	GPIO_SetBits(YELLOW_PORT, YELLOW_PIN);
	GPIO_SetBits(RED_PORT, RED_PIN);
}

/*******************************************************************************
* 函 数 名         : TrafficLight_SetLed
* 函数功能		   : 设置交通灯状态
* 输    入         : green - 绿灯状态（1=亮，0=灭）
*                    yellow - 黄灯状态（1=亮，0=灭）
*                    red - 红灯状态（1=亮，0=灭）
* 输    出         : 无
*******************************************************************************/
void TrafficLight_SetLed(u8 green, u8 yellow, u8 red)
{
	if(green)
		GPIO_ResetBits(GREEN_PORT, GREEN_PIN);
	else
		GPIO_SetBits(GREEN_PORT, GREEN_PIN);
	
	if(yellow)
		GPIO_ResetBits(YELLOW_PORT, YELLOW_PIN);
	else
		GPIO_SetBits(YELLOW_PORT, YELLOW_PIN);
	
	if(red)
		GPIO_ResetBits(RED_PORT, RED_PIN);
	else
		GPIO_SetBits(RED_PORT, RED_PIN);
}

/*******************************************************************************
* 函 数 名         : EXTI15_10_IRQHandler
* 函数功能		   : 外部中断15~10处理函数
* 输    入         : 无
* 输    出         : 无
* 说    明		   : 处理KEY0按键中断（PA15）
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
	/* 检查EXTI15中断标志 */
	if(EXTI_GetITStatus(EXTI_Line15) != RESET)
	{
		/* 消抖处理 */
		if(!debounce_flag)
		{
			debounce_flag = 1;
			debounce_counter = 0;
			
			/* 切换紧急模式 */
			if(traffic_light.emergency_flag)
				TrafficLight_ExitEmergency();
			else
				TrafficLight_EnterEmergency();
		}
		
		/* 清除中断标志 */
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}
