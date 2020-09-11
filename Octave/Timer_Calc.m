%STM32 ROMI Timer Test
%Generate 500mSec timer 
clc
clear

printf("*********\n");
printf("STM32 Timer Calc\n");
printf("*********\n");
printf("\n");


%Input Timer Clock HZ - APB1 Bus for this timer
TIM6_CLK = 50e6; %HZ
printf("Source CLK For TIM6 %d (Hz)\n",TIM6_CLK);
printf("\n");

%The timer interrupt period in seconds
target_interupt_sec = 1/250e3;
target_interupt_msec = target_interupt_sec*1000;
target_interupt_uSec = target_interupt_msec*1000;
printf("Target Timer Interupt %d (Sec)\n",target_interupt_sec );
printf("Target Timer Interupt %d (mSec)\n",target_interupt_msec);
printf("Target Timer Interupt %d (uSec)\n",target_interupt_uSec);
printf("\n");

%The prescaler can divide the counter clock frequency by any factor between 1 and 65536
prescaler = 9
printf("\n");

CNT_CLK = TIM6_CLK/(prescaler+1);

timer_period = 1/CNT_CLK; %Sec

%Target period value of interrupt
period_value = ((target_interupt_sec)/timer_period)-1

Extra_Value = (2^16) - period_value