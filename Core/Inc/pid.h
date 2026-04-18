#ifndef PID_H
#define PID_H

#include <stdint.h>

typedef struct {
    float Kp;
    float Ki;
    float Kd;

    float setpoint; // target

    float integral_error;
    float prev_error;

    // o/p limits
    float limit_max;
    float limit_min;
} PID_Controller;

// initialize the PID structure with starting values
void PID_Init(PID_Controller *pid, float kp, float ki, float kd, float setpoint);

float PID_Compute(PID_Controller *pid, float current_pos);

#endif