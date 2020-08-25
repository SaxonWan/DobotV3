/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
	 #include "stdio.h"
	 #include "Protocol.h"
	 #include "emw3080.h"
	 #include "string.h"
	 #include "tim.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */
	 #define open_service_receive 		tim4uart3 = 300
#define close_service_receive 		tim4uart3 = 400
#define open_cam_receive			tim4uart1 = 300
#define close_cam_receive			tim4uart1 = 400

	#define MAX_LEN 400
	extern uint8_t uart4buffer;
	extern uint8_t uart4rbuffer;
	extern uint8_t uart1rbuffer;
	extern uint8_t uart3rbuffer;
	extern char wifi_receive_buffer[MAX_LEN];
	extern char	uart1_receive_buffer[MAX_LEN];
	extern char uart1_transmit_buffer[MAX_LEN];
	extern int 	uart1_receive_len;
/* USER CODE END Private defines */

void MX_UART4_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
	int fputc(int ch, FILE *f);
	void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
	void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
	bool check_send_command(const char* cmd,const char *resp,int timeout);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
