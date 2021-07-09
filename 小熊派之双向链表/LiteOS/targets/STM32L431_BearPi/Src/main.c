/*
*2021.7.7 
*2018119145 李嘉雯
*/
//双向链表
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"
/* LiteOS 头文件 */
#include "los_task.ph"
#include "los_sys.h"
#include "los_memory.h"

#define KEY1_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOB

#define KEY2_Pin GPIO_PIN_3
#define KEY2_GPIO_Port GPIOB

#define KEY_ON	1
#define KEY_OFF	0

UINT32 g_TskHandle;

/* 定义任务句柄 */
UINT32 Test_Task_Handle;

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

/*********************************************************************************
  * @ 函数名  ： Clear_Task
  * @ 功能说明： 写入已经初始化成功的内存池地址数据
  * @ 参数    ： void
  * @ 返回值  ： 无
  ********************************************************************************/
static void Test_Task(void)
{
	UINT32 uwRet = LOS_OK;			/* 定义一个初始化的返回类型，初始化为成功的返回值 */
	printf("\n双向链表初始化中......\t\r\n");
	
	LOS_DL_LIST *head;				/* 定义一个双向链表的头节点 */
	head = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));	/* 动态申请头结点的内存 */

	LOS_ListInit(head);				/* 初始化双向链表 */
	if (!LOS_ListEmpty(head))		/* 判断是否初始化成功 */
	{
			printf("双向链表初始化失败!\n\n");
	}
	else
	{
		printf("双向链表初始化成功!\t\r\n\n");
	}
	
	printf("添加节点和尾节点添加......\t\r\n");			/* 插入节点：顺序插入与从末尾插入 */

    LOS_DL_LIST *node1 = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));/* 动态申请第一个结点的内存 */
    LOS_DL_LIST *node2 = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));/* 动态申请第二个结点的内存 */
    LOS_DL_LIST *tail = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));	/* 动态申请尾结点的内存 */

	printf("添加第一个节点与第二个节点.....\t\r\n");
    LOS_ListAdd(head,node1);					/* 添加第一个节点，连接在头结点上 */
    LOS_ListAdd(node1,node2);					/* 添加第二个节点，连接在一个节点上 */
    if ((node1->pstPrev == head) || (node2->pstPrev == node1))	/* 判断是否插入成功 */
    {
        printf("添加节点成功!\t\r\n\n");
    }
	else
	{
		printf("添加节点失败!\n\n");
	}
	printf("将尾节点插入双向链表的末尾.....\t\r\n");
	LOS_ListTailInsert(head, tail);				/* 将尾节点插入双向链表的末尾 */
    if (tail->pstPrev == node2)					/* 判断是否插入成功 */		
    {
        printf("链表尾节点添加成功!\t\r\n\n");
    }
	else
	{
		printf("链表尾节点添加失败!\n\n");
	}
	
    printf("删除节点......\t\r\n");				/* 删除已有节点 */		
    LOS_ListDelete(node1);					/* 删除第一个节点 */	
    LOS_MemFree(m_aucSysMem0, node1);		/* 释放第一个节点的内存，保证后面的程序回收内存空间 */	
    if (head->pstNext == node2)				/* 判断是否删除成功 */
    {
        printf("删除节点成功\t\r\n\n");
    }
    else
    {
        printf("删除节点失败\t\r\n\n");
        
    }
	
	while(1)
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		LCD_ShowString(10, 50, 240, 24, 24, "success");
		printf("任务运行中!\t\r\n");
		LOS_TaskDelay (2000);
	}
}


/* 创建Test_Task任务*/
static UINT32 Creat_Test_Task(void)
{
    UINT32 uwRet = LOS_OK;				/* 定义一个创建任务的返回类型，初始化为创建成功的返回值 */
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 4;				/* 优先级，数值越小，优先级越高 */
    task_init_param.pcName = "Test_Task";						/* 任务名 */
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Test_Task;	/* 任务函数名 */
    task_init_param.uwStackSize = 1024;						/* 堆栈大小 */

    uwRet = LOS_TaskCreate(&Test_Task_Handle, &task_init_param);/* 创建任务 */
    return uwRet;
}

int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();
    uwRet = LOS_KernelInit();
	
    extern UINT32 create_work_tasks(VOID);
    uwRet = create_work_tasks();
		
		/* 创建Test_Task任务 */
	uwRet = Creat_Test_Task();
    if (uwRet != LOS_OK)
    {
		printf("Test_Task任务创建失败！\n");
    }
		(void)LOS_Start();
		return 0;
}
