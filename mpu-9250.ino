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

struct vectors{
  float x;
  float y;
  float z;
};

vectors accelerations, deltaTilts, cal_offsets;

float xTilt, yTilt, zTilt;
bool cal;

void setup() {
xTilt = 0; yTilt = 0; zTilt = 0;  
Wire.begin(); // Join I2C bus as a master.
Serial.begin(115200);
cal = false;

}

void loop() {
       //Gyro section
    if(cal==false){
       cal_offsets = calGyro();
       cal = true;
    }  
    // Accelerometer section
    accelerations = ReadHiLoBytes(acc_x_req);
          
    deltaTilts = getTilt(gyro_x_req, cal_offsets);  
    
    xTilt += deltaTilts.x;
    yTilt += deltaTilts.y;
    zTilt += deltaTilts.z;
    
    Serial.print("TILT_X: ");
    Serial.print(xTilt);
    Serial.print(" TILT_Y: ");
    Serial.print(yTilt);
    Serial.print(" TILT_Z: ");
    Serial.print(zTilt);
    Serial.print(" | ");
    Serial.print("ACC_X: ");
    Serial.print(accelerations.x);
    Serial.print(" ACC_Y: ");
    Serial.print(accelerations.y);
    Serial.print(" ACC_Z: ");
    Serial.println(accelerations.z);
}


// ReadHiLoBytes from the MPU-9250 and return the joined values. Used initallty for accelerometer
vectors calGyro(){
  vectors deltaTilts, calTilts;
  
      calTilts.x = 0; calTilts.y = 0;calTilts.z = 0;
      deltaTilts.x = 0; deltaTilts.y = 0; deltaTilts.z = 0;
      
  for(int i=0; i<5000; i++){
      deltaTilts = ReadHiLoBytes(gyro_x_req);

      calTilts.x += deltaTilts.x;
      calTilts.y += deltaTilts.y;
      calTilts.z += deltaTilts.z;
      delay(1);
    }
    
    calTilts.x = calTilts.x/5000;
    calTilts.y = calTilts.y/5000;
    calTilts.z = calTilts.z/5000;

    delay(5000);

    return calTilts;
   }


vectors ReadHiLoBytes(int reg){
  
  int HiBytex, LoBytex, HiBytey, LoBytey, HiBytez, LoBytez;
  vectors values;
  
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
    
    values.x = (float(HiBytex) + float(LoBytex))/16384;
    values.y = (float(HiBytey) + float(LoBytey))/16384;
    values.z = (float(HiBytez) + float(LoBytez))/16384;
  }
  return values;

  
}
// Input register for gyroscope axis
vectors getTilt(int reg, vectors calTilts){
  int t1, t2, delta_t;
  vectors deltaTilt;

   t1 = millis();
   deltaTilt = ReadHiLoBytes(reg);
   delay(20);
   t2 = millis();
   delta_t = t2-t1;
   
   deltaTilt.x = 131*(deltaTilt.x-calTilts.x)*(0.001*delta_t);
   deltaTilt.y = 131*(deltaTilt.y-calTilts.y)*(0.001*delta_t);
   deltaTilt.z = 131*(deltaTilt.z-calTilts.z)*(0.001*delta_t);
   
   return deltaTilt;
}
