# WearableECGMonitor

![ECG Monitor](https://www.mahmed.tech/wp-content/uploads/2020/08/PHOTO_20200401_173223.jpg)

![result](https://www.mahmed.tech/wp-content/uploads/2020/08/Screenshot-2020-03-21-20.14.36.png)

![result2](https://www.mahmed.tech/wp-content/uploads/2020/08/Screenshot-2020-03-21-20.14.38.png)

Project Report (110 pages) : https://www.mahmed.tech/wp-content/uploads/2020/05/Project_Report_MAhmed-converted-compressed.pdf 


## 1.1 Background

The IoT (internet of things) field in wearable healthcare has received a lot of attention in the last
few years due to advancement in computing paradigms: cloud computing, fog computing and
edge computing. Machine learning on the cloud has revolutionized traditional healthcare.
ECG monitors is not a new concept, it has been around the medical environment for many
decades. In the last few years, technology has started appearing in many wearable devices such
as smartwatches (apple watch 4+) and dedicated standalone products such as AliveCor's
KardiaMobile[3]. Most of the designs use 3 wires: 2 wires for bipotential input and the third wire
for body bias (common-mode rejection).


## 1.2 Aim

This project aims to design a low power battery operated wearable device with wireless
capability that can measure ECG from the bipotential single channel. The system should be
lightweight and small since it will be worn for a long period without a battery recharge.
Use professional tools & software platforms to design the system which is relevant in the
embedded systems industry.
Understand low level embedded systems and overcome problems which are specific to the
project using OEM datasheet/documentation. 


## 1.3 Objective

* Small custom-designed embedded arm cortex board using small SMD parts (0603 or
smaller).

* Develop low power wearable hardware that can be powered from a small battery.

* Develop onboard battery management system for charging lipo battery.

* Implement low power wireless capability using BLE.

* Design and implement embedded firmware using RTOS

* Use of UART, SPI, PWM, SAADC, Timers and interrupts in the firmware using
low-level hardware coding and API.

* Use of commercial development software and tools.


## 1.4 Technical Requirement / Tools

During the development phase of this project, the following tools were used, and it would be
essential to have access to these tools for future continuation of the work.

Software IDE: Keil uVision 5 

JLink Debugger / STLink Programmer: Used to flash the microcontroller.

J-Flash Light v6.54 (Tool to flash hex file)

nRF Bluetooth Connect App for Bluetooth Debugging (Android)

FTDI (USB to Serial) Module - UART communication

Eagle Autodesk to modify the schematic

Embedded C++/C, JavaScript, Bluetooth GATT, Max30003 AFE, Mbed OS, nrfSDK15

Optional: Tools that can be helpful.

SamacSys PCB Library: Excellent tool to get component footprint and save time

Notepad++ : Generic text editor, highly recommended

UART serial monitor: Arduino Serial Monitor, Termite

Plotting ECG Waveform: Arduino Plotter, Windisp (Custom Tool)

Multimeter, Oscilloscope & SMD soldering skills 


## 2. System Design 

![Systemm Design](https://www.mahmed.tech/wp-content/uploads/2020/08/digram.png)
