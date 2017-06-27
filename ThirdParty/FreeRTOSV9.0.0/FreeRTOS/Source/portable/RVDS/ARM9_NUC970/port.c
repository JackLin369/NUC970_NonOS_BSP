/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/


/* Standard includes. */
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Nuvoton includes. */
#include "nuc970.h"
#include "sys.h"

/* Constants required to setup the initial task context. */
#define portINITIAL_SPSR				( ( StackType_t ) 0x1f ) /* System mode, ARM mode, interrupts enabled. */
#define portTHUMB_MODE_BIT				( ( StackType_t ) 0x20 )
#define portINSTRUCTION_SIZE			( ( StackType_t ) 4 )
#define portNO_CRITICAL_SECTION_NESTING	( ( StackType_t ) 0 )

/*-----------------------------------------------------------*/

/* The code generated by the Keil compiler does not maintain separate
stack and frame pointers. The portENTER_CRITICAL macro cannot therefore
use the stack as per other ports.  Instead a variable is used to keep
track of the critical section nesting.  This variable has to be stored
as part of the task context and must be initialised to a non zero value. */

#define portNO_CRITICAL_NESTING		( ( uint32_t ) 0 )
volatile uint32_t ulCriticalNesting = 9999UL;

/*-----------------------------------------------------------*/

/* Setup the timer to generate the tick interrupts. */
static void prvSetupTimerInterrupt( void );

/* 
 * The scheduler can only be started from ARM mode, so 
 * vPortStartFirstSTask() is defined in portISR.c. 
 */
extern __asm void vPortStartFirstTask( void );

/*-----------------------------------------------------------*/

/* 
 * See header file for description. 
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
StackType_t *pxOriginalTOS;

	/* Setup the initial stack of the task.  The stack is set exactly as 
	expected by the portRESTORE_CONTEXT() macro.

	Remember where the top of the (simulated) stack is before we place 
	anything on it. */
	pxOriginalTOS = pxTopOfStack;
	
	/* To ensure asserts in tasks.c don't fail, although in this case the assert
	is not really required. */
	pxTopOfStack--;

	/* First on the stack is the return address - which in this case is the
	start of the task.  The offset is added to make the return address appear
	as it would within an IRQ ISR. */
	*pxTopOfStack = ( StackType_t ) pxCode + portINSTRUCTION_SIZE;		
	pxTopOfStack--;

	*pxTopOfStack = ( StackType_t ) 0xaaaaaaaa;	/* R14 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) pxOriginalTOS; /* Stack used when task starts goes in R13. */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x12121212;	/* R12 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x11111111;	/* R11 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x10101010;	/* R10 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x09090909;	/* R9 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x08080808;	/* R8 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x07070707;	/* R7 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x06060606;	/* R6 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x05050505;	/* R5 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x04040404;	/* R4 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x03030303;	/* R3 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x02020202;	/* R2 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x01010101;	/* R1 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) pvParameters; /* R0 */
	pxTopOfStack--;

	/* The last thing onto the stack is the status register, which is set for
	system mode, with interrupts enabled. */
	*pxTopOfStack = ( StackType_t ) portINITIAL_SPSR;

	if( ( ( uint32_t ) pxCode & 0x01UL ) != 0x00UL )
	{
		/* We want the task to start in thumb mode. */
		*pxTopOfStack |= portTHUMB_MODE_BIT;
	}

	pxTopOfStack--;

	/* The code generated by the Keil compiler does not maintain separate
	stack and frame pointers. The portENTER_CRITICAL macro cannot therefore
	use the stack as per other ports.  Instead a variable is used to keep
	track of the critical section nesting.  This variable has to be stored
	as part of the task context and is initially set to zero. */
	*pxTopOfStack = portNO_CRITICAL_SECTION_NESTING;

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
	/* Start the timer that generates the tick ISR. */
	prvSetupTimerInterrupt();

	/* Start the first task.  This is done from portISR.c as ARM mode must be
	used. */
	vPortStartFirstTask();

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the ARM port will require this function as there
	is nothing to return to.  If this is required - stop the tick ISR then
	return back to main. */
}
/*-----------------------------------------------------------*/

/* Interrupt Handler */
void systemIrqHandler(UINT32 _mIPER, UINT32 _mISNR)
{		
    _mIPER = (_mIPER >> 2) & 0x3f;
		
    if (_mISNR != 0)
        if (_mIPER == _mISNR)
            (*sysIrqHandlerTable[_mIPER])();
		
    outpw(REG_AIC_EOSCR, 1);
}

#if configUSE_PREEMPTION == 0

	/* 
	 * The cooperative scheduler requires a normal IRQ service routine to 
	 * simply increment the system tick. 
	 */
	void vNonPreemptiveTick( void ) __irq;
	void vNonPreemptiveTick( void ) __irq
	{
		UINT32 volatile _mIPER, _mISNR;

		/* Increment the tick count - this may make a delaying task ready
		to run - but a context switch is not performed. */		
		xTaskIncrementTick();

		_mIPER = inpw(REG_AIC_IPER);
		_mISNR = inpw(REG_AIC_ISNR);
		if (_mISNR != TMR1_IRQn)
			systemIrqHandler(_mIPER, _mISNR);

		// clear TIF1
		outpw(REG_TMR_TISR, 0x2);
		// Acknowledge the Interrupt
		outpw(REG_AIC_EOSCR, 0x01);
	}

 #else

	/*
	 **************************************************************************
	 * The preemptive scheduler ISR is written in assembler and can be found   
	 * in the portASM.s file. This will only get used if portUSE_PREEMPTION
	 * is set to 1 in portmacro.h
	 ************************************************************************** 
	 */

	void vPreemptiveTick( void );

#endif
/*-----------------------------------------------------------*/

static void prvSetupTimerInterrupt( void )
{
uint32_t ulCompareMatch;

	// enable the Timer 1 clock	
  outpw(REG_CLK_PCLKEN0, inpw(REG_CLK_PCLKEN0)| 0x200);
  outp32(REG_SYS_APBIPRST0, inp32(REG_SYS_APBIPRST0) | BIT9);
	outp32(REG_SYS_APBIPRST0, inp32(REG_SYS_APBIPRST0) & ~BIT9);

	/* Calculate the match value required for our wanted tick rate. */
	//ulCompareMatch = EXTERNAL_CRYSTAL_CLOCK / configTICK_RATE_HZ;
	ulCompareMatch = 12000000 / configTICK_RATE_HZ;

	/* Protect against divide by zero.  Using an if() statement still results
	in a warning - hence the #if. */
	#if portPRESCALE_VALUE != 0
	{
		ulCompareMatch /= ( portPRESCALE_VALUE + 1 );
	}
	#endif

	/* Setup the AIC for the timer. */
	#if 0
	/* The ISR installed depends on whether the preemptive or cooperative
	scheduler is being used. */
	#if configUSE_PREEMPTION == 1
	{	
		sysInstallISR(IRQ_LEVEL_1, TMR1_IRQn, (PVOID)vPreemptiveTick);
	}
	#else
	{
		sysInstallISR(IRQ_LEVEL_1, TMR1_IRQn, (PVOID)vNonPreemptiveTick);
	}
	#endif
	#endif
	
	_sys_bIsAICInitial = TRUE;
	
#if configUSE_PREEMPTION == 1	
	sysInstallIrqHandler(vPreemptiveTick);
#endif	

	sysEnableInterrupt(TMR1_IRQn);

	outpw(REG_TMR1_TICR, ulCompareMatch);
	outpw(REG_TMR1_TCSR, (inpw(REG_TMR1_TCSR) & 0x81FFFF00) | (0xD << 27));		// 0xC means CEN and IE were enable	
	
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	/* Disable interrupts as per portDISABLE_INTERRUPTS(); 							*/
	__disable_irq();

	/* Now interrupts are disabled ulCriticalNesting can be accessed 
	directly.  Increment ulCriticalNesting to keep a count of how many times
	portENTER_CRITICAL() has been called. */
	ulCriticalNesting++;
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	if( ulCriticalNesting > portNO_CRITICAL_NESTING )
	{
		/* Decrement the nesting count as we are leaving a critical section. */
		ulCriticalNesting--;

		/* If the nesting level has reached zero then interrupts should be
		re-enabled. */
		if( ulCriticalNesting == portNO_CRITICAL_NESTING )
		{
			/* Enable interrupts as per portEXIT_CRITICAL(). */
			__enable_irq();
		}
	}
}
/*-----------------------------------------------------------*/


