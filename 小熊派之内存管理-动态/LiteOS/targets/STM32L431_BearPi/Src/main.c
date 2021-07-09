/*
*2021.7.7 
*2018119145 �����
*/
//�ڴ����-��̬
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"
 /* LiteOS ͷ�ļ� */
#include "los_task.ph"
#include "los_sys.h"
#include "los_memory.h"

#define KEY1_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOB

#define KEY2_Pin GPIO_PIN_3
#define KEY2_GPIO_Port GPIOB

#define KEY_ON	1
#define KEY_OFF	0
#define  	MALLOC_MEM_SIZE   	16	//�����ڴ�Ĵ�С���ֽڣ�

UINT32 g_TskHandle;

/* ���������� */
UINT32 LED_Task_Handle;
UINT32 Key_Task_Handle;

UINT32 *p_Num = NULL;			//ָ���д�ڴ��ַ��ָ��


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
  * @ ������  �� LED_Task
  * @ ����˵���� LED_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void LED_Task(void)
{
	// ����һ���¼����ձ���
	UINT32 uwRet;			
  /* ������һ������ѭ�������ܷ��� */
	while(1)
	{
   HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
   LOS_TaskDelay(1000);/* ��ʱ1000��tick */
	}
}

/******************************************************************
  * @ ������  �� Key_Task
  * @ ����˵���� Key_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Key_Task(void)
{			
	// ����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;			

	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		LCD_ShowString(10, 30, 240, 24, 24, "Open the port");
		LCD_ShowString(10, 60, 240, 16, 16, "Please press the Key");
		/* KEY1 ������ */
    if( Key_Scan(KEY1_GPIO_Port,KEY1_Pin) == KEY_ON )       
		{
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
      if(NULL == p_Num)
      {
				printf("���������ڴ�....................\t\r\n");  
				p_Num = (UINT32*)LOS_MemAlloc(m_aucSysMem0,MALLOC_MEM_SIZE);	
					
				if (NULL == p_Num)
					printf("�����ڴ�ʧ��!\n");
				else
				{
					printf("�����ڴ�ɹ�!��ַΪ0x%X \t\r\n",(uint32_t)p_Num);
					
					//��Test_Ptr��д�뵱����:��ǰϵͳʱ��
					sprintf((char*)p_Num,"��ǰϵͳTickCount = %d",(UINT32)LOS_TickCountGet());
					printf("д��������� %s \t\r\n\n",(char*)p_Num);
				}
			}
			else
				printf("���Ȱ���KEY2�ͷ��ڴ�������\t\r\n");
		}

    /* KEY2 ������ */
		if( Key_Scan(KEY2_GPIO_Port,KEY2_Pin) == KEY_ON )      
		{
      if(NULL != p_Num)
      {
				printf("�����ͷ��ڴ�....................\t\r\n");
				uwRet = LOS_MemFree(m_aucSysMem0,p_Num);
				if (LOS_OK == uwRet)
				{
					printf("�ڴ��ͷųɹ�!\t\r\n\n");//�ڴ��ͷųɹ���
					p_Num = NULL;
				}
				else
				{
					printf("�ڴ��ͷ�ʧ��!\n\n");//�ڴ��ͷ�ʧ�ܣ�
				}
			}
			else
        printf("���Ȱ���KEY1�����ڴ����ͷ\t\r�\n\n");
		}
		
		LOS_TaskDelay(20);     //ÿ20msɨ��һ��		
	}
}

/******************************************************************
  * @ ������  �� Creat_LED_Task
  * @ ����˵���� ����LED_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_LED_Task()
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;			
	
	//����һ�����ڴ�������Ĳ����ṹ��
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 5;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "LED_Task";/* ������ */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)LED_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;		/* ��ջ��С */

	uwRet = LOS_TaskCreate(&LED_Task_Handle, &task_init_param);/* �������� */
	return uwRet;
}
/*******************************************************************
  * @ ������  �� Creat_Key_Task
  * @ ����˵���� ����Key_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Key_Task()
{
	// ����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Key_Task";	/* ������*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Key_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;	/* ��ջ��С */
	
	uwRet = LOS_TaskCreate(&Key_Task_Handle, &task_init_param);/* �������� */

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
			printf("LED_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
			return uwRet;
		}
		
		uwRet = Creat_Key_Task();
		if (uwRet != LOS_OK)
		{
			printf("Key_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
			return uwRet;
		}
		(void)LOS_Start();
		return 0;
}
