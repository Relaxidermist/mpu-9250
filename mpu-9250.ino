#include <Wire.h>
#include <math.h>

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
int asa_x, asa_y, asa_z;

unsigned long tstart, tfin;
float dt;

void setup() {
xTilt = 0; yTilt = 0; zTilt = 0;  
Wire.begin(); // Join I2C bus as a master.
delay(100);

tstart = 0;
tfin = 0;
dt = 0;

Serial.begin(115200);

config_mpu9250();
cal = false;

}

void loop() {
    
       //Gyro section
    if(cal==false){
       cal_offsets = calGyro();
       cal = true;
    }
    //reset gyro here
    if(accelerations.x < 0.05 and accelerations.y < 0.05 and accelerations.z > 0.97){
      xTilt = 0;
      yTilt = 0;
      zTilt = 0;  
    }
    // Accelerometer section
    accelerations = ReadHiLoBytes(acc_x_req);

    // debugging timer

    tstart = millis();

        
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
    Serial.print(accelerations.z);

    Serial.print(" dt = ");
    Serial.println(dt);

    delay(10);
    tfin = millis();
    
    dt = tfin-tstart;

}


// ReadHiLoBytes from the MPU-9250 and return the joined values. Used initallty for accelerometer
vectors calGyro(){
  vectors deltaTilts, calTilts;
  
      calTilts.x = 0; calTilts.y = 0;calTilts.z = 0;
      deltaTilts.x = 0; deltaTilts.y = 0; deltaTilts.z = 0;
      
  for(int i=0; i<1000; i++){
      deltaTilts = ReadHiLoBytes(gyro_x_req);

      calTilts.x += deltaTilts.x;
      calTilts.y += deltaTilts.y;
      calTilts.z += deltaTilts.z;

      
      Serial.println("Calibrating...");
      
    }
    
    calTilts.x = calTilts.x/1000;
    calTilts.y = calTilts.y/1000;
    calTilts.z = calTilts.z/1000;

    //delay(5000);

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

   //t1 = millis();
   deltaTilt = ReadHiLoBytes(reg);
   //delay(20);
   //t2 = millis();
   delta_t = t2-t1;
   /*
   deltaTilt.x = 131*(deltaTilt.x-calTilts.x)*(0.001*delta_t);
   deltaTilt.y = 131*(deltaTilt.y-calTilts.y)*(0.001*delta_t);
   deltaTilt.z = 131*(deltaTilt.z-calTilts.z)*(0.001*delta_t);
   */
   deltaTilt.x = 131*(deltaTilt.x-calTilts.x)*(0.001*dt);
   deltaTilt.y = 131*(deltaTilt.y-calTilts.y)*(0.001*dt);
   deltaTilt.z = 131*(deltaTilt.z-calTilts.z)*(0.001*dt);
   
   return deltaTilt;
}


// Magnetometer Code to go here.

vectors magnetometer(){
  vectors magnetic;
  
  int HiBytex, LoBytex, HiBytey, LoBytey, HiBytez, LoBytez, stat_2;
  
  Wire.beginTransmission(0x0C);
  Wire.write(0x02);
  Wire.endTransmission();
  Wire.requestFrom(0x0C, 1);
  if(Wire.available()>=1){
    if(Wire.read()>=1){
      Wire.beginTransmission(0x0C);
      Wire.write(0x03);
      Wire.endTransmission();
      Wire.requestFrom(0x0C,7);
      if(Wire.available()>=7){
        LoBytex = Wire.read();
        HiBytex = Wire.read();
        LoBytey = Wire.read();
        HiBytey = Wire.read();
        LoBytez = Wire.read();
        HiBytez = Wire.read();
        stat_2 = Wire.read();

        HiBytex = HiBytex << 8;
        HiBytey = HiBytey << 8;
        HiBytez = HiBytez << 8;

        magnetic.x = HiBytex + LoBytex;
        magnetic.y = HiBytey + LoBytey;
        magnetic.z = HiBytez + LoBytez;
        
        Serial.print("MAG_X: ");
        Serial.print(0.6*magnetic.x);
        Serial.print("  MAG_Y:  ");
        Serial.print(0.6*magnetic.y);
        Serial.print("  MAG_Z:  ");
        Serial.print(0.6*magnetic.z);
        Serial.print("  Stat_2: ");
        Serial.print(stat_2);
        Serial.println("  ANGLE:  ");
        Serial.println((180/3.14159)*atan2((magnetic.y),(magnetic.x)));
        
      }
    }
  }
  else{
    Serial.println("No Bytes");
  }
  return magnetic;
}

void config_mpu9250(){

  Wire.beginTransmission(0x0C);
  Wire.write(0x0A);
  Wire.write(0x16);
  Wire.endTransmission();
  delay(100);
  Wire.beginTransmission(0x0C);
  Wire.write(0x10);
  Wire.endTransmission();
  Wire.requestFrom(0x0C, 3);
  if(Wire.available()>=3){
    asa_x = Wire.read();
    asa_y = Wire.read();
    asa_z = Wire.read();

    Serial.print("ASA_X:  ");
    Serial.print(asa_x);
    Serial.print("  ASA_Y:  ");
    Serial.print(asa_y);
    Serial.print("  ASA_Z:  ");
    Serial.println(asa_z);
    delay(1000);
  }
  else{
    Serial.println("No Bytes");
    delay(1000);
  }
}
