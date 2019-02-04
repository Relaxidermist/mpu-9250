#include <Wire.h>

#define mpu9250_address 0x68
#define acc_x_req       0x3B
#define acc_y_req       0x3D
#define acc_z_req       0x3F

#define gyro_config_req 0x1B
#define gyro_config     0b00000000

#define gyro_x_req      0x43
#define gyro_y_req      0x45
#define gyro_z_req      0x47

float Tilt, newTilt, rate, newreading, calreading;
unsigned long t1, t2, period;
bool cal;
byte configbyte;

void setup() {
Wire.begin(); // Join I2C bus as a master.
Serial.begin(115200);
Tilt = 0;
cal = false;
Wire.beginTransmission(mpu9250_address);
Wire.write(gyro_config_req);
Wire.write(gyro_config);
Wire.endTransmission(mpu9250_address);
Wire.requestFrom(mpu9250_address, 1);
if(Wire.available() >0){
  Serial.print("Gyro Config = ");
  configbyte = Wire.read();
  Serial.println(configbyte, BIN);
}

}

void loop() {
  // Accelerometer section
  /*
   Serial.print("X_ACC:  ");
   Serial.print(ReadHiLoBytes(acc_x_req));
   Serial.print(" Y_ACC: ");
   Serial.print(ReadHiLoBytes(acc_y_req));
   Serial.print(" Z_ACC: ");
   Serial.println(ReadHiLoBytes(acc_z_req));
   delay(50);
*/
   //Gyro section
   if(cal==false){
    for(int i=0; i<10000; i++){
      newreading = ReadHiLoBytes(gyro_x_req);
      calreading = calreading + newreading;
      delay(1);
    }
    calreading = calreading/10000;
    delay(500);
    Serial.print("CAL OFFSET = ");
    Serial.println(calreading);
    delay(2000);
    cal = true;
   }
   
   t1 = millis();
   rate = ReadHiLoBytes(gyro_x_req);
   delay(20);
   t2 = millis();
   period = t2-t1;
   newTilt = 131*(rate-calreading)*(0.001*period);
   Tilt = Tilt + newTilt;
   Serial.println(Tilt);
   /*
   Serial.print(" Y_TILT: ");
   Serial.print(ReadHiLoBytes(gyro_y_req));
   Serial.print(" Z_TILT: ");
   Serial.println(ReadHiLoBytes(gyro_z_req));
   */
   //delay(1000);
   
}


// ReadHiLoBytes from the MPU-9250 and return the joined values. Used initallty for accelerometer
float ReadHiLoBytes(int reg){
  
  int HiByte, LoByte;
  float Result;
  
  Wire.beginTransmission(mpu9250_address);
  Wire.write(reg);
  Wire.endTransmission(mpu9250_address);
  Wire.requestFrom(mpu9250_address, 2);
  if(Wire.available()>=2){
    HiByte = Wire.read();
    LoByte = Wire.read();

    HiByte = HiByte << 8;
    Result = (float(HiByte) + float(LoByte))/16384;
  }
  return Result;
}
