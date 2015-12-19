/* 
dgps_logging_v1 provides API for the DExter Industries dGPS snesor 

Changelog:
0.1: Initial release

Credits:
Developed based on information and comments from Dexter Industries 
and from C code provided by Xander Soldaat (xander_at_botbench.com)
in files common.h and example dexterind-test1.c

License:
This code can be used freely, provided credit is given where due

\author:  John Harrison
\date:    August 10 2014
\version:  0.1 
*/

#include <Wire.h>

#define DGPS_I2C_ADDR   0x03      /* dGPS sensor device address */
#define DGPS_CMD_UTC    0x00      /* Fetch UTC */
#define DGPS_CMD_STATUS 0x01      /* Status of satellite link: 0 no link, 1 link */
#define DGPS_CMD_LAT    0x02      /* Fetch Latitude */
#define DGPS_CMD_LONG   0x04      /* Fetch Longitude */
#define DGPS_CMD_VELO   0x06      /* Fetch velocity in cm/s */
#define DGPS_CMD_HEAD   0x07      /* Fetch heading in degrees */
#define DGPS_CMD_DIST   0x08      /* Fetch distance to destination */
#define DGPS_CMD_ANGD   0x09      /* Fetch angle to destination */
#define DGPS_CMD_ANGR   0x0A      /* Fetch angle travelled since last request */
#define DGPS_CMD_SLAT   0x0B      /* Set latitude of destination */
#define DGPS_CMD_SLONG  0x0C      /* Set longitude of destination */
#define AREF_VOLTAGE 5            /* aref_voltage set to 5V */

boolean result = false;
byte DGPS_I2CRequest[3];
//byte DGPS_I2CReply[7];
long UTCtime;
long DGPSLat;
long DGPSLong;
long DGPSVelo;
long DGPSHead;
int gsrPin = 0;  //set the analog data in to pin A0
int raw = 0;    //raw will be raw gsr reading
float voltage = 0;

void setup()
{
    Serial.begin(115200); // start serial for output
    Wire.begin();
    delay(15);
//    Serial.println("Ready!");
//    result = DGPSreadStatus();
//    if (result = true) {
//      Serial.println("Got valid signal");  
//    }
//    else(Serial.println("No valid signal"));
}

void loop()
{
  UTCtime = DGPSreadUTC();
  DGPSLat = DGPSreadLatitude();
  DGPSLong = DGPSreadLongitude();
  DGPSVelo = DGPSreadVelocity();
  DGPSHead = DGPSreadHeading();
  raw = readGSR(gsrPin);
  voltage = getVoltage(raw);
  Serial.print(UTCtime);
  Serial.print(",");
  Serial.print(DGPSLat);
  Serial.print(",");
  Serial.print(DGPSLong);
  Serial.print(",");
  Serial.print(DGPSVelo);
  Serial.print(",");
  Serial.print(DGPSHead);
  Serial.print(",");
  Serial.print(raw);
  Serial.print(",");
  Serial.print(voltage);
  
  delay(1000);
}

long DGPSreadRegister(byte msgsize, byte command, int replysize)
{
  DGPS_I2CRequest[0] = msgsize;    //message size
  DGPS_I2CRequest[1] = DGPS_I2C_ADDR;  //I2C address
  DGPS_I2CRequest[2] = command;
  byte DGPS_I2CReply[replysize];
  
  Wire.beginTransmission(DGPS_I2C_ADDR);
  delay(5);
  Wire.write(msgsize);
  Wire.write(DGPS_I2C_ADDR);
  Wire.write(command);
  Wire.endTransmission();
  delay(15);
  Wire.requestFrom(DGPS_I2C_ADDR, replysize);
  while(!Wire.available()) {
  }
  for (int n = 0; n < replysize; n++) {
    DGPS_I2CReply[n] = Wire.read(); 
  }
  
  if (replysize == 4) {
    return ((long)DGPS_I2CReply[3] + ((long)DGPS_I2CReply[2] << 8) + ((long)DGPS_I2CReply[1] << 16) + ((long)DGPS_I2CReply[0] << 24));
  }
  
  else if (replysize == 3)
    return (long)DGPS_I2CReply[2] + ((long)DGPS_I2CReply[1] << 8) + ((long)DGPS_I2CReply[0] << 16);
  
  else if (replysize == 2)
    return (long)DGPS_I2CReply[1] + ((long)DGPS_I2CReply[0] << 8);
  
  else if (replysize == 1)
    return (long)DGPS_I2CReply[0];
    
  return 0;  
}

/**
 * Read the status returned by the GPS.
 * @param message size is 2 and reply size is 1
 * @return ture if valid signal else return false for no valid signal
 */
boolean DGPSreadStatus()
{
  return (DGPSreadRegister(0x02, DGPS_CMD_STATUS, 1) == 1) ? true : false;
  
}

/**
 * Read the time returned by the GPS in UTC.
 * @param message size is 0x00 and reply size is 4
 * @return the time in UTC as long (hhmmss)
 */
long DGPSreadUTC() {
  return DGPSreadRegister(0x00, DGPS_CMD_UTC, 4);
}

/**
 * Read the current location's latitude in decimal degree format
 * @param message size is 0x02 and reply size is 4
 * @return current latitude as long Positive = North, negative = South
 */
long DGPSreadLatitude() {
  return DGPSreadRegister(0x02, DGPS_CMD_LAT, 4);
}


/**
 * Read the current location's longitude in decimal degree format
 * @param message size is 0x04 and reply size is 4
 * @return current longitude as long Positive = East, negative = West
 */
long DGPSreadLongitude() {
  return DGPSreadRegister(0x04, DGPS_CMD_LONG, 4);
}

/**
 * Read the current velocity in cm/s
 * @param message size is 0x03 and reply size is 3
 * @return current velocity 
 */
long DGPSreadVelocity() {
  return DGPSreadRegister(0x03, DGPS_CMD_VELO, 3);
}

/**
 * Read the current velocity in cm/s
 * @param message size is 0x02 and reply size is 2
 * @return current velocity 
 */
long DGPSreadHeading() {
  return DGPSreadRegister(0x02, DGPS_CMD_HEAD, 2);
}

int readGSR(int gp) {
  return analogRead(gp);
}

float getVoltage(int r) {
  float v = r * AREF_VOLTAGE;
  return v /= 1023; 
}

