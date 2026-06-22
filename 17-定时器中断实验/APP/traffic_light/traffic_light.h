#ifndef _TRAFFIC_LIGHT_H
#define _TRAFFIC_LIGHT_H

#include "system.h"

/* 交通灯引脚定义 */
#define GREEN_PORT      GPIOB
#define GREEN_PIN       GPIO_Pin_0
#define GREEN_RCC       RCC_APB2Periph_GPIOB

#define YELLOW_PORT     GPIOB
#define YELLOW_PIN      GPIO_Pin_1
#define YELLOW_RCC      RCC_APB2Periph_GPIOB

#define RED_PORT        GPIOB
#define RED_PIN         GPIO_Pin_2
#define RED_RCC         RCC_APB2Periph_GPIOB

/* 交通灯状态枚举 */
typedef enum {
    STATE_GREEN = 0,        /* 绿灯 */
    STATE_YELLOW,           /* 黄灯闪烁 */
    STATE_RED,              /* 红灯 */
    STATE_EMERGENCY         /* 紧急模式 */
} TrafficLightState_t;

/* 交通灯结构体 */
typedef struct {
    TrafficLightState_t current_state;  /* 当前状态 */
    u16 time_counter;                    /* 计时计数器（单位：10ms） */
    u16 time_limit;                      /* 当前状态时间限制 */
    u8 yellow_toggle;                    /* 黄灯闪烁标志 */
    u8 emergency_flag;                   /* 紧急模式标志 */
} TrafficLight_t;

/* 函数声明 */
void TrafficLight_Init(void);
void TrafficLight_Update(void);
void TrafficLight_EnterEmergency(void);
void TrafficLight_ExitEmergency(void);
u8 TrafficLight_IsEmergency(void);
void TrafficLight_AllLedOff(void);
void TrafficLight_SetLed(u8 green, u8 yellow, u8 red);

#endif
