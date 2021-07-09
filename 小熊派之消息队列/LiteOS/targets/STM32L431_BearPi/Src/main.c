/*
*2021.7.7 
*2018119145 李嘉雯
*/
//消息队列
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"
/* LiteOS 头文件 */
#include "los_sys.h"
#include "los_task.ph"
#include "los_queue.h"


#define KEY1_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOB

#define KEY2_Pin GPIO_PIN_3
#define KEY2_GPIO_Port GPIOB

#define KEY_ON	1
#define KEY_OFF	0

UINT32 g_TskHandle;

/* 定义任务句柄 */
UINT32 Receive_Task_Handle;
UINT32 Send_Task_Handle;

 /* 定义消息队列的句柄 */
UINT32 Test_Queue_Handle;
/* 定义消息队列长度 */
#define  TEST_QUEUE_LEN  	10
#define  TEST_QUEUE_SIZE  10

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
  * @ 函数名  ： Send_Task
  * @ 功能说明： Send_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void Send_Task(void)
{
	/* 定义一个返回类型变量，初始化为LOS_OK */
	UINT32 uwRet = LOS_OK;				
	
	//要发送的消息
	UINT32 send_data1 = 1;
	UINT32 send_data2 = 2;
	
	/* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		LCD_ShowString(10, 30, 240, 24, 24, "Open the port");
    LCD_ShowString(10, 60, 240, 16, 16, "Please press the Key");
		/* K1 被按下 */
		if( Key_Scan(GPIOB,KEY1_Pin) == KEY_ON )      
		{
			/* 将数据写入（发送）到队列中，等待时间为 0  */
			uwRet = LOS_QueueWrite(	Test_Queue_Handle,	/* 写入（发送）队列的ID(句柄) */
															&send_data1,				/* 写入（发送）的数据 */
															sizeof(send_data1),	/* 数据的长度 */
															0);									/* 等待时间为 0  */
			if(LOS_OK != uwRet)
			{
				printf("数据不能发送到消息队列！错误代码0x%X\n",uwRet);
			}
		}
		LOS_TaskDelay(20);  
		
		/* K2 被按下 */
		if( Key_Scan(GPIOB,KEY2_Pin) == KEY_ON ) 
		{
			/* 将数据写入（发送）到队列中，等待时间为 0  */
			uwRet = LOS_QueueWrite(	Test_Queue_Handle,	/* 写入（发送）队列的ID(句柄) */
															&send_data2,				/* 写入（发送）的数据 */
															sizeof(send_data2),	/* 数据的长度 */
															0);									/* 等待时间为 0  */
			if(LOS_OK != uwRet)
			{
				printf("数据不能发送到消息队列！错误代码0x%X\n",uwRet);
			}
		}
		/* 20ms扫描一次 */
		LOS_TaskDelay(20);       
	}
}

/******************************************************************
  * @ 函数名  ： Receive_Task
  * @ 功能说明： Receive_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void Receive_Task(void)
{
	/* 定义一个返回类型变量，初始化为LOS_OK */
	UINT32 uwRet = LOS_OK;
	UINT32 *r_queue;
	
	/* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		/* 队列读取（接收），等待时间为一直等待 */
		uwRet = LOS_QueueRead(Test_Queue_Handle,	/* 读取（接收）队列的ID(句柄) */
													&r_queue,			/* 读取（接收）的数据保存位置 */
													TEST_QUEUE_SIZE,		/* 读取（接收）的数据的长度 */
													LOS_WAIT_FOREVER); 	/* 等待时间：一直等 */
		if(LOS_OK == uwRet)
		{
			printf("本次接收到的数据是%x\t\r\n",*r_queue);
		}
		else
		{
			printf("数据接收出错,错误代码0x%X\n",uwRet);
		}
	}
}

/******************************************************************
  * @ 函数名  ： Creat_Receive_Task
  * @ 功能说明： 创建Receive_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static UINT32 Creat_Receive_Task()
{
	//定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;			
	
	//定义一个用于创建任务的参数结构体
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 5;	/* 任务优先级，数值越小，优先级越高 */
	task_init_param.pcName = "Receive_Task";/* 任务名 */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Receive_Task;/* 任务函数入口 */
	task_init_param.uwStackSize = 1024;		/* 堆栈大小 */

	uwRet = LOS_TaskCreate(&Receive_Task_Handle, &task_init_param);/* 创建任务 */
	return uwRet;
}


/*******************************************************************
  * @ 函数名  ： Creat_Send_Task
  * @ 功能说明： 创建Send_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static UINT32 Creat_Send_Task()
{
	// 定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* 任务优先级，数值越小，优先级越高 */
	task_init_param.pcName = "Send_Task";	/* 任务名*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Send_Task;/* 任务函数入口 */
	task_init_param.uwStackSize = 1024;	/* 堆栈大小 */
	
	uwRet = LOS_TaskCreate(&Send_Task_Handle, &task_init_param);/* 创建任务 */

	return uwRet;
}


int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();
    uwRet = LOS_KernelInit();
	
    extern UINT32 create_work_tasks(VOID);
    uwRet = create_work_tasks();
		/* 创建一个测试队列*/
		uwRet = LOS_QueueCreate("Test_Queue",				/* 队列的名称 */
														TEST_QUEUE_LEN,			/* 队列的长度 */
														&Test_Queue_Handle,	/* 队列的ID(句柄) */
														0,						/* 队列模式，官方暂时未使用 */
														TEST_QUEUE_SIZE);	/* 节点大小，单位为字节 */
		if (uwRet != LOS_OK)
		{
			printf("Test_Queue队列创建失败！失败代码0x%X\n",uwRet);
			return uwRet;
		}
			
		uwRet = Creat_Receive_Task();
		if (uwRet != LOS_OK)
		{
			printf("Receive_Task任务创建失败！失败代码0x%X\n",uwRet);
			return uwRet;
		}

		uwRet = Creat_Send_Task();
		if (uwRet != LOS_OK)
		{
			printf("Send_Task任务创建失败！失败代码0x%X\n",uwRet);
			return uwRet;
		}
		(void)LOS_Start();
		return 0;
}
