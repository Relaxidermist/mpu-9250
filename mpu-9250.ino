#include <Wire.h>

#define mpu9250_address 0x68
#define acc_x_req       0x3B
#define acc_y_req       0x3D
#define acc_z_req       0x3F

int acc_xout_h, acc_xout_l, acc_yout_h, acc_yout_l, acc_zout_h, acc_zout_l;
float acc_xout, acc_yout, acc_zout;

void setup() {
Wire.begin(); // Join I2C bus as a master.
Serial.begin(115200);
}

void loop() {
   Serial.print("XOUT:  ");
   Serial.print(ReadHiLoBytes(acc_x_req));
   Serial.print(" YOUT: ");
   Serial.print(ReadHiLoBytes(acc_y_req));
   Serial.print(" ZOUT: ");
   Serial.println(ReadHiLoBytes(acc_z_req));
   delay(50);
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
