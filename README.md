# STM32-ROMI

## Project Description

The Silicon Valley [HomeBrew Robotics Club](http://www.hbrobotics.org/) built an awesome [FPGA ROMI](https://github.com/hbrc-fpga-class) robot late 2019. I was inspired to build my own version using an STM32F4  while keeping the board compatable with the previous Polulu ROMI base. The purpose of this was to familiarize myself with the STM32 environment and get a cool robot out of it.
 
 
## Project Status
Boards have been sent for PCBA
 **This project is still a work in progress**


![3D Render](https://github.com/Elipsit/STM32-ROMI/blob/master/Pics/Render_Top.png)

## Electrical Design Files
This project was designed in Altium Designer 20

## Datasheets
- [DRV8838 Motor Driver](https://www.ti.com/lit/ds/symlink/drv8838.pdf?HQS=TI-null-null-digikeymode-df-pf-null-wwe&ts=1599069814852)

## Motor Driver

The DRV8838 offers a simple two-pin PHASE/ENABLE control interface, which this board makes available for each motor as DIR and PWM, respectively. The DIR pin determines the motor direction (low drives the motor forward, high drives it in reverse) and the PWM pin can be supplied with a PWM signal to control the motor speed. The DIR and PWM control inputs are pulled low through weak internal pull-down resistors (approximately 100 kΩ). When the PWM pin is low, the motor outputs are both shorted to ground, which results in dynamic braking of a connected motor.

The two drivers’ SLEEP pins (labeled SLP) are connected together by default and can be driven low to put the drivers into a low-power sleep mode and turn off the motor outputs, which is useful if you want to let the motors coast. The SLEEP pins are pulled high through 10 kΩ pull-up resistors on the board so that the drivers are awake by default. In most applications, these pins can be left disconnected; if you want independent control of SLEEP on each side, you can cut the jumper labeled SLP L = R. The two SLEEP pins should not be driven separately without cutting this jumper.

The following simplified truth table shows how each driver operates:

### Motor Driver Truth Table
![Truth Table](https://github.com/Elipsit/STM32-ROMI/blob/master/Pics/DRV8838_Truth_Table.png)

## PCBA
This project was manufactured by JLCPCB as an excuse for me to test their PCBA service.
*They didn't sponcer me I just wanted to try it out*


## Requited Parts

- [Romi Chassis Kit](https://www.pololu.com/product/3506)

- [Romi Encoder Pair Kit, 12 CPR, 3.5-18V](https://www.pololu.com/product/3542)

- [Motor Driver and Power Distribution Board for Romi Chassis](https://www.pololu.com/product/3543)

- [QTR-1A Reflectance Sensor (2-Pack)](https://www.pololu.com/product/2458)

- [0.96 Inch OLED](https://www.amazon.com/UCTRONICS-SSD1306-Self-Luminous-Display-Raspberry/dp/B072Q2X2LL/ref=sr_1_3?dchild=1&keywords=oled+0.96&qid=1598137389&sr=8-3)

- [HC-SRO4](https://www.amazon.com/Smraza-Ultrasonic-Distance-Mounting-Duemilanove/dp/B01JG09DCK/ref=sr_1_6?dchild=1&keywords=sonar+arduino&qid=1598137419&sr=8-6)

## 3D Print Files
- [Sonar Servo Mount](https://www.thingiverse.com/thing:1423)

