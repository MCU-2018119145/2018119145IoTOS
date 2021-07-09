/*
*2021.7.7 
*2018119145 �����
*/
//���ȼ���ת
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"
/* LiteOS ͷ�ļ� */
#include "los_sys.h"
#include "los_task.ph"
#include "los_sem.h"

UINT32 g_TskHandle;

/* ���������� */
UINT32 HighPriority_Task_Handle;
UINT32 MidPriority_Task_Handle;
UINT32 LowPriority_Task_Handle;

/* �����ֵ�ź����ľ�� */
UINT32 BinarySem_Handle;

VOID HardWare_Init(VOID)
{
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();
	MX_GPIO_Init();//GPIO��ʼ��
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
  * @ ������  �� HighPriority_Task
  * @ ����˵���� HighPriority_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void HighPriority_Task(void)
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;	
	
  /* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		//��ȡ��ֵ�ź��� xSemaphore,û��ȡ����һֱ�ȴ�
		uwRet = LOS_SemPend( BinarySem_Handle , LOS_WAIT_FOREVER ); 
		if (uwRet == LOS_OK)
			printf("HighPriority_Task Runing\t\r\n");
		
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		LCD_ShowString(10, 50, 240, 24, 24, "HighPriority ");
		
		LOS_SemPost( BinarySem_Handle );   //������ֵ�ź��� xSemaphore
		LOS_TaskDelay ( 1000 );        			/* ��ʱ100ms */	
  }
}
/******************************************************************
  * @ ������  �� MidPriority_Task
  * @ ����˵���� MidPriority_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void MidPriority_Task(void)
{	
	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		printf("MidPriority_Task Runing\t\r\n");
		LOS_TaskDelay ( 1000 );        			/* ��ʱ100ms */		
	}
}

/******************************************************************
  * @ ������  �� LowPriority_Task
  * @ ����˵���� LowPriority_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void LowPriority_Task(void)
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;		
	
	static uint32_t i;
	
	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		//��ȡ��ֵ�ź��� BinarySem_Handle��û��ȡ����һֱ�ȴ�		
		uwRet = LOS_SemPend( BinarySem_Handle , LOS_WAIT_FOREVER );  
		if (uwRet == LOS_OK)
			printf("LowPriority_Task Runing\t\r\n");
		
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		LCD_ShowString(10, 50, 240, 24, 24, "LowPriority ");
		
    for(i=0;i<3000000;i++)//ģ������ȼ�����ռ���ź���
		{
			//����ʣ��ʱ��Ƭ������һ�������л�
			LOS_TaskYield();
		}
    printf("LowPriority_Task �ͷ��ź���!\t\r\n");
		LOS_SemPost( BinarySem_Handle );    //������ֵ�ź��� xSemaphore		
		
		LOS_TaskDelay ( 1000 );        			/* ��ʱ100ms */		
	}
}

/******************************************************************
  * @ ������  �� Creat_HighPriority_Task
  * @ ����˵���� ����HighPriority_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_HighPriority_Task()
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;			
	
	//����һ�����ڴ�������Ĳ����ṹ��
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 3;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "HighPriority_Task";/* ������ */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)HighPriority_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;		/* ��ջ��С */

	uwRet = LOS_TaskCreate(&HighPriority_Task_Handle, &task_init_param);/* �������� */
	return uwRet;
}
/*******************************************************************
  * @ ������  �� Creat_MidPriority_Task
  * @ ����˵���� ����MidPriority_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_MidPriority_Task()
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "MidPriority_Task";	/* ������*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)MidPriority_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;	/* ��ջ��С */
	
	uwRet = LOS_TaskCreate(&MidPriority_Task_Handle, &task_init_param);/* �������� */

	return uwRet;
}

/*******************************************************************
  * @ ������  �� Creat_MidPriority_Task
  * @ ����˵���� ����MidPriority_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_LowPriority_Task()
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 5;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "LowPriority_Task";	/* ������*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)LowPriority_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;	/* ��ջ��С */
	
	uwRet = LOS_TaskCreate(&LowPriority_Task_Handle, &task_init_param);/* �������� */

	return uwRet;
}

int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();
    uwRet = LOS_KernelInit();
	
    extern UINT32 create_work_tasks(VOID);
    uwRet = create_work_tasks();
    /* ����һ����ֵ�ź���*/
		uwRet = LOS_BinarySemCreate(1,&BinarySem_Handle);
		if (uwRet != LOS_OK)
		{
			printf("BinarySem����ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		}
			
		uwRet = Creat_HighPriority_Task();
		if (uwRet != LOS_OK)
		{
			printf("HighPriority_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
			return uwRet;
		}
		
		uwRet = Creat_MidPriority_Task();
		if (uwRet != LOS_OK)
		{
			printf("MidPriority_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
			return uwRet;
		}
		
		uwRet = Creat_LowPriority_Task();
		if (uwRet != LOS_OK)
		{
			printf("LowPriority_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
			return uwRet;
		}
		(void)LOS_Start();
		return 0;
}
