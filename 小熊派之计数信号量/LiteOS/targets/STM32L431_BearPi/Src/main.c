/*
*2021.7.7 
*2018119145 李嘉雯
*/
//计数信号量
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"
 /* LiteOS 头文件 */
#include "los_task.ph"
#include "los_sys.h"
#include "los_sem.h"

#define KEY1_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOB

#define KEY2_Pin GPIO_PIN_3
#define KEY2_GPIO_Port GPIOB

#define KEY_ON	1
#define KEY_OFF	0

UINT32 g_TskHandle;

/* 定义任务句柄 */
UINT32 Pend_Task_Handle;
UINT32 Post_Task_Handle;

/* 定义计数信号量的句柄 */
UINT32 CountSem_Handle;

VOID HardWare_Init(VOID)
{
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();
	MX_GPIO_Init();//GPIO初始化
	MX_USART1_UART_Init();
	MX_SPI2_Init();
	dwt_delay_init(SystemCoreClock);
	LCD_Init();					
	LCD_Clear(WHITE);		   	
	POINT_COLOR = RED;			
//	LCD_ShowString(10, 50, 240, 24, 24, "Welcome to IoTCluB!");
//	LCD_ShowString(20, 90, 240, 16, 16, "BearPi IoT Develop Board");
//	LCD_ShowString(20, 130, 240, 16, 16, "Powerd by Huawei LiteOS!");
//	LCD_ShowString(10, 170, 240, 16, 16, "Please wait for system init");
}

/******************************************************************
  * @ 函数名  ： Pend_Task
  * @ 功能说明： Pend_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void Pend_Task(void)
{
	UINT32 uwRet = LOS_OK;				
	
  /* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		LCD_ShowString(10, 30, 240, 24, 24, "Open the port");
		LCD_ShowString(10, 60, 240, 16, 16, "Please press the Key");
		//如果KEY1被按下
		if( Key_Scan(GPIOB,KEY1_Pin) == KEY_ON )      
		{
			/* 获取一个计数信号量，等待时间0 */
			uwRet = LOS_SemPend ( CountSem_Handle,0); 

			if (LOS_OK ==  uwRet)
			{				
				printf ( "\r\nKEY1被按下，成功申请到停车位。\r\n" );
			  LCD_ShowString(10, 80, 240, 24, 24, "Key1 is pressed");
			}
			else
				printf ( "\r\nKEY1被按下，不好意思，现在停车场已满！\r\n" );			
				
		}
		LOS_TaskDelay(20);     //每20ms扫描一次		
	}
}
/******************************************************************
  * @ 函数名  ： Post_Task
  * @ 功能说明： Post_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void Post_Task(void)
{
	UINT32 uwRet = LOS_OK;				
   
	while(1)
	{
		//如果KEY2被按下
		if( Key_Scan(GPIOB,KEY2_Pin) == KEY_ON )       
		{
			/* 释放一个计数信号量，LiteOS的计数信号量允许一直释放，在编程中注意一下即可*/
			uwRet = LOS_SemPost(CountSem_Handle);    
				
			if ( LOS_OK == uwRet )
			{				
				printf ( "\r\nKEY2被按下，释放1个停车位。\r\n" );	
			  LCD_ShowString(10, 80, 240, 24, 24, "Key2 is pressed");
			}
			else
				printf ( "\r\nKEY2被按下，但已无车位可以释放！\r\n" );					
	
		}
		LOS_TaskDelay(20);     //每20ms扫描一次		
	}		
}


/******************************************************************
  * @ 函数名  ： Creat_Pend_Task
  * @ 功能说明： 创建Pend_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static UINT32 Creat_Pend_Task()
{
	//定义一个创建任务的返回类型，初始化为创建成功的返回值
	UINT32 uwRet = LOS_OK;			
	
	//定义一个用于创建任务的参数结构体
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 3;	/* 任务优先级，数值越小，优先级越高 */
	task_init_param.pcName = "Pend_Task";/* 任务名 */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Pend_Task;/* 任务函数入口 */
	task_init_param.uwStackSize = 1024;		/* 堆栈大小 */

	uwRet = LOS_TaskCreate(&Pend_Task_Handle, &task_init_param);/* 创建任务 */
	return uwRet;
}
/*******************************************************************
  * @ 函数名  ： Creat_Post_Task
  * @ 功能说明： 创建Post_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static UINT32 Creat_Post_Task()
{
	// 定义一个创建任务的返回类型，初始化为创建成功的返回值
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 3;	/* 任务优先级，数值越小，优先级越高 */
	task_init_param.pcName = "Post_Task";	/* 任务名*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Post_Task;/* 任务函数入口 */
	task_init_param.uwStackSize = 1024;	/* 堆栈大小 */
	
	uwRet = LOS_TaskCreate(&Post_Task_Handle, &task_init_param);/* 创建任务 */

	return uwRet;
}


int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();
    uwRet = LOS_KernelInit();
	
    extern UINT32 create_work_tasks(VOID);
    uwRet = create_work_tasks();
		/* 创建一个计数信号量，初始化计数值为5*/
		uwRet = LOS_SemCreate (5,&CountSem_Handle);
		if (uwRet != LOS_OK)
		{
			printf("CountSem创建失败！失败代码0x%X\n",uwRet);
		}
			
		uwRet = Creat_Pend_Task();
		if (uwRet != LOS_OK)
		{
			printf("Pend_Task任务创建失败！失败代码0x%X\n",uwRet);
			return uwRet;
		}
		
		uwRet = Creat_Post_Task();
		if (uwRet != LOS_OK)
		{
			printf("Post_Task任务创建失败！失败代码0x%X\n",uwRet);
			return uwRet;
		}
		(void)LOS_Start();
		return 0;
}
