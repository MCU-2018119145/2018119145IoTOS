/*
*2021.7.7 
*2018119145 �����
*/
//˫������
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

UINT32 g_TskHandle;

/* ���������� */
UINT32 Test_Task_Handle;

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

/*********************************************************************************
  * @ ������  �� Clear_Task
  * @ ����˵���� д���Ѿ���ʼ���ɹ����ڴ�ص�ַ����
  * @ ����    �� void
  * @ ����ֵ  �� ��
  ********************************************************************************/
static void Test_Task(void)
{
	UINT32 uwRet = LOS_OK;			/* ����һ����ʼ���ķ������ͣ���ʼ��Ϊ�ɹ��ķ���ֵ */
	printf("\n˫�������ʼ����......\t\r\n");
	
	LOS_DL_LIST *head;				/* ����һ��˫�������ͷ�ڵ� */
	head = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));	/* ��̬����ͷ�����ڴ� */

	LOS_ListInit(head);				/* ��ʼ��˫������ */
	if (!LOS_ListEmpty(head))		/* �ж��Ƿ��ʼ���ɹ� */
	{
			printf("˫�������ʼ��ʧ��!\n\n");
	}
	else
	{
		printf("˫�������ʼ���ɹ�!\t\r\n\n");
	}
	
	printf("��ӽڵ��β�ڵ����......\t\r\n");			/* ����ڵ㣺˳��������ĩβ���� */

    LOS_DL_LIST *node1 = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));/* ��̬�����һ�������ڴ� */
    LOS_DL_LIST *node2 = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));/* ��̬����ڶ��������ڴ� */
    LOS_DL_LIST *tail = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));	/* ��̬����β�����ڴ� */

	printf("��ӵ�һ���ڵ���ڶ����ڵ�.....\t\r\n");
    LOS_ListAdd(head,node1);					/* ��ӵ�һ���ڵ㣬������ͷ����� */
    LOS_ListAdd(node1,node2);					/* ��ӵڶ����ڵ㣬������һ���ڵ��� */
    if ((node1->pstPrev == head) || (node2->pstPrev == node1))	/* �ж��Ƿ����ɹ� */
    {
        printf("��ӽڵ�ɹ�!\t\r\n\n");
    }
	else
	{
		printf("��ӽڵ�ʧ��!\n\n");
	}
	printf("��β�ڵ����˫�������ĩβ.....\t\r\n");
	LOS_ListTailInsert(head, tail);				/* ��β�ڵ����˫�������ĩβ */
    if (tail->pstPrev == node2)					/* �ж��Ƿ����ɹ� */		
    {
        printf("����β�ڵ���ӳɹ�!\t\r\n\n");
    }
	else
	{
		printf("����β�ڵ����ʧ��!\n\n");
	}
	
    printf("ɾ���ڵ�......\t\r\n");				/* ɾ�����нڵ� */		
    LOS_ListDelete(node1);					/* ɾ����һ���ڵ� */	
    LOS_MemFree(m_aucSysMem0, node1);		/* �ͷŵ�һ���ڵ���ڴ棬��֤����ĳ�������ڴ�ռ� */	
    if (head->pstNext == node2)				/* �ж��Ƿ�ɾ���ɹ� */
    {
        printf("ɾ���ڵ�ɹ�\t\r\n\n");
    }
    else
    {
        printf("ɾ���ڵ�ʧ��\t\r\n\n");
        
    }
	
	while(1)
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		LCD_ShowString(10, 50, 240, 24, 24, "success");
		printf("����������!\t\r\n");
		LOS_TaskDelay (2000);
	}
}


/* ����Test_Task����*/
static UINT32 Creat_Test_Task(void)
{
    UINT32 uwRet = LOS_OK;				/* ����һ����������ķ������ͣ���ʼ��Ϊ�����ɹ��ķ���ֵ */
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 4;				/* ���ȼ�����ֵԽС�����ȼ�Խ�� */
    task_init_param.pcName = "Test_Task";						/* ������ */
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Test_Task;	/* �������� */
    task_init_param.uwStackSize = 1024;						/* ��ջ��С */

    uwRet = LOS_TaskCreate(&Test_Task_Handle, &task_init_param);/* �������� */
    return uwRet;
}

int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();
    uwRet = LOS_KernelInit();
	
    extern UINT32 create_work_tasks(VOID);
    uwRet = create_work_tasks();
		
		/* ����Test_Task���� */
	uwRet = Creat_Test_Task();
    if (uwRet != LOS_OK)
    {
		printf("Test_Task���񴴽�ʧ�ܣ�\n");
    }
		(void)LOS_Start();
		return 0;
}
