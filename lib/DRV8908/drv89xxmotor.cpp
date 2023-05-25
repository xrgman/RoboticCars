#include "drv89xxmotor.h"
#include "drv8910registers.h"

DRV89xxMotor::DRV89xxMotor(HalfBridge half_bridge1, HalfBridge half_bridge2, PWMChannel pwm_channel, uint8_t reverse_delay) {
    this->pwm_channel = pwm_channel;
    this->reverse_delay = reverse_delay;

    //Configuring half bridge settings:
    populateHalfbridges(0, half_bridge1);
    populateHalfbridges(1, half_bridge2);
}

//*****************************************
//******** Motor control functions ********
//*****************************************

void DRV89xxMotor::set(uint8_t *config_cache, uint8_t speed, Direction direction) {
    //Is this even usefull?
    enabled = true;
    speed = speed;
    direction = direction;

    switch(direction) {
        case FORWARD: 
            setBridgeLowsideDisablePWM(config_cache, half_bridges[0]);
            setBridgeHighsideEnablePWM(config_cache, half_bridges[1]);
            setPWMFrequency(config_cache, speed);
            break;
        case REVERSE:
            setBridgeHighsideEnablePWM(config_cache, half_bridges[0]);
            setBridgeLowsideDisablePWM(config_cache, half_bridges[1]);
            setPWMFrequency(config_cache, speed);
            break;
        case BRAKE: 
        default: 
            setBridgeLowsideDisablePWM(config_cache, half_bridges[0]);
            setBridgeLowsideDisablePWM(config_cache, half_bridges[1]);
            break;
    }

    //Update the actual things:
}

void DRV89xxMotor::disable(uint8_t *config_cache) {
    enabled = false;

    setBridgeOpen(config_cache, half_bridges[0]);
    setBridgeOpen(config_cache, half_bridges[1]);
}

//*****************************************
//******** Motor configuration functions **
//*****************************************

void DRV89xxMotor::populateHalfbridges(uint8_t motor_hb_id, HalfBridge half_bridge_id) {
    half_bridges[motor_hb_id].id = half_bridge_id;

    //Checking if half bridge id is valid:
    if(half_bridge_id <= 0 || half_bridge_id > 12) {
        return;
    }

    //Setting the correct registers for the motor:
    half_bridges[motor_hb_id].enable_register = OP_CTRL_1 + (half_bridge_id - 1) / 4;
    half_bridges[motor_hb_id].pwm_map_register = PWM_MAP_CTRL_1 + (half_bridge_id - 1) / 4;
    half_bridges[motor_hb_id].pwm_ctrl_register = PWM_CTRL_1 + (half_bridge_id - 1) / 8;

    // This bitshift is used for PWM ctrl (1 bytes per hb)
    half_bridges[motor_hb_id].bitshift_1 = ((half_bridge_id - 1) % 8); // hb1 = bitshift 0, hb2 = bitshift 1, ..., hb12 = bitshift 4

    // This bitshift is used for PWM map, and ctrl (2 bytes per hb)
    half_bridges[motor_hb_id].bitshift_2 = ((half_bridge_id - 1) % 4) * 2; // hb1 = bitshift 0, hb2 = bitshift 2, ..., hb12 = bitshift 6
}

void DRV89xxMotor::setBridgeLowsideDisablePWM(uint8_t *config_cache, DRV89xxHalfBridge &bridge) {
    //Skipping for unconfigured bridge:
    if(bridge.id == 0) {
        return;
    }

    BIT_SET(config_cache[bridge.enable_register], bridge.bitshift_2); // Enable the lowside of the half bridge
    BIT_CLEAR(config_cache[bridge.enable_register], bridge.bitshift_2 + 1); //Disable the highside of the half bridge
    BIT_CLEAR(config_cache[bridge.pwm_ctrl_register], bridge.bitshift_1); //Disable PWM of the half bridge 
}

void DRV89xxMotor::setBridgeHighsideEnablePWM(uint8_t *config_cache, DRV89xxHalfBridge &bridge) {
    //Skipping for unconfigured bridge:
    if(bridge.id == 0) {
        return;
    }

    BIT_SET(config_cache[bridge.enable_register], bridge.bitshift_2 + 1); // Enable the highside of the half bridge
    BIT_CLEAR(config_cache[bridge.enable_register], bridge.bitshift_2); //Disable the lowside of the half bridge
    BIT_SET(config_cache[bridge.pwm_ctrl_register], bridge.bitshift_1); //Enable PWM of the half bridge 

    //Configuring the correct PWM channel:
    if(pwm_channel == PWM_CHANNEL_2 || PWM_CHANNEL_4) {
        BIT_SET(config_cache[bridge.pwm_map_register], bridge.bitshift_2);
    }
    else {
        BIT_CLEAR(config_cache[bridge.pwm_map_register], bridge.bitshift_2);
    }

    if(pwm_channel == PWM_CHANNEL_3 || PWM_CHANNEL_4) {
        BIT_SET(config_cache[bridge.pwm_map_register], bridge.bitshift_2 + 1);
    }
    else {
        BIT_CLEAR(config_cache[bridge.pwm_map_register], bridge.bitshift_2 + 1);
    }
}

void DRV89xxMotor::setBridgeOpen(uint8_t *config_cache, DRV89xxHalfBridge &bridge) {
    //Skipping for unconfigured bridge:
    if(bridge.id == 0) {
        return;
    }

    BIT_CLEAR(config_cache[bridge.enable_register], bridge.bitshift_2); //Disable the lowside of the half bridge
    BIT_CLEAR(config_cache[bridge.enable_register], bridge.bitshift_2 + 1); //Disable the highside of the half bridge
    BIT_CLEAR(config_cache[bridge.pwm_ctrl_register], bridge.bitshift_1); //Disable PWM of the half bridge 
}

void DRV89xxMotor::setPWMFrequency(uint8_t *config_cache, uint8_t speed) {
    config_cache[PWM_DUTY_CTRL_1 + pwm_channel] = speed;
}