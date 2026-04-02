#include "motor_control.h"

#include <Wire.h>
#include "board_config.h"

namespace {

constexpr uint8_t PCA9685_ADDR = 0x40;
constexpr uint8_t PCA9685_MODE1 = 0x00;
constexpr uint8_t PCA9685_MODE2 = 0x01;
constexpr uint8_t PCA9685_PRESCALE = 0xFE;
constexpr uint8_t PCA9685_LED0_ON_L = 0x06;

constexpr uint8_t PCA9685_MODE1_SLEEP = 0x10;
constexpr uint8_t PCA9685_MODE1_AI = 0x20;
constexpr uint8_t PCA9685_MODE1_RESTART = 0x80;
constexpr uint8_t PCA9685_MODE2_OUTDRV = 0x04;

constexpr uint8_t MOTOR_FL_IN1_CH = 0;
constexpr uint8_t MOTOR_FL_IN2_CH = 1;
constexpr uint8_t MOTOR_FR_IN1_CH = 2;
constexpr uint8_t MOTOR_FR_IN2_CH = 3;
constexpr uint8_t MOTOR_RL_IN1_CH = 4;
constexpr uint8_t MOTOR_RL_IN2_CH = 5;
constexpr uint8_t MOTOR_RR_IN1_CH = 6;
constexpr uint8_t MOTOR_RR_IN2_CH = 7;

uint32_t g_watchdogMs = MOTOR_WATCHDOG_DEFAULT_MS;
uint32_t g_deadlineMs = 0;
uint32_t g_lastCommandMs = 0;
uint32_t g_lastSeq = 0;
bool g_isInitialized = false;
bool g_isHardwareReady = false;
bool g_motorsActive = false;

uint8_t readRegister(uint8_t reg) {
  Wire.beginTransmission(PCA9685_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return 0;
  }
  Wire.requestFrom(static_cast<int>(PCA9685_ADDR), 1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0;
}

bool writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(PCA9685_ADDR);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool setPwm(uint8_t channel, uint16_t on, uint16_t off) {
  if (channel > 15) {
    return false;
  }

  const uint8_t reg = PCA9685_LED0_ON_L + 4 * channel;
  Wire.beginTransmission(PCA9685_ADDR);
  Wire.write(reg);
  Wire.write(on & 0xFF);
  Wire.write((on >> 8) & 0x0F);
  Wire.write(off & 0xFF);
  Wire.write((off >> 8) & 0x0F);
  return Wire.endTransmission() == 0;
}

bool setChannelDuty(uint8_t channel, uint16_t duty) {
  if (duty > 4095) {
    duty = 4095;
  }
  return setPwm(channel, 0, duty);
}

bool setPwmFrequency(uint16_t hz) {
  if (hz < 24) {
    hz = 24;
  }
  if (hz > 1526) {
    hz = 1526;
  }

  const float prescaleFloat = 25000000.0f / (4096.0f * static_cast<float>(hz)) - 1.0f;
  uint8_t prescale = static_cast<uint8_t>(prescaleFloat + 0.5f);

  uint8_t oldMode = readRegister(PCA9685_MODE1);
  if (!writeRegister(PCA9685_MODE1, static_cast<uint8_t>((oldMode & 0x7F) | PCA9685_MODE1_SLEEP))) {
    return false;
  }
  if (!writeRegister(PCA9685_PRESCALE, prescale)) {
    return false;
  }
  if (!writeRegister(PCA9685_MODE1, static_cast<uint8_t>(oldMode | PCA9685_MODE1_AI))) {
    return false;
  }
  delay(5);
  return writeRegister(PCA9685_MODE1, static_cast<uint8_t>(oldMode | PCA9685_MODE1_AI | PCA9685_MODE1_RESTART));
}

int clampSpeed(int speed) {
  if (speed > 100) {
    return 100;
  }
  if (speed < -100) {
    return -100;
  }
  return speed;
}

uint16_t speedToDuty(int speed) {
  const int absSpeed = abs(speed);
  return static_cast<uint16_t>((absSpeed * 4095) / 100);
}

void applySingleMotor(int speed, uint8_t in1Channel, uint8_t in2Channel) {
  speed = clampSpeed(speed);
  const uint16_t duty = speedToDuty(speed);

  if (speed > 0) {
    setChannelDuty(in1Channel, duty);
    setChannelDuty(in2Channel, 0);
    return;
  }

  if (speed < 0) {
    setChannelDuty(in1Channel, 0);
    setChannelDuty(in2Channel, duty);
    return;
  }

  setChannelDuty(in1Channel, 0);
  setChannelDuty(in2Channel, 0);
}

void applyWheelSpeeds(int fl, int fr, int rl, int rr) {
  applySingleMotor(fl, MOTOR_FL_IN1_CH, MOTOR_FL_IN2_CH);
  applySingleMotor(fr, MOTOR_FR_IN1_CH, MOTOR_FR_IN2_CH);
  applySingleMotor(rl, MOTOR_RL_IN1_CH, MOTOR_RL_IN2_CH);
  applySingleMotor(rr, MOTOR_RR_IN1_CH, MOTOR_RR_IN2_CH);

  g_motorsActive = !(fl == 0 && fr == 0 && rl == 0 && rr == 0);
}

}  // namespace

bool motorControlInit() {
  Wire.begin(MOTOR_I2C_SDA_PIN, MOTOR_I2C_SCL_PIN);
  Wire.setClock(400000);

  if (!writeRegister(PCA9685_MODE2, PCA9685_MODE2_OUTDRV)) {
    g_isInitialized = true;
    g_isHardwareReady = false;
    g_lastCommandMs = millis();
    return false;
  }
  if (!writeRegister(PCA9685_MODE1, PCA9685_MODE1_AI)) {
    g_isInitialized = true;
    g_isHardwareReady = false;
    g_lastCommandMs = millis();
    return false;
  }
  if (!setPwmFrequency(MOTOR_PWM_FREQ_HZ)) {
    g_isInitialized = true;
    g_isHardwareReady = false;
    g_lastCommandMs = millis();
    return false;
  }

  g_watchdogMs = MOTOR_WATCHDOG_DEFAULT_MS;
  g_deadlineMs = millis() + g_watchdogMs;
  g_lastCommandMs = millis();
  g_lastSeq = 0;
  g_isInitialized = true;
  g_isHardwareReady = true;

  motorControlStopAll();
  return true;
}

bool motorControlIsReady() {
  return g_isInitialized && g_isHardwareReady;
}

void motorControlServiceWatchdog() {
  if (!g_isInitialized) {
    return;
  }

  const uint32_t now = millis();
  const int32_t remaining = static_cast<int32_t>(g_deadlineMs - now);
  if (remaining <= 0 && g_motorsActive) {
    motorControlStopAll();
  }
}

void motorControlStopAll() {
  if (!g_isInitialized || !g_isHardwareReady) {
    return;
  }

  applyWheelSpeeds(0, 0, 0, 0);
}

MotorApplyResult motorControlApplyCommand(
  int fl,
  int fr,
  int rl,
  int rr,
  uint32_t seq,
  uint32_t ttlMs,
  bool enforceSeq,
  uint32_t *appliedWatchdogMs
) {
  if (!g_isInitialized || !g_isHardwareReady) {
    return MOTOR_APPLY_INVALID;
  }

  if (enforceSeq && seq > 0 && g_lastSeq > 0 && seq <= g_lastSeq) {
    return MOTOR_APPLY_OUT_OF_ORDER;
  }

  if (ttlMs == 0) {
    ttlMs = MOTOR_WATCHDOG_DEFAULT_MS;
  }
  if (ttlMs > MOTOR_WATCHDOG_MAX_MS) {
    ttlMs = MOTOR_WATCHDOG_MAX_MS;
  }

  g_watchdogMs = ttlMs;
  g_deadlineMs = millis() + g_watchdogMs;
  g_lastCommandMs = millis();

  if (seq > 0) {
    g_lastSeq = seq;
  }

  applyWheelSpeeds(fl, fr, rl, rr);

  if (appliedWatchdogMs) {
    *appliedWatchdogMs = g_watchdogMs;
  }

  return MOTOR_APPLY_OK;
}

uint32_t motorControlGetWatchdogMs() {
  return g_watchdogMs;
}

uint32_t motorControlGetLastSeq() {
  return g_lastSeq;
}

uint32_t motorControlGetLastCommandAgeMs() {
  const uint32_t now = millis();
  return now - g_lastCommandMs;
}
