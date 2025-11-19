#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
int led=4;
int alert=0;
int it=0;
Adafruit_MPU6050 mpu;
int force, x, y, z,X,Y,Z;
RF24 radio(7, 8); // CE, CSN pins
int flag = 0;
const byte address[6] = "00001";
const int ledPin = 13; // Pin where the LED is connected

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT); // Set the LED pin as output
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  Serial.println("Adafruit MPU6050 test!");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
}

void loop() {
    radio.openReadingPipe(0, address);
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.print("Received: ");
    Serial.println(text);

    if (strcmp(text, "activate") == 0) {
      flag = 1; // Activate the MPU sensor readings
      digitalWrite(ledPin, HIGH); // Turn on the LED
      Serial.println("System activated");
      delay(500);
      transmit();
      blinkactivate();
      it=0;
    } else if (strcmp(text, "deactivate") == 0) {
      flag = 0; // Deactivate the MPU sensor readings
      digitalWrite(ledPin, LOW); // Turn off the LED
      Serial.println("System deactivated");
      delay(500);
      transmit_();
      blinkactivate();
      alert=0;
      
    }
  }

  if (flag == 1) {
    mpu_();
   if(it==0)
   {
    X=x;
    Y=y;
    Z=z;
    it++;
   }
      
  }
}

void mpu_() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");

  x = abs(a.acceleration.x);
  y = abs(a.acceleration.y);
  z = abs(a.acceleration.z);

  if ((x > 3+X ||y > 3+Y || z > 3+Z )&&it==1) {
    Serial.print("Impact detected: ");
    force = abs(x) + abs(y) + abs(z);
    Serial.println(force);
    transmitImpact();
    alert=1;
  }

if (alert==1)
{
  blinkimpact();
}

  Serial.println("");
  delay(500);
}

void transmitImpact() {
  const char text[] = "impact";
  radio.stopListening(); // Stop listening to be able to transmit
  radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MIN);

  radio.write(&text, sizeof(text));
  Serial.println("Impact data transmitted");
  delay(500); // Ensure the message is sent
  radio.startListening(); // Start listening again
}

void transmit() {
  const char text[] = "activate";
  radio.stopListening(); // Stop listening to be able to transmit
  radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MIN);

  radio.write(&text, sizeof(text));
  delay(500); // Ensure the message is sent
  radio.startListening(); // Start listening again
}

void transmit_() {
  const char text[] = "deactivate";
  radio.stopListening(); // Stop listening to be able to transmit
  radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MIN);

  radio.write(&text, sizeof(text));
  delay(500); // Ensure the message is sent
  radio.startListening(); // Start listening again
}

void blinkactivate()
{
  digitalWrite(led,HIGH);
  delay(300);
  digitalWrite(led,LOW);
}

void blinkimpact()
{
  digitalWrite(led,HIGH);
  delay(100);
  digitalWrite(led,LOW);
  delay(100);
}
