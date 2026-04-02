#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

enum MotorApplyResult {
  MOTOR_APPLY_OK = 0,
  MOTOR_APPLY_OUT_OF_ORDER = 1,
  MOTOR_APPLY_INVALID = 2
};

bool motorControlInit();
bool motorControlIsReady();
void motorControlServiceWatchdog();
void motorControlStopAll();

MotorApplyResult motorControlApplyCommand(
  int fl,
  int fr,
  int rl,
  int rr,
  uint32_t seq,
  uint32_t ttlMs,
  bool enforceSeq,
  uint32_t *appliedWatchdogMs
);

uint32_t motorControlGetWatchdogMs();
uint32_t motorControlGetLastSeq();
uint32_t motorControlGetLastCommandAgeMs();

#endif
