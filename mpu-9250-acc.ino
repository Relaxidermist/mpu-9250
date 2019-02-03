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
  // put your main code here, to run repeatedly:

  Wire.beginTransmission(mpu9250_address);
  Wire.write(acc_x_req);
  Wire.endTransmission(mpu9250_address);
  Wire.requestFrom(mpu9250_address, 2);
  if(Wire.available()>=2){

    acc_xout_h = Wire.read();
    acc_xout_l = Wire.read();

    acc_xout_h = acc_xout_h << 8;
    acc_xout = (float(acc_xout_h) + float(acc_xout_l))/16384; // Divide by sensitvity
  }
  
  Wire.beginTransmission(mpu9250_address);
  Wire.write(acc_y_req);
  Wire.endTransmission(mpu9250_address);
  Wire.requestFrom(mpu9250_address, 2);
   if(Wire.available()>=2){

    acc_yout_h = Wire.read();
    acc_yout_l = Wire.read();

    acc_yout_h = acc_yout_h << 8;
    acc_yout = (float(acc_yout_h) + float(acc_yout_l))/16384; // Divide by sensitvity
  }
  
  Wire.beginTransmission(mpu9250_address);
  Wire.write(acc_z_req);
  Wire.endTransmission(mpu9250_address);
  Wire.requestFrom(mpu9250_address, 2);
  
      acc_zout_h= Wire.read();
      acc_zout_l = Wire.read();

      acc_zout_h = acc_zout_h << 8;
      acc_zout = (float(acc_zout_h) + float(acc_zout_l))/16384;
      
   Serial.print("XOUT:  ");
   Serial.print(acc_xout);
   Serial.print(" YOUT: ");
   Serial.print(acc_yout);
   Serial.print(" ZOUT: ");
   Serial.println(acc_zout);
   delay(50);
}
