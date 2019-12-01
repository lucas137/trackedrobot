_______________________________________________________________________________

Dagu Rover 5 chassis


Motor rated voltage        7.2 V
Motor stall current        2.5 A
Output shaft stall torque  10 kg/cm
Gearbox ratio              86.8:1
Encoder type               Quadrature
Encoder resolution         1000 state changes per 3 wheel rotations
Encoder voltage            5 V
Speed                      1 km/hr (0.278 m/s)


_______________________________________________________________________________

Motor Output (4.5-12V, 4.5A max)

  CH1   Channel 1
  CH2   Channel 2
  CH3   Channel 3
  CH4   Channel 4

Control Input/Output

  J1    CH1 and CH2
  J2    CH3 and CH4

    CH1   CH2
    CH3   CH4
    1     5     GND   Ground                  0V
    2     6     PWM   Motor speed input       0V off, 5V max
    3     7     CUR   Motor current output    1V per A drawn by motor
    4     8     DIR   Motor direction input   5V forward(?), 0V reverse(?)

Encoder Input/Output

  J3    Encoder 1
  J4    Encoder 2
  J5    Encoder 3
  J6    Encoder 4

    1   VCC   5V
    2   INT   Interrupt output
    3   AIN   Signal A input
    4   AOUT  Signal A output
    5   BIN   Signal B input
    6   BOUT  Signal B output
    7   GND   Ground
    8   GND   Ground

Power Input

  J9    Motor Power

    1   UB    4.5-12V
    2   GND   Ground

  J10   Vlogic

    1   VCC   5V
    2   GND   Ground

_______________________________________________________________________________
