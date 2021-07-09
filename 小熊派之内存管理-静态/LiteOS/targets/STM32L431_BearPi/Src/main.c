/*
*2021.7.7 
*2018119145 李嘉雯
*/
//内存管理-静态
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"
/* LiteOS 头文件 */
#include "los_task.ph"
#include "los_sys.h"
#include "los_membox.h"

#define KEY1_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOB

#define KEY2_Pin GPIO_PIN_3
#define KEY2_GPIO_Port GPIOB

#define KEY_ON	1
#define KEY_OFF	0

/* 相关宏定义 */
#define  MEM_BOXSIZE   128		//内存池大小
#define  MEM_BLKSIZE   16			//内存块大小

UINT32 g_TskHandle;

/* 定义任务句柄 */
UINT32 LED_Task_Handle;
UINT32 Key_Task_Handle;

static UINT32 BoxMem[MEM_BOXSIZE*MEM_BLKSIZE];
UINT32 *p_Num = NULL;			//指向读写内存池地址的指针
UINT32 *p_Initial = NULL;		//保存初始指针

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
  * @ 函数名  ： LED_Task
  * @ 功能说明： LED_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void LED_Task(void)
{
	// 定义一个事件接收变量
	UINT32 uwRet;			
  /* 任务都是一个无限循环，不能返回 */
	while(1)
	{
    //HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    LOS_TaskDelay(1000);/* 延时1000个tick */
	}
}
/******************************************************************
  * @ 函数名  ： Key_Task
  * @ 功能说明： Key_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void Key_Task(void)
{			
	// 定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;			
	
	printf("正在初始化静态内存池....................\t\r\n");
	/* 初始化内存池 */
	uwRet = LOS_MemboxInit(	&BoxMem[0],			/* 内存池地址 */
													MEM_BOXSIZE,		/* 内存池大小 */
													MEM_BLKSIZE);		/* 内存块大小 */
	if (uwRet != LOS_OK)
		printf("内存池初始化失败\n\n");
	else
		printf("内存池初始化成功!\t\r\n\n");
	
	/* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		LCD_ShowString(10, 30, 240, 24, 24, "Open the port");
    LCD_ShowString(10, 60, 240, 16, 16, "Please press the Key");
		
		/* KEY1 被按下 */
    if( Key_Scan(KEY1_GPIO_Port,KEY1_Pin) == KEY_ON )       
		{
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
      if(NULL == p_Num)
      {
				printf("正在向内存池申请内存....................\t\r\n");  
					
				/* 向已经初始化的内存池申请内存 */
				p_Num = (UINT32*)LOS_MemboxAlloc(BoxMem);
					
				if (NULL == p_Num)
					printf("申请内存失败!\n");
				else
				{
					printf("申请内存成功!地址为0x%X \t\r\n",(uint32_t)p_Num);
					
					//向Test_Ptr中写入当数据:当前系统时间
					sprintf((char*)p_Num,"当前系统TickCount = %d",(UINT32)LOS_TickCountGet());
					printf("写入的数据是 %s \t\r\n\n",(char*)p_Num);
				}
			}
			else
				printf("请先按下KEY2释放内存再申请\t\r\n");
		}

    /* KEY2 被按下 */
		if( Key_Scan(KEY1_GPIO_Port,KEY2_Pin) == KEY_ON )      
		{
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
      if(NULL != p_Num)
      {
				printf("清除前内存信息是 %s ,地址为0x%X \t\r\n", (char*)p_Num,(uint32_t)p_Num);
				printf("正在清除p_Num的内容......................\t\r\n");
				LOS_MemboxClr(BoxMem, p_Num);		/* 清除在p_Num地址的内容 */
				printf("清除后内存信息是 %s ,地址为0x%X \t\r\n\n", (char*)p_Num,(uint32_t)p_Num);
				
				printf("正在释放内存.............................\t\r\n");
				uwRet = LOS_MemboxFree(BoxMem, p_Num);
				if (LOS_OK == uwRet)
				{
					printf("内存释放成功!\t\r\n");//内存释放成功！
					p_Num = NULL;
				}
				else
				{
					printf("内存释放失败!\n");//内存释放失败！
				}
			}
			else
        printf("请先按下KEY1申请内存再释放\t\r\n");
		}
		
		LOS_TaskDelay(20);     //每20ms扫描一次		
	}
}

/******************************************************************
  * @ 函数名  ： Creat_LED_Task
  * @ 功能说明： 创建LED_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static UINT32 Creat_LED_Task()
{
	//定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;			
	
	//定义一个用于创建任务的参数结构体
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 5;	/* 任务优先级，数值越小，优先级越高 */
	task_init_param.pcName = "LED_Task";/* 任务名 */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)LED_Task;/* 任务函数入口 */
	task_init_param.uwStackSize = 1024;		/* 堆栈大小 */

	uwRet = LOS_TaskCreate(&LED_Task_Handle, &task_init_param);/* 创建任务 */
	return uwRet;
}
/*******************************************************************
  * @ 函数名  ： Creat_Key_Task
  * @ 功能说明： 创建Key_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static UINT32 Creat_Key_Task()
{
	// 定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* 任务优先级，数值越小，优先级越高 */
	task_init_param.pcName = "Key_Task";	/* 任务名*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Key_Task;/* 任务函数入口 */
	task_init_param.uwStackSize = 1024;	/* 堆栈大小 */
	
	uwRet = LOS_TaskCreate(&Key_Task_Handle, &task_init_param);/* 创建任务 */

	return uwRet;
}

int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();
    uwRet = LOS_KernelInit();
	
    extern UINT32 create_work_tasks(VOID);
    uwRet = create_work_tasks();
		
		uwRet = Creat_LED_Task();
		if (uwRet != LOS_OK)
		{
			printf("LED_Task任务创建失败！失败代码0x%X\n",uwRet);
			return uwRet;
		}
		
		uwRet = Creat_Key_Task();
		if (uwRet != LOS_OK)
		{
			printf("Key_Task任务创建失败！失败代码0x%X\n",uwRet);
			return uwRet;
		}
		(void)LOS_Start();
		return 0;
}
