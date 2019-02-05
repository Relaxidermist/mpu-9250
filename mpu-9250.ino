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
struct tiltAngle{
  float delta_x;
  float delta_y;
  float delta_z;
};

tiltAngle Tilts, deltaTilts, calTilts;

float xTilt, yTilt, zTilt, xRaw, yRaw, zRaw, xCal, yCal, zCal;
bool cal;
byte configbyte;
int x = 0;
void setup() {
Wire.begin(); // Join I2C bus as a master.
Serial.begin(115200);
xTilt = 0; yTilt = 0; zTilt = 0;
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
    for(int i=0; i<2000; i++){
      calTilts = ReadHiLoBytes(gyro_x_req);
      //calTilts.delta_y = ReadHiLoBytes(gyro_y_req);
      //calTilts.delta_z = ReadHiLoBytes(gyro_z_req);
      
      xCal = xCal + calTilts.delta_x;
      yCal = yCal + calTilts.delta_y;
      zCal = zCal + calTilts.delta_z;
      
      delay(1);
    }
    xCal = xCal/2000;
    yCal = yCal/2000;
    zCal = zCal/2000;
    
    delay(500);
    Serial.print("XCAL OFFSET = ");
    Serial.print(xCal);
    Serial.print("  YCAL = ");
    Serial.print(yCal);
    Serial.print("  ZCAL = ");
    Serial.println(zCal);
    delay(2000);
    cal = true;
   }
deltaTilts = getTilt(gyro_x_req, xCal);  
xTilt = xTilt + deltaTilts.delta_x;
yTilt = yTilt + deltaTilts.delta_y;
zTilt = zTilt + deltaTilts.delta_z;
Serial.print(xTilt);
Serial.print("  ");
Serial.print(yTilt);
Serial.print("  ");
Serial.println(zTilt);

/*
    deltaTilts = getTilts();
    Tilts.delta_x = Tilts.delta_x + deltaTilts.delta_x;
    Serial.println(Tilts.delta_x);

  // delay(50);
  */
}


// ReadHiLoBytes from the MPU-9250 and return the joined values. Used initallty for accelerometer
tiltAngle ReadHiLoBytes(int reg){
  
  int HiBytex, LoBytex, HiBytey, LoBytey, HiBytez, LoBytez;
  float Resultx, Resulty;
  tiltAngle deltaTilts;
  
  Wire.beginTransmission(mpu9250_address);
  Wire.write(reg);
  Wire.endTransmission(mpu9250_address);
  Wire.requestFrom(mpu9250_address, 6);
  if(Wire.available()>=6){
    HiBytex = Wire.read();
    LoBytex = Wire.read();
    HiBytey = Wire.read();
    LoBytey = Wire.read();
    HiBytez = Wire.read();
    LoBytez = Wire.read();
    HiBytex = HiBytex << 8;
    HiBytey = HiBytey << 8;
    HiBytez = HiBytez << 8;
    deltaTilts.delta_x = (float(HiBytex) + float(LoBytex))/16384;
    deltaTilts.delta_y = (float(HiBytey) + float(LoBytey))/16384;
    deltaTilts.delta_z = (float(HiBytez) + float(LoBytez))/16384;
    
  }
  return deltaTilts;

  
}
// Input register for gyroscope axis
tiltAngle getTilt(int reg, float cal){
  int t1, t2, delta_t;
  float ratex, ratey, ratez;
  tiltAngle deltaTilt;

   t1 = millis();
   deltaTilt = ReadHiLoBytes(reg);
   delay(20);
   t2 = millis();
   delta_t = t2-t1;
   deltaTilt.delta_x = 131*(deltaTilt.delta_x-xCal)*(0.001*delta_t);
   deltaTilt.delta_y = 131*(deltaTilt.delta_y-yCal)*(0.001*delta_t);
   deltaTilt.delta_z = 131*(deltaTilt.delta_z-zCal)*(0.001*delta_t);
   
   return deltaTilt;
}
/*
tiltAngle getTilts(){
  byte HiByte_x, LoByte_x, HiByte_y, LoByte_y, HiByte_z, LoByte_z;
  float Res_x, Res_y, Res_z;
  int t1, t2, delta_t;
  t1 = millis();
  /*
  Wire.beginTransmission(mpu9250_address);
  Wire.write(gyro_x_req);
  Wire.endTransmission(mpu9250_address);
  Wire.requestFrom(mpu9250_address, 2);
  if(Wire.available()>=2){
    
    HiByte_x = Wire.read();
    LoByte_x = Wire.read();
    //HiByte_y = Wire.read();
   // LoByte_y = Wire.read();
   // HiByte_z = Wire.read();
   // LoByte_z = Wire.read();

    HiByte_x = HiByte_x << 8;
   // HiByte_y = HiByte_y << 8;
   // HiByte_z = HiByte_z << 8;

    Res_x = (float(HiByte_x) + float(LoByte_x))/16384;
   // Res_y = (float(HiByte_y) + float(LoByte_y))/16384;
   // Res_z = (float(HiByte_z) + float(LoByte_z))/16384;
    delay(10);
    */

    /*

    Res_x = ReadHiLoBytes(gyro_x_req);
    t2 = millis();

    delta_t = t2 - t1;
    
    Res_x = 131*(Res_x-xCal)*(0.001*delta_t);
   // Res_y = 131*(Res_y-yCal)*(0.001*delta_t);
   // Res_z = 131*(Res_z-zCal)*(0.001*delta_t);
  return tiltAngle{Res_x, Res_y, Res_z};
}
*/
