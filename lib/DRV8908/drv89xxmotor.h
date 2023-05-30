#ifndef DRV89XXMOTOR_H
#define DRV89XXMOTOR_H

#include "mbed.h"
#include "drv89xxenums.h"

// from https://stackoverflow.com/a/263738/346227
#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))        // '!!' to make sure this returns 0 or 1

typedef struct { 
    uint8_t id;
    uint8_t enable_register;
    uint8_t pwm_map_register;
    uint8_t pwm_ctrl_register;
    uint8_t bitshift_1;  // used for pwm_ctrl
    uint8_t bitshift_2;  // used for enable, and pwm_map
} DRV89xxHalfBridge;

class DRV89xxMotor {
    public:
        DRV89xxMotor() : DRV89xxMotor(NO_HALF_BRIDGE, NO_HALF_BRIDGE, PWM_CHANNEL_1, 0, false) {};
        DRV89xxMotor(HalfBridge half_bridge1, HalfBridge half_bridge2, PWMChannel pwm_channel, uint8_t reverse_delay, bool isReversed);

        void set(uint8_t *config_cache, uint8_t speed, Direction direction);
        void disable(uint8_t *config_cache);

    private:
        bool isReversed;
        uint8_t reverse_delay = 0; // number of milliseconds to brake before reversing direction
        PWMChannel pwm_channel;
        Direction direction;
        bool enabled;

        DRV89xxHalfBridge half_bridges[2];

        void populateHalfbridges(uint8_t motor_hb_id, HalfBridge half_bridge_id);
        void setBridgeLowsideDisablePWM(uint8_t *config_cache, DRV89xxHalfBridge &bridge);
        void setBridgeHighsideEnablePWM(uint8_t *config_cache, DRV89xxHalfBridge &bridge);
        void setBridgeOpen(uint8_t *config_cache, DRV89xxHalfBridge &bridge);
        void setPWMFrequency(uint8_t *config_cache, uint8_t speed);
};

#endif