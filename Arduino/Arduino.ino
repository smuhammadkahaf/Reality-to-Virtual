#include <Wire.h>
#include <math.h>

/* =====================================================
   ================= FLEX SENSOR ======================
   ===================================================== */

const int flex_pin = A1;
float adc0  = 621.51;
float adc90 = 388.05;
const int SAMPLES = 10;

float getAngle() {
  long sum = 0;
  for (int i = 0; i < SAMPLES; i++) {
    sum += analogRead(flex_pin);
    delay(5);
  }

  float adc = sum / (float)SAMPLES;
  adc = constrain(adc, min(adc0, adc90), max(adc0, adc90));

  float angle = (adc - adc0) * 90.0 / (adc90 - adc0);
  angle = constrain(angle, 0.0, 90.0);

  return -angle;   // intentional
}

/* =====================================================
   ================= MPU6050 ===========================
   ===================================================== */

const int MPU_ADDR = 0x68;

/* ---- Raw data ---- */
int16_t raw_ax, raw_ay, raw_az;
int16_t raw_gx, raw_gy, raw_gz;

/* ---- Gyro offsets ---- */
float gyro_offset_x, gyro_offset_y, gyro_offset_z;

/* ---- Orientation angles (deg) ---- */
float angle_x = 0;
float angle_y = 0;
float angle_z = 0;

/* ---- Counter-drift rates (deg/s) ---- */
float drift_x = 0;
float drift_y = 0;
float drift_z = 0;

/* ---- Time ---- */
unsigned long lastTime;
float dt;

/* ---- Tunable parameters ---- */
const float COMPLEMENTARY_ALPHA = 0.98;
const float STILL_GYRO_THRESHOLD = 0.04;   // deg/s
const float DRIFT_LEARN_RATE     = 0.002;  // smaller = slower correction

/* =====================================================
   ================= SETUP =============================
   ===================================================== */

void setup() {
  Serial.begin(115200);
  Wire.begin();

  initializeMPU();
  calibrateGyro();

  lastTime = millis();
}

/* =====================================================
   ================= LOOP ==============================
   ===================================================== */

void loop() {
  calculateDeltaTime();
  readMPU();
  updateOrientation();
  learnAndApplyCounterDrift();
  printOutput();
  delay(10);
}

/* =====================================================
   ================= MPU CORE ==========================
   ===================================================== */

void initializeMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  delay(100);
}

void calibrateGyro() {
  const int samples = 300;
  long sx = 0, sy = 0, sz = 0;

  for (int i = 0; i < samples; i++) {
    readMPU();
    sx += raw_gx;
    sy += raw_gy;
    sz += raw_gz;
    delay(5);
  }

  gyro_offset_x = sx / (float)samples;
  gyro_offset_y = sy / (float)samples;
  gyro_offset_z = sz / (float)samples;
}

void readMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  raw_ax = Wire.read() << 8 | Wire.read();
  raw_ay = Wire.read() << 8 | Wire.read();
  raw_az = Wire.read() << 8 | Wire.read();
  Wire.read(); Wire.read();   // temperature

  raw_gx = Wire.read() << 8 | Wire.read();
  raw_gy = Wire.read() << 8 | Wire.read();
  raw_gz = Wire.read() << 8 | Wire.read();
}

/* =====================================================
   ========== ORIENTATION UPDATE =======================
   ===================================================== */

void updateOrientation() {

  // Gyro (deg/s)
  float gx = (raw_gx - gyro_offset_x) / 131.0 - drift_x;
  float gy = (raw_gy - gyro_offset_y) / 131.0 - drift_y;
  float gz = (raw_gz - gyro_offset_z) / 131.0 - drift_z;

  // Integrate
  angle_x += gx * dt;
  angle_y += gy * dt;
  angle_z += gz * dt;

  // Accelerometer correction (absolute reference)
  float ax = raw_ax / 16384.0;
  float ay = raw_ay / 16384.0;
  float az = raw_az / 16384.0;

  float accel_roll  = atan2(ay, az) * 180.0 / PI;
  float accel_pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;

  // Complementary fusion (X, Y only)
  angle_x = COMPLEMENTARY_ALPHA * angle_x + (1 - COMPLEMENTARY_ALPHA) * accel_roll;
  angle_y = COMPLEMENTARY_ALPHA * angle_y + (1 - COMPLEMENTARY_ALPHA) * accel_pitch;
}

/* =====================================================
   ========== ACTIVE COUNTER-DRIFT =====================
   ===================================================== */

void learnAndApplyCounterDrift() {

  float gx = abs((raw_gx - gyro_offset_x) / 131.0);
  float gy = abs((raw_gy - gyro_offset_y) / 131.0);
  float gz = abs((raw_gz - gyro_offset_z) / 131.0);

  bool still = (gx < STILL_GYRO_THRESHOLD &&
                gy < STILL_GYRO_THRESHOLD &&
                gz < STILL_GYRO_THRESHOLD);

  if (still) {
    // Learn drift only when not moving
    drift_x += DRIFT_LEARN_RATE * angle_x;
    drift_y += DRIFT_LEARN_RATE * angle_y;
    drift_z += DRIFT_LEARN_RATE * angle_z;
  }
}

/* =====================================================
   ================= TIME ==============================
   ===================================================== */

void calculateDeltaTime() {
  unsigned long now = millis();
  dt = (now - lastTime) / 1000.0;
  lastTime = now;
}

/* =====================================================
   ================= OUTPUT ============================
   ===================================================== */

void printOutput() {
  Serial.print(getAngle(), 2);
  Serial.print(", (");
  Serial.print(angle_x, 2); Serial.print(",");
  Serial.print(angle_y, 2); Serial.print(",");
  Serial.print(angle_z, 2);
  Serial.println(")");
}