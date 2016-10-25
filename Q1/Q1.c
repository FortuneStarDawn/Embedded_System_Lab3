#include <msp430.h>
#include <intrinsics.h>
#define LED1 BIT0
#define LED2 BIT6
#define B1 BIT3

volatile unsigned int mode=0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    P1DIR = LED1 + LED2;
    P1OUT = LED1 + B1;
    P1REN |= B1;
    P1IE |= B1;
    P1IFG &= ~B1;
    _BIS_SR(GIE);
    BCSCTL3 |= LFXT1S_2;
    TA0CCR0 = 449;
    TA0CCTL0 = CCIE;
    TA0CTL = MC_1|ID_3|TASSEL_1|TACLR; //1500 Hz
    __enable_interrupt();
    for(;;) { }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR (void)
{
	if(mode==0) P1OUT ^= LED1;
	else P1OUT ^= LED2;
	if(TA0CCR0==449) TA0CCR0 = 1049;
	else TA0CCR0 = 449;
	TA0CTL |= TACLR;
}

#pragma vector = PORT1_VECTOR
__interrupt void Port_1 (void)
{
	P1IFG &= ~B1;
	if(mode==0)
	{
		mode = 1;
		P1OUT &= ~LED1;
		P1OUT |= LED2;
	}
	else
	{
		mode = 0;
		P1OUT &= ~LED2;
		P1OUT |= LED1;
	}
	TA0CCR0 = 449;
	TA0CTL |= TACLR;
}
