#ifndef PWM_H
#define PWM_H

#include <stdint.h>

// initializes timer 2 channel 1 for 1kHz PWM on PA0
void PWM_Init(void);

// updates the duty cycle of the electromagnet (0 - 999)
void PWM_SetDutyCycle(uint16_t duty);

#endif