/*
*2021.7.7 
*2018119145 �����
*/
//�����ź���
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"
 /* LiteOS ͷ�ļ� */
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

/* ���������� */
UINT32 Pend_Task_Handle;
UINT32 Post_Task_Handle;

/* ��������ź����ľ�� */
UINT32 CountSem_Handle;

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
  * @ ������  �� Pend_Task
  * @ ����˵���� Pend_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Pend_Task(void)
{
	UINT32 uwRet = LOS_OK;				
	
  /* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		LCD_ShowString(10, 30, 240, 24, 24, "Open the port");
		LCD_ShowString(10, 60, 240, 16, 16, "Please press the Key");
		//���KEY1������
		if( Key_Scan(GPIOB,KEY1_Pin) == KEY_ON )      
		{
			/* ��ȡһ�������ź������ȴ�ʱ��0 */
			uwRet = LOS_SemPend ( CountSem_Handle,0); 

			if (LOS_OK ==  uwRet)
			{				
				printf ( "\r\nKEY1�����£��ɹ����뵽ͣ��λ��\r\n" );
			  LCD_ShowString(10, 80, 240, 24, 24, "Key1 is pressed");
			}
			else
				printf ( "\r\nKEY1�����£�������˼������ͣ����������\r\n" );			
				
		}
		LOS_TaskDelay(20);     //ÿ20msɨ��һ��		
	}
}
/******************************************************************
  * @ ������  �� Post_Task
  * @ ����˵���� Post_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Post_Task(void)
{
	UINT32 uwRet = LOS_OK;				
   
	while(1)
	{
		//���KEY2������
		if( Key_Scan(GPIOB,KEY2_Pin) == KEY_ON )       
		{
			/* �ͷ�һ�������ź�����LiteOS�ļ����ź�������һֱ�ͷţ��ڱ����ע��һ�¼���*/
			uwRet = LOS_SemPost(CountSem_Handle);    
				
			if ( LOS_OK == uwRet )
			{				
				printf ( "\r\nKEY2�����£��ͷ�1��ͣ��λ��\r\n" );	
			  LCD_ShowString(10, 80, 240, 24, 24, "Key2 is pressed");
			}
			else
				printf ( "\r\nKEY2�����£������޳�λ�����ͷţ�\r\n" );					
	
		}
		LOS_TaskDelay(20);     //ÿ20msɨ��һ��		
	}		
}


/******************************************************************
  * @ ������  �� Creat_Pend_Task
  * @ ����˵���� ����Pend_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Pend_Task()
{
	//����һ����������ķ������ͣ���ʼ��Ϊ�����ɹ��ķ���ֵ
	UINT32 uwRet = LOS_OK;			
	
	//����һ�����ڴ�������Ĳ����ṹ��
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 3;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Pend_Task";/* ������ */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Pend_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;		/* ��ջ��С */

	uwRet = LOS_TaskCreate(&Pend_Task_Handle, &task_init_param);/* �������� */
	return uwRet;
}
/*******************************************************************
  * @ ������  �� Creat_Post_Task
  * @ ����˵���� ����Post_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Post_Task()
{
	// ����һ����������ķ������ͣ���ʼ��Ϊ�����ɹ��ķ���ֵ
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 3;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Post_Task";	/* ������*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Post_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;	/* ��ջ��С */
	
	uwRet = LOS_TaskCreate(&Post_Task_Handle, &task_init_param);/* �������� */

	return uwRet;
}


int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();
    uwRet = LOS_KernelInit();
	
    extern UINT32 create_work_tasks(VOID);
    uwRet = create_work_tasks();
		/* ����һ�������ź�������ʼ������ֵΪ5*/
		uwRet = LOS_SemCreate (5,&CountSem_Handle);
		if (uwRet != LOS_OK)
		{
			printf("CountSem����ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		}
			
		uwRet = Creat_Pend_Task();
		if (uwRet != LOS_OK)
		{
			printf("Pend_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
			return uwRet;
		}
		
		uwRet = Creat_Post_Task();
		if (uwRet != LOS_OK)
		{
			printf("Post_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
			return uwRet;
		}
		(void)LOS_Start();
		return 0;
}
