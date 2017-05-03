/*=============================================================================
   Copyright (c) 2016 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_INTERRUPTS_H_MAY_3_2017)
#define CYCFI_INFINITY_INTERRUPTS_H_MAY_3_2017

///////////////////////////////////////////////////////////////////////////////
//            Cortex-M4 Processor Exceptions Handlers
///////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

	void NMI_Handler(void);
	void HardFault_Handler(void);
	void MemManage_Handler(void);
	void BusFault_Handler(void);
	void UsageFault_Handler(void);
	void SVC_Handler(void);
	void DebugMon_Handler(void);
	void PendSV_Handler(void);
	void SysTick_Handler(void);
	void USER_BUTTON_IRQHANDLER(void);

#ifdef __cplusplus
}
#endif

#endif
