#include "stm32f10x.h"
#include "Key.h"
#include "OLED.h"
#include "menu.h"
#include "Delay.h"

//编码器测速与电机速度环
int menu_Speed(void)
{
	OLED_Clear();
	OLED_ShowString(1,1,"Speed_CL");
	OLED_ShowString(2,1,"Tar:");
	OLED_ShowString(3,1,"Act:");
	OLED_ShowString(4,1,"Out:");
	
	return 0;  
}

//电机传动系统
int menu_Location(void)  
{
	OLED_Clear();
	OLED_ShowString(1,1,"Location_CL");
	OLED_ShowString(2,1,"Motor1");
	OLED_ShowString(3,1,"Motor2");
	
	return 0;  
}
