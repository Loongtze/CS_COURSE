#include "can.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "control.h" 
 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//CAN驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/11
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:1~3; CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
//tbs2:时间段2的时间单元.范围:1~8;
//tbs1:时间段1的时间单元.范围:1~16;	  CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024;(实际要加1,也就是1~1024) tq=(brp)*tpclk1
//注意以上参数任何一个都不能设为0,否则会乱.
//波特率=Fpclk1/((tsjw+tbs1+tbs2)*brp);
//mode:0,普通模式;1,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Normal_Init(1,8,7,5,1);
//则波特率为:36M/((1+8+7)*5)=450Kbps
//返回值:0,初始化OK;
//    其他,初始化失败;

u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{

	  GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
 	  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE 
   	NVIC_InitTypeDef  NVIC_InitStructure;
#endif

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PORTA时钟	                   											 

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
    GPIO_Init(GPIOA, &GPIO_InitStructure);		//初始化IO
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化IO
	  
 	//CAN单元设置
 	CAN_InitStructure.CAN_TTCM=DISABLE;						 //非时间触发通信模式  //
 	CAN_InitStructure.CAN_ABOM=DISABLE;						 //软件自动离线管理	 //
  	CAN_InitStructure.CAN_AWUM=DISABLE;						 //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)//
  	CAN_InitStructure.CAN_NART=ENABLE;						 	//禁止报文自动传送 //
  	CAN_InitStructure.CAN_RFLM=DISABLE;						 //报文不锁定,新的覆盖旧的 // 
  	CAN_InitStructure.CAN_TXFP=DISABLE;						 //优先级由报文标识符决定 //
  	CAN_InitStructure.CAN_Mode= mode;	         //模式设置： mode:0,普通模式;1,回环模式; //
  	//设置波特率
  	CAN_InitStructure.CAN_SJW=tsjw;				//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;            //分频系数(Fdiv)为brp+1	//
  	CAN_Init(CAN1, &CAN_InitStructure);            // 初始化CAN1 

 	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
   	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
 	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0

  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
	
#if CAN_RX0_INT_ENABLE
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // 主优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;            // 次优先级为2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               // 开启can中断
  	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   
 
#if CAN_RX0_INT_ENABLE	//使能RX0中断
//中断服务函数			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  	CanRxMsg RxMessage;
	
//	i++;
//	if(i==100)
//	{
//		LED0=!LED0;//提示系统正在运行	
//		i=0;
//	}
	
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	printf("id:%d",RxMessage.StdId);
	
	if(RxMessage.StdId == 0x01)//firelay 发来的消息的 can_stdid  为 0x01 
	{
//		printf("\r\ncan_receive");
//		printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t\r\n",RxMessage.Data[0],RxMessage.Data[1],RxMessage.Data[2],RxMessage.Data[3],RxMessage.Data[4],RxMessage.Data[5],RxMessage.Data[6],RxMessage.Data[7]);

/////////////两轮车
/*		
		if(ptForkLift->u8CarID==0)  //右轮
		{

			ptForkLift->bDrection = (RxMessage.Data[6] & 0x02)>>1;

			if ((RxMessage.Data[6] & 0x02)>>1)
				ptForkLift->s16speedwant = -(float)((RxMessage.Data[3]<<8)| RxMessage.Data[2])*2.5; //25ms内的脉冲数 
			else
				ptForkLift->s16speedwant = (float)((RxMessage.Data[3]<<8) | RxMessage.Data[2])*2.5;//25ms内的脉冲数 
		}
		
		if(ptForkLift->u8CarID==1) //左轮
		{
			ptForkLift->bDrection = (RxMessage.Data[6] & 0x01);

//			printf("bDrection:%d",ptForkLift->bDrection);


			if ((RxMessage.Data[6] & 0x01))
				ptForkLift->s16speedwant = -(float)((RxMessage.Data[1]<<8) | RxMessage.Data[0])*2.5; //25ms内的脉冲数 
			else
				ptForkLift->s16speedwant = (float)((RxMessage.Data[1]<<8) | RxMessage.Data[0])*2.5;//25ms内的脉冲数 
			}
*/
///////////////////////////////叉车//////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
/*
下行部分（叉车）
（1）电机驱动
字节7：
X X X X 0 0 0 0 //message_type = 0x0
字节6：（各个方向控制）
X X X X X - - -  
（bit7  叉车臂前后     1 向前 0 向后）
（bit6  叉车臂上下     1 向上 0 向下）
（bit5  叉车X方向      1 正   0 负）
（bit4  叉车Y方向      1 正   0 负）
（bit3  叉车旋转方向   1 正   0 负）
字节5：保留
字节4：（叉车臂前后）
X X X X X X X X //（0.32m/s 等分256） 
字节3：（叉车上下）
X X X X X X X X //（0.32m/s 等分256） 
字节2：（叉车X速度）
X X X X X X X X //（0.8m/s 等分256）
字节1 ：（叉车Y速度）
X X X X X X X X //（0.8m/s 等分256）
字节0 ：（角速度）
X X X X X X X X // （3.14弧度/s 等分256
底盘四个轮子转一圈 27500个脉冲
轮子直径 0.125m   12.5cm
*/
/*
//角速度
if ((RxMessage.Data[6] & 0x08)) // 字节6 的bit3
  ptForkLift->s16w  =(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
else
  ptForkLift->s16w =-(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
//y轴速度
if ((RxMessage.Data[6] & 0x10)) // 字节6 的bit4
  ptForkLift->s16y  =(float)(RxMessage.Data[1]/255.0*0.8);  //0~255 -> 0~0.8m/s
else
  ptForkLift->s16y =-(float)(RxMessage.Data[1]/255.0*0.8);  //0~255 -> 0~0.8m/s
//x轴速度
if ((RxMessage.Data[6] & 0x20)) // 字节6 的bit5
  ptForkLift->s16x  =(float)(RxMessage.Data[2]/255.0*0.8);  //0~255 -> 0~0.8m/s
else
  ptForkLift->s16x =-(float)(RxMessage.Data[2]/255.0*0.8);  //0~255 -> 0~0.8m/s
//上下叉速度
if ((RxMessage.Data[6] & 0x40)) // 字节6 的bit5
  ptForkLift->s16ud =(float)(RxMessage.Data[3]/255.0*0.32); //0~255 -> 0~0.32m/s
else
  ptForkLift->s16ud=-(float)(RxMessage.Data[3]/255.0*0.32); //0~255 -> 0~0.32m/s�
//前后叉速度
if ((RxMessage.Data[6] & 0x80)) // 字节6 的bit5
  ptForkLift->s16fb =(float)(RxMessage.Data[4]/255.0*0.32); //0~255 -> 0~0.32m/s
else  ptForkLift->s16fb=-(float)(RxMessage.Data[4]/255.0*0.32); //0~255 -> 0~0.32m/s
*/
//printf("w:%f\r\n",ptForkLift->s16w);
/*
//叉车物理参数
ptForkLift->s16fb_d=0.032; //前后叉轮子直径 0.032m    3.2cm
ptForkLift->s16ud_d=0.032; //上下叉轮子直径
ptForkLift->s16dp_d=0.485; //底盘直径      //0.485m   48.5cm
ptForkLift->s16dpl_d=0.125;//底盘轮子直径   //0.125m   12.5cm

ptForkLift->u32dpl_econder=500;//底盘轮线数
ptForkLift->u32dpl_econder_reduc=55;//底盘轮减速比

ptForkLift->u32ud_econder=500;//底盘轮线数
ptForkLift->u32ud_econder_reduc=55;//底盘轮减速比

ptForkLift->u32fb_econder=500;//前后轮线数
ptForkLift->u32fb_econder_reduc=160;//前后轮减速比
*/
///*

if(ptForkLift->u8CarID==1)  //前轮  v1=Vy+w*R
		{
	//		/*
//角速度
if ((RxMessage.Data[6] & 0x08)) // 字节6 的bit3
  ptForkLift->s16w  =(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
else
  ptForkLift->s16w = -(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
//y轴速度
if ((RxMessage.Data[6] & 0x10)) // 字节6 的bit4
  ptForkLift->s16y  =(float)(RxMessage.Data[1]/255.0*0.8);  //0~255 -> 0~0.8m/s
else
  ptForkLift->s16y = -(float)(RxMessage.Data[1]/255.0*0.8);  //0~255 -> 0~0.8m/s
//*/
			
ptForkLift->s16speedwant = (float)((ptForkLift->s16y + ptForkLift->s16w * ptForkLift->s16dp_d*0.5)/(3.14*ptForkLift->s16dpl_d)*ptForkLift->u32dpl_econder*ptForkLift->u32dpl_econder_reduc/1000); //1ms内的脉冲数 

}
		
	
if(ptForkLift->u8CarID==2)  //左轮 v1=-Vx+w*R
		{
//			/*
//角速度
if ((RxMessage.Data[6] & 0x08)) // 字节6 的bit3
  ptForkLift->s16w  =(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
else
  ptForkLift->s16w = -(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
//x轴速度
if ((RxMessage.Data[6] & 0x20)) // 字节6 的bit5
  ptForkLift->s16x  =(float)(RxMessage.Data[2]/255.0*0.8);  //0~255 -> 0~0.8m/s
else
  ptForkLift->s16x = -(float)(RxMessage.Data[2]/255.0*0.8);  //0~255 -> 0~0.8m/s
//*/
ptForkLift->s16speedwant = (float)((-ptForkLift->s16x + ptForkLift->s16w * ptForkLift->s16dp_d*0.5)/(3.14*ptForkLift->s16dpl_d)*ptForkLift->u32dpl_econder*ptForkLift->u32dpl_econder_reduc/1000); //1ms内的脉冲数 
		}
		
		
if(ptForkLift->u8CarID==3)  //后轮 v1=-Vy+w*R
		{
//			/*
//角速度
if ((RxMessage.Data[6] & 0x08)) // 字节6 的bit3
  ptForkLift->s16w  =(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
else
  ptForkLift->s16w = -(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
//y轴速度
if ((RxMessage.Data[6] & 0x10)) // 字节6 的bit4
  ptForkLift->s16y  =(float)(RxMessage.Data[1]/255.0*0.8);  //0~255 -> 0~0.8m/s
else
  ptForkLift->s16y = -(float)(RxMessage.Data[1]/255.0*0.8);  //0~255 -> 0~0.8m/s
//*/
ptForkLift->s16speedwant = (float)((-ptForkLift->s16y + ptForkLift->s16w * ptForkLift->s16dp_d*0.5)/(3.14*ptForkLift->s16dpl_d)*ptForkLift->u32dpl_econder*ptForkLift->u32dpl_econder_reduc/1000); //1ms内的脉冲数 
		}
		
		
if(ptForkLift->u8CarID==4)  //右轮 v1=Vx+w*R
    {
//			/*
//角速度
if ((RxMessage.Data[6] & 0x08)) // 字节6 的bit3
  ptForkLift->s16w  =(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
else
  ptForkLift->s16w = -(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
//x轴速度
if ((RxMessage.Data[6] & 0x20)) // 字节6 的bit5
  ptForkLift->s16x  =(float)(RxMessage.Data[2]/255.0*0.8);  //0~255 -> 0~0.8m/s
else
	ptForkLift->s16x = -(float)(RxMessage.Data[2]/255.0*0.8);  //0~255 -> 0~0.8m/s
//*/
ptForkLift->s16speedwant = (float)((ptForkLift->s16x + ptForkLift->s16w * ptForkLift->s16dp_d*0.5)/(3.14*ptForkLift->s16dpl_d)*ptForkLift->u32dpl_econder*ptForkLift->u32dpl_econder_reduc/1000); //1ms内的脉冲数 
		}
		
		
if(ptForkLift->u8CarID==5)  //上下叉
		{
    	// ptForkLift->s16speedwant = (float)(ptForkLift->s16ud/(3.14*ptForkLift->s16ud_d)*ptForkLift->u32ud_econder*ptForkLift->u32ud_econder_reduc/1000); //1ms内的脉冲数
			ptForkLift->s16speedwant =0;
		}
if(ptForkLift->u8CarID==6)  //前后叉
		{
//			/*
//前后叉速度
if ((RxMessage.Data[6] & 0x80)) // 字节6 的bit5
  ptForkLift->s16fb =(float)(RxMessage.Data[4]/255.0*0.32); //0~255 -> 0~0.32m/s
else
  ptForkLift->s16fb = -(float)(RxMessage.Data[4]/255.0*0.32); //0~255 -> 0~0.32m/s
//*/
ptForkLift->s16speedwant = (float)(ptForkLift->s16fb/(3.14*ptForkLift->s16fb_d)*ptForkLift->u32fb_econder*ptForkLift->u32fb_econder_reduc/1000); //1ms内的脉冲数 
		 //ptForkLift->s16speedwant =0;
		}
//*/

//printf("%f\r\n",ptForkLift->s16speedwant);

			if (ptForkLift->s16speedwant==0)
			{
				ptForkLift->s16error[0] = 0;
				ptForkLift->s16error[1] = 0;
				ptForkLift->s16error[2] = 0;
				ptForkLift->s16ErrorSum = 0;
			}
			

//		printf("direction: %d\t value: %f\t\r\n",ptForkLift->bDrection,ptForkLift->s16speedwant);
		ptForkLift->u16CanComCount[0]++;
		
	}
	   
	
	
	
	//for(i=0;i<8;i++)
	//printf("rxbuf[%d]:%d\r\n",i,RxMessage.Data[i]);
}
#endif

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 Can_Send_Msg(void)
{	
  u8 mbox;
  u16 i=0;

//	printf("can_send_msg:%d\r\t\n",ptForkLift->s32EncoderCounter);
	
  mbox= CAN_Transmit(CAN1, &ptForkLift->Can_Txmessage);   
  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
  if(i>=0XFFF)return 1;
  return 0;		

}
//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
u8 Can_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//没有接收到数据,直接退出 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
    for(i=0;i<8;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}














