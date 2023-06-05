#ifndef DRV89XXENUMS_H
#define DRV89XXENUMS_H

#define MOTOR_FRONT_LEFT 0
#define MOTOR_FRONT_RIGHT 3
#define MOTOR_BACK_LEFT 2
#define MOTOR_BACK_RIGHT 1

typedef enum
{
    FORWARD = 0,
    REVERSE,
    BRAKE
} Direction;

typedef enum  {
    NO_HALF_BRIDGE = 0, //DO not use!
    HALF_BRIDGE_1,
    HALF_BRIDGE_2,
    HALF_BRIDGE_3,
    HALF_BRIDGE_4,
    HALF_BRIDGE_5,
    HALF_BRIDGE_6,
    HALF_BRIDGE_7,
    HALF_BRIDGE_8,
    HALF_BRIDGE_9,
    HALF_BRIDGE_10,
    HALF_BRIDGE_11,
    HALF_BRIDGE_12
} HalfBridge;

typedef enum
{
    PWM_CHANNEL_1,
    PWM_CHANNEL_2,
    PWM_CHANNEL_3,
    PWM_CHANNEL_4
} PWMChannel;



#endif