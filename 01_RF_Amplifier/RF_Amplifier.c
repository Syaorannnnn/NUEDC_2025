/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "stdio.h"
#include "ti_msp_dl_config.h"
#include "././LCD/tjc_usart_hmi.h"
#include "././BTN/BTN.h"

#define DAC_MAX 	(4095)
#define DAC_VCC		(3300)
#define STEP	    (10)		//步进
#define AD_MAX		(500)

volatile uint32_t delay_times = 0;
volatile uint8_t uart_data = 0;
volatile uint32_t now_time = 0;

int main(void)
{
	SYSCFG_DL_init();
	// 清除串口中断标志
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	// 使能串口中断
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

    uint32_t dac_value = 0;
    uint32_t step_sum = 0;
    BTNData_t BTNData = {0};
    
    int count = 0;

	char str[100] = {0};
	//uint32_t last_time = 0;
	
	DL_DAC12_output12(DAC0, dac_value);
	DL_DAC12_enable(DAC0);
	/*这里的误差消除不了，固定4mV*/
	sprintf(str,"msg.txt=\"%d mV\"",0);
	tjc_send_string(str);
	while (1)
	{
		/*
         * 经过纠正加减输出的电压误差发生概率变小
         * 出现的误差大小仅有±1mV
         */
		BTN_getData(&BTNData);
				if(BTNData.left)
				{
						DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_PIN_B_PIN);
						count += 1;
						step_sum = count * STEP;
						if(step_sum > AD_MAX) count = 50;

						dac_value = (step_sum + 5) * DAC_MAX / DAC_VCC;
						DL_DAC12_output12(DAC0, dac_value);
					
						sprintf(str,"msg.txt=\"%d mV\"",step_sum);
						tjc_send_string(str);
				}
				if(BTNData.right)
				{
						DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_PIN_B_PIN);
						count -= 1;
						if(count < 0) count = 0;

						step_sum = count * STEP;
						dac_value = (step_sum + 5) * DAC_MAX / DAC_VCC;
						DL_DAC12_output12(DAC0, dac_value);
					
						sprintf(str,"msg.txt=\"%d mV\"",step_sum);
						tjc_send_string(str);
				}				
		}
}

// 串口的中断服务函数
void UART_0_INST_IRQHandler(void)
{
	// 如果产生了串口中断
	switch (DL_UART_Main_getPendingInterrupt(UART_0_INST))
	{
	case DL_UART_MAIN_IIDX_RX: // 如果是接收中断
		// 接发送过来的数据保存在变量中
		uart_data = DL_UART_Main_receiveData(UART_0_INST);
		// 将保存的数据再发送出去
		// uart0_send_char(uart_data);
		writeRingBuff(uart_data);
		break;

	default: // 其他的串口中断
		break;
	}
}
