clear
clc

printf("**********\n Sonar Timer\n **********\n\r")

tick = 59;
tock = 65;
SpeedOfSound = 0.0343/2;
ScaleFactor = 2.8
%distance = 7 %cm
distance = (tock-tick)*ScaleFactor*SpeedOfSound
printf("Tick = %d\t Tock = %d\t Tock-Tick = %d\n\r",tick,tock,tock-tick)