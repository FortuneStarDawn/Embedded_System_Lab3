#include <msp430.h>
#include <intrinsics.h>
#define LED1 BIT6
#define LED2 BIT0
#define B1 BIT3

volatile unsigned int count=0, down=0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    P1DIR = LED1 + LED2;
    P1OUT = B1;
    P1REN |= B1;
    P1IE |= B1;
    P1IES |= B1;
    P1IFG &= ~B1;
    _BIS_SR(GIE);
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ; // DCO = 1Mz
    BCSCTL2 |= DIVS_3;
    BCSCTL3 |= LFXT1S_2;
    TA0CCR0 = 449;
    TA1CCR0 = 3124;
    TA0CCTL0 = CCIE;
    TA1CCTL0 = CCIE;
    TA0CTL = MC_1|ID_3|TASSEL_1|TACLR; //1500 Hz
    TA1CTL = MC_1|ID_3|TASSEL_2|TACLR; //15625 Hz
    __enable_interrupt();
    for(;;) { }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR (void)
{
	if(down==1)
	{
		if(count<6)
		{
			P1OUT ^= LED1;
			if(TA0CCR0==449) TA0CCR0 = 1049;
			else TA0CCR0 = 449;
			count++;
			if(count==6)
			{
				P1OUT &= ~LED1;
				P1OUT |= LED2;
				TA1CCR0 = 3124;
				TA1CTL |= TACLR;
			}
		}
	}
	TA0CTL |= TACLR;
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void TA1_ISR (void)
{
	if(count==6)
	{
		P1OUT ^= LED2;
		if(TA1CCR0==3124) TA1CCR0 = 12499;
		else TA1CCR0 = 3124;
	}
	TA1CTL |= TACLR;
}

#pragma vector = PORT1_VECTOR
__interrupt void Port_1 (void)
{
	P1IFG &= ~B1;
	if(P1IES & B1)
	{
		P1IES &= ~B1;
		down=1;
		P1OUT |= LED1;
		TA0CCR0 = 449;
		TA0CTL |= TACLR;
	}
	else
	{
		P1IES |= B1;
		down=0;
		count=0;
		P1OUT &= ~LED1;
		P1OUT &= ~LED2;
	}
}
