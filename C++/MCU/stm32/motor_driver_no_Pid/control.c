#include "control.h"
#include "PidContoller.h"
/************************************************
 
 创建者：汪自强
 创建时间：2017/03/20
 文件说明：智乘网络科技公司叉车电机驱动程序
 
 电机上层控制程序都在此文件中
 
************************************************/

//全局变量声明
TCtrl TForkLift;     //定义叉车结构体
ptCtrl ptForkLift;   //定义叉车结构体指针

// 设置 PWM和方向值
void SetPwmDir(ptCtrl ptCtrlCar)
{
	// pwm 限幅 输出
	if(ptCtrlCar->u16PWM>MAX_PWM)
	{
		ptCtrlCar->u16PWM=MAX_PWM;
	}
	else if(ptCtrlCar->u16PWM<=MIN_PWM)
	{
		ptCtrlCar->u16PWM=MIN_PWM;
	}
	
	//TIM_SetCompare3(TIM1,(ptCtrlCar->u16PWM)/2);//AD触发中断
	//TIM_SetCompare3(TIM1,ptCtrlCar->u16PWM);//AD触发中�
	 TIM_SetCompare1(TIM1,ptCtrlCar->u16PWM);
	//TIM1->CCR1 = ptCtrlCar->u16PWM;       // 通道1 pwm
	
	// //PA.9 控制方向 高 前进  低 后退
	if(ptCtrlCar->bDrection==FOWARD)
	{
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET);  //dir	
		//GPIO_SetBits(GPIOA,GPIO_Pin_9);			//PA.9 输出高
		TIM_SetCompare2(TIM1,1799);           // 通道2 pwm 为 100% 脉冲 即为高电平
		//TIM1->CCR2 = ptCtrlCar->1799;       // 通道2 pwm
	}
	
	else if(ptCtrlCar->bDrection==BACK)
	{
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);  //dir	
		//GPIO_ResetBits(GPIOA,GPIO_Pin_9);			//PA.9 输出低
	  TIM_SetCompare2(TIM1,0);           // 通道2 pwm 为 0% 脉冲 即为低电平
		//TIM1->CCR2 = ptCtrlCar->0;       // 通道2 pwm
		
	}	
}		


// 设置 PWM和方向值
void SetPwmDir_new(ptCtrl ptCtrlCar)
{
	// pwm 限幅 输出
	if(ptCtrlCar->u16PWM>MAX_PWM)
	{
		ptCtrlCar->u16PWM=MAX_PWM;
	}
	else if(ptCtrlCar->u16PWM<=MIN_PWM)
	{
		ptCtrlCar->u16PWM=MIN_PWM;
	}
	
	//TIM_SetCompare3(TIM1,(ptCtrlCar->u16PWM)/2);//AD触发中断
	//TIM_SetCompare3(TIM1,ptCtrlCar->u16PWM);
	//TIM_SetCompare1(TIM1,ptCtrlCar->u16PWM);
	//TIM1->CCR1 = ptCtrlCar->u16PWM;       // 通道1 pwm
	
	// //PA.9 控制方向 高 前进  低 后退
	if(ptCtrlCar->bDrection==FOWARD)
	{
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET);  //dir	
		//GPIO_SetBits(GPIOA,GPIO_Pin_9);			//PA.9 输出高
		
	 /*若方向相反， 则 将 if  和 else if 内容互换 */
		TIM_SetCompare1(TIM1,ptCtrlCar->u16PWM);
		TIM_SetCompare2(TIM1,0);           // 通道2 pwm 为  0% 脉冲 即为低电平
	}
	else if(ptCtrlCar->bDrection==BACK)
	{
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);  //dir	
		//GPIO_ResetBits(GPIOA,GPIO_Pin_9);			//PA.9 输出低
		
	 /*若方向相反， 则 将 if  和 else if 内容互换 */
		TIM_SetCompare1(TIM1,0);                // 通道1 pwm 为 0% 脉冲 即为低电平
	  TIM_SetCompare2(TIM1,ptCtrlCar->u16PWM); 
	}	
}	


void PID_Control()
{
	
}





