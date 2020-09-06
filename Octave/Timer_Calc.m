%STM32 ROMI Timer Test
%Generate 500mSec timer 

clc
clear

%Input Timer Clock HZ
TIM6_CLK = 50e6 %HZ
printf("\n");

%The timer interrupt period in seconds
target_interupt_sec = 100e-3
target_interupt_msec = target_interupt_sec*1000
target_interupt_sec_uSec = target_interupt_msec*1000
printf("\n");

%The prescaler can divide the counter clock frequency by any factor between 1 and 65536
prescaler = 800
printf("\n");

CNT_CLK = TIM6_CLK/(prescaler+1);

timer_period = 1/CNT_CLK; %Sec

%Target period value of interrupt
period_value = ((target_interupt_sec)/timer_period)-1

Extra_Value = (2^16) - period_value