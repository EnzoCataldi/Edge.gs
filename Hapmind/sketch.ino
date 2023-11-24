#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MPU6050.h>

MPU6050 mpu;
LiquidCrystal_I2C lcd(0x27, 20, 4); // Endereço I2C do LCD 20x4

const int LED_ANSIEDADE = 2; 
const int LED_PANICO = 4;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  mpu.initialize();
  lcd.init();
  lcd.backlight();

  pinMode(LED_ANSIEDADE, OUTPUT);
  pinMode(LED_PANICO, OUTPUT);

  if (!mpu.testConnection()) {
    lcd.print("MPU6050 falhou");
    while(1);
  }
}

void loop() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  lcd.clear();
  if (detectPanic(ax, ay, az, gx, gy, gz)) {
    digitalWrite(LED_PANICO, HIGH);
    lcd.print("Crise de Panico!");
    delay(1000); 
    digitalWrite(LED_PANICO, LOW);
  } else if (detectAnxiety(ax, ay, az)) {
    digitalWrite(LED_ANSIEDADE, HIGH);
    lcd.print("Crise de Ansiedade!");
    delay(1000); 
    digitalWrite(LED_ANSIEDADE, LOW);
  } else {
    lcd.print("Monitoramento Ativo");
  }

  delay(100);
}

bool detectAnxiety(int16_t ax, int16_t ay, int16_t az) {
  static int16_t lastAx = ax, lastAy = ay, lastAz = az;
  int16_t deltaX = abs(ax - lastAx);
  int16_t deltaY = abs(ay - lastAy);
  int16_t deltaZ = abs(az - lastAz);

  lastAx = ax;
  lastAy = ay;
  lastAz = az;

  int threshold = 500; // Valor de limiar para ansiedade
  return (deltaX > threshold || deltaY > threshold || deltaZ > threshold);
}

bool detectPanic(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz) {
  return detectSuddenMovements(gx, gy, gz) && detectTremors(ax, ay, az);
}

bool detectSuddenMovements(int16_t gx, int16_t gy, int16_t gz) {
  int threshold = 1000; // Valor de limiar para pânico
  return (abs(gx) > threshold || abs(gy) > threshold || abs(gz) > threshold);
}

bool detectTremors(int16_t ax, int16_t ay, int16_t az) {
  static int16_t lastAx = ax, lastAy = ay, lastAz = az;
  int16_t deltaX = abs(ax - lastAx);
  int16_t deltaY = abs(ay - lastAy);
  int16_t deltaZ = abs(az - lastAz);

  lastAx = ax;
  lastAy = ay;
  lastAz = az;

  int threshold = 1000; // Valor de limiar para pânico
  return (deltaX > threshold || deltaY > threshold || deltaZ > threshold);
}
