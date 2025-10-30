#include "stm32f10x.h"  
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "menu.h"
#include "Timer.h"
#include "PWM.h"
#include "Motor.h"
#include "Serial.h"
#include "Encoder.h"

uint8_t KeyNum = 1;
int16_t Speed;
int16_t Location;

//增量式PID
float Target, Actual, Out;
float Kp, Ki, Kd;
float Error0, Error1, Error2;

int main(void)
{
	OLED_Init();
	Key_Init();
	Timer_Init();
	Encoder1_Init();
	Encoder2_Init();
	
	while(1)
	{
		// 在循环中检查按键状态
		if(Key_Check(KEY_3, KEY_DOWN)) 
		{
			KeyNum++;
			if(KeyNum > 2) KeyNum = 1;  // 在1和2之间切换
			
			// 根据KeyNum显示不同菜单
			if(KeyNum == 1)
			{
				menu_Speed();
				OLED_ShowNum(3,5,Speed,5);
			}
			else if(KeyNum == 2)
			{
				menu_Location();
			}
		}
		
		Delay_ms(10);  // 添加延时防止过于频繁的检测
	}
}

// 定时器中断
void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count;
	
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {
		Count ++;
		if(Count >= 10)
		{
			Speed = Encoder1_Get();
			Speed = Encoder2_Get();
			Location += Speed;
			
		}
		
        Key_Tick();
		
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}
