
Pololu DRV8835 Dual Motor Driver Kit for Raspberry Pi B+
Pololu 5V Step-Up/Step-Down Voltage Regulator S7V7F5

MD = Motor Driver
VR = Voltage Regulator

Batt -> MD:VIN -> [1.5-11 V] -> MD:VOUT
MD:VOUT -> DRV8835 -> Motor 1, Motor 2
MD:VOUT -> VR:VIN -> [5 V] -> VR:VOUT
VR:VOUT -> MD:5V -> GPIO -> [3.3 V] -> MD:VCC
MD:VCC -> DRV8835
