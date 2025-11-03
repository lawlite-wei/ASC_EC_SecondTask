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

// 全局变量定义
uint8_t KeyNum = 1;
int16_t Speed1 = 0;        // 用于显示的转速
int16_t Location1 = 0;
int16_t Location2 = 0;
int16_t Actual_Speed = 0; // 实际用于PID计算的速度
int16_t Actual_Location = 0; // 实际用于PID计算的速度

// 增量式PID变量
float Target1 , Actual1 , Out1 ;
float Target2 , Actual2 , Out2 ;
float Kp1 = 0.5, Ki1 = 0.16, Kd1 = 0.5;
float Kp2 = 0.4, Ki2 = 0.0, Kd2 = 0.0;
float Error0_1 = 0, Error1_1 = 0, Error2_1 = 0;
float Error0_2 = 0, Error1_2 = 0, Error2_2 = 0;


int main(void)
{
	OLED_Init();
	Key_Init();
	Timer_Init();
	Encoder1_Init();
	Encoder2_Init();
	Motor_Init();
	Serial_Init();
	
	// 初始显示菜单
	menu_Speed();
	
	while(1)
	{
		//串口发送数据包改变Target的值
		if(Serial_GetRxFlag() == 1)
		{
			Target1 = Serial_SpeedValue;
			Serial_Printf("New Target: %d\r\n", (int16_t)Target1);
		}
		// 按键检测和菜单切换
		if(Key_Check(KEY_3, KEY_DOWN)) 
		{
			KeyNum++;
			if(KeyNum > 2) KeyNum = 1;
			
			if(KeyNum == 1)
			{
				menu_Speed();
			}
			else if(KeyNum == 2)
			{
				menu_Location();
			}
		}
		
		// 实时更新OLED显示
		if(KeyNum == 1)  // 速度菜单
		{
			OLED_ShowSignedNum(2, 5, (int16_t)Target1, 5);  // 显示目标值
			OLED_ShowSignedNum(3, 5, Speed1, 5);            // 显示实际速度
			OLED_ShowSignedNum(4, 5, (int16_t)Out1, 5);     // 显示输出
			Serial_Printf("Target:%.1f, Speed:%d, Out:%.1f\r\n", Target1, Speed1, Out1);
		}
		else if(KeyNum == 2)  // 位置菜单
		{
			OLED_ShowSignedNum(2, 8, Location1, 5);         // 显示位置
			OLED_ShowSignedNum(3, 8, Location2, 5);
		}
		
		Delay_ms(50);  // 适当延时
	}
}

// 定时器中断
void TIM1_UP_IRQHandler(void)
{
	Key_Tick();  // 按键扫描
	
	static uint16_t Count = 0;
	
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {
		Count++;
		if(Count >= 10)  // 每10次中断执行一次（约10ms）
		{
			Count = 0;
			
			if(KeyNum == 1)
			{
				// 获取编码器值
				Actual_Speed = Encoder1_Get();
				Speed1 = Actual_Speed;  // 更新显示用的速度值
				
				// PID计算
				Error2_1 = Error1_1;
				Error1_1 = Error0_1;
				Error0_1 = Target1 - Actual_Speed;
			
				Out1 += Kp1 * (Error0_1 - Error1_1) + Ki1 * Error0_1 + Kd1 * (Error0_1 - 2 * Error1_1 + Error2_1);
			
				// 输出限幅
				if(Out1 > 100) Out1 = 100;
				if(Out1 < -100) Out1 = -100;
			
				Motor1_SetPWM(Out1);
			}
			
			if(KeyNum == 2)
			{
				//获取电机1的转动位置，并赋值到电机2的Target
				Speed1 = Encoder1_Get();
				Location1 += Speed1;
				Target2 = Location1;
				Location2 += Encoder2_Get();
				Actual_Location = Location2;
				
				//PID计算
				Error2_2 = Error1_2;
				Error1_2 = Error0_2;
				Error0_2 = Target2 - Actual_Location;
				
				Out2 += Kp2 * (Error0_2 - Error1_2) + Ki2 * Error0_2 + Kd2 * (Error0_2 - 2 * Error1_2 + Error2_2);
				
				// 输出限幅
				if(Out2 > 100) Out2 = 100;
				if(Out2 < -100) Out2 = -100;
				
				Motor2_SetPWM(Out2);
			}
		}
		
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}
