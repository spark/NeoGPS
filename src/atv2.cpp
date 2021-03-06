#include <atv2.h>
#include "GPSport.h"
#include "Streamers.h"

//------------------------------------------------------------
// This object parses received characters
//   into the gps.fix() data structure

static NMEAGPS  gps;

//------------------------------------------------------------
//  Define a set of GPS fix information.  It will
//  hold on to the various pieces as they are received from
//  an RMC sentence.  It can be used anywhere in your sketch.

static gps_fix  fix_data;

Adafruit_LIS3DH accel = Adafruit_LIS3DH(A2);

AssetTracker::AssetTracker(){

}

void AssetTracker::begin(){
  	accel.begin(LIS3DH_DEFAULT_ADDRESS);
    
    // Default to 5kHz low-power sampling
    accel.setDataRate(LIS3DH_DATARATE_LOWPOWER_5KHZ);
    
    // Default to 4 gravities range
    accel.setRange(LIS3DH_RANGE_4_G);

}

float AssetTracker::readLat(){
	
	while (gps.available( gps_port )) {
    fix_data = gps.read();
}
	return fix_data.latitude();
	//else return 0;

}

float AssetTracker::readLon(){

	while (gps.available( gps_port )) {
    fix_data = gps.read();
}
	return fix_data.longitude();
	//else return 0;

}

float AssetTracker::readLatDeg(){
	if(fix_data.valid.location) return fix_data.latitude();
	else return 0;

}

float AssetTracker::readLonDeg(){
	if(fix_data.valid.location) return fix_data.longitude();
	else return 0;

}

float AssetTracker::readHDOP(){
	//if(fix_data.valid.hdop) return fix_data.hdop;
	//else return 0;

}

float AssetTracker::getGpsAccuracy(){

}

uint32_t AssetTracker::getGpsTimestamp(){

	while (gps.available( gps_port )) {
    fix_data = gps.read();
}
	return fix_data.dateTime;

}

String AssetTracker::readLatLon(){

	if(fix_data.valid.location)
	{
		String latLon = String::format("%f,%f",fix_data.latitudeL(),fix_data.longitudeL());
    	return latLon;
	}
	return 0;
}

void AssetTracker::gpsOn(){
    // Power to the GPS is controlled by a FET connected to D6
    pinMode(D6,OUTPUT);
    digitalWrite(D6,LOW);
    delay(100);
    // Start the UART for the GPS device
  	gps_port.begin( 9600 );
  	delay(100);
}

void AssetTracker::gpsOff(){
    digitalWrite(D6,HIGH);
}

char* AssetTracker::preNMEA(){

}

bool AssetTracker::gpsFix(){

	if(fix_data.valid.location) return 1;
	else return 0;
}


void AssetTracker::updateGPS(){
	while (gps.available( gps_port )) {
    fix_data = gps.read();
  }

}

int AssetTracker::readX(){
    accel.read();
    return accel.x;
}

int AssetTracker::readY(){
    accel.read();
    return accel.y;
}

int AssetTracker::readZ(){
    accel.read();
    return accel.z;
}

int AssetTracker::readXYZmagnitude(){
    accel.read();
    int magnitude = sqrt((accel.x*accel.x)+(accel.y*accel.y)+(accel.z*accel.z));
    return magnitude;
}

bool AssetTracker::setupLowPowerWakeMode(uint8_t movementThreshold) {
	return accel.setupLowPowerWakeMode(movementThreshold);
}

uint8_t AssetTracker::clearAccelInterrupt() {
	return accel.clearInterrupt();
}

/**************************************************************************/
/*!
    @file     Adafruit_LIS3DH.cpp
    @author   K. Townsend / Limor Fried (Adafruit Industries)
    @license  BSD (see license.txt)

    This is a library for the Adafruit LIS3DH Accel breakout board
    ----> https://www.adafruit.com/products/????

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section  HISTORY

    v1.0  - First release
*/
/**************************************************************************/

 //#include "application.h"


//#include <Wire.h>
//#include "Adafruit_LIS3DH.h"


/**************************************************************************/
/*!
    @brief  Instantiates a new LIS3DH class in I2C or SPI mode
*/
/**************************************************************************/
// I2C
Adafruit_LIS3DH::Adafruit_LIS3DH() 
  : _cs(-1), _mosi(-1), _miso(-1), _sck(-1), _sensorID(-1)
{
}

Adafruit_LIS3DH::Adafruit_LIS3DH(int8_t cspin) 
  : _cs(cspin), _mosi(-1), _miso(-1), _sck(-1), _sensorID(-1) 
{ }

Adafruit_LIS3DH::Adafruit_LIS3DH(int8_t cspin, int8_t mosipin, int8_t misopin, int8_t sckpin) 
  : _cs(cspin), _mosi(mosipin), _miso(misopin), _sck(sckpin), _sensorID(-1) 
{ }



/**************************************************************************/
/*!
    @brief  Setups the HW (reads coefficients values, etc.)
*/
/**************************************************************************/
bool Adafruit_LIS3DH::begin(uint8_t i2caddr) {
  _i2caddr = i2caddr;
  

  if (_cs == -1) {
    // i2c
    Wire.begin();
  } else {
    digitalWrite(_cs, HIGH);
    pinMode(_cs, OUTPUT);

    if (_sck == -1) {
      // hardware SPI
      SPI.begin();
    } else {
      // software SPI
      pinMode(_sck, OUTPUT);
      pinMode(_mosi, OUTPUT);
      pinMode(_miso, INPUT);
    }
  }

  /* Check connection */
  uint8_t deviceid = readRegister8(LIS3DH_REG_WHOAMI);
  if (deviceid != 0x33)
  {
    /* No LIS3DH detected ... return false */
    //Serial.println(deviceid, HEX);
    return false;
  }

  // enable all axes, normal mode
  writeRegister8(LIS3DH_REG_CTRL1, 0x07);
  // 400Hz rate
  setDataRate(LIS3DH_DATARATE_400_HZ);

  // High res enabled
  writeRegister8(LIS3DH_REG_CTRL4, 0x08);

  // DRDY on INT1
  writeRegister8(LIS3DH_REG_CTRL3, 0x10);

  // Turn on orientation config
  //writeRegister8(LIS3DH_REG_PL_CFG, 0x40);

  // enable adc & temp sensor
  writeRegister8(LIS3DH_REG_TEMPCFG, 0xC0);
  
  
  for (uint8_t i=0; i<0x30; i++) {
    Serial.print("$");
    Serial.print(i, HEX); Serial.print(" = 0x");
    Serial.println(readRegister8(i), HEX);
  }
  

  return true;
}


void Adafruit_LIS3DH::read(void) {
  // read x y z at once

  if (_cs == -1) {
    // i2c
    Wire.beginTransmission(_i2caddr);
    Wire.write(LIS3DH_REG_OUT_X_L | 0x80); // 0x80 for autoincrement
    Wire.endTransmission();
    
    Wire.requestFrom(_i2caddr, 6);
    x = Wire.read(); x |= ((uint16_t)Wire.read()) << 8;
    y = Wire.read(); y |= ((uint16_t)Wire.read()) << 8;
    z = Wire.read(); z |= ((uint16_t)Wire.read()) << 8;
  } else {
    if (_sck == -1)
      beginTransaction();

    digitalWrite(_cs, LOW);
    spixfer(LIS3DH_REG_OUT_X_L | 0x80 | 0x40); // read multiple, bit 7&6 high

    x = spixfer(); x |= ((uint16_t)spixfer()) << 8;
    y = spixfer(); y |= ((uint16_t)spixfer()) << 8;
    z = spixfer(); z |= ((uint16_t)spixfer()) << 8;

    digitalWrite(_cs, HIGH);
    if (_sck == -1)
    	endTransaction();

  }
  uint8_t range = getRange();
  uint16_t divider = 1;
  if (range == LIS3DH_RANGE_16_G) divider = 2048;
  if (range == LIS3DH_RANGE_8_G) divider = 4096;
  if (range == LIS3DH_RANGE_4_G) divider = 8190;
  if (range == LIS3DH_RANGE_2_G) divider = 16380;

  x_g = (float)x / divider;
  y_g = (float)y / divider;
  z_g = (float)z / divider;

}

/**************************************************************************/
/*!
    @brief  Read the auxilary ADC
*/
/**************************************************************************/

uint16_t Adafruit_LIS3DH::readADC(uint8_t adc) {
  if ((adc < 1) || (adc > 3)) return 0;
  uint16_t value;

  adc--;

  uint8_t reg = LIS3DH_REG_OUTADC1_L + adc*2;


  if (_cs == -1) {
    // i2c
    Wire.beginTransmission(_i2caddr);
    Wire.write(reg | 0x80);   // 0x80 for autoincrement
    Wire.endTransmission();    
    Wire.requestFrom(_i2caddr, 2);
    value = Wire.read();  value |= ((uint16_t)Wire.read()) << 8;
  } else {
	if (_sck == -1)
	  beginTransaction();

	digitalWrite(_cs, LOW);
    spixfer(reg | 0x80 | 0x40); // read multiple, bit 7&6 high

    value = spixfer(); value |= ((uint16_t)spixfer()) << 8;

    digitalWrite(_cs, HIGH);
    if (_sck == -1)
    	endTransaction();
  }

  return value;
}

/**************************************************************************/
/*!
    @brief  Sets the g range for the accelerometer
*/
/**************************************************************************/
void Adafruit_LIS3DH::setRange(lis3dh_range_t range)
{
  uint8_t r = readRegister8(LIS3DH_REG_CTRL4);
  r &= ~(0x30);
  r |= range << 4;
  writeRegister8(LIS3DH_REG_CTRL4, r);
}

/**************************************************************************/
/*!
    @brief  Sets the g range for the accelerometer
*/
/**************************************************************************/
lis3dh_range_t Adafruit_LIS3DH::getRange(void)
{
  /* Read the data format register to preserve bits */
  return (lis3dh_range_t)((readRegister8(LIS3DH_REG_CTRL4) >> 4) & 0x03);
}

/**************************************************************************/
/*!
    @brief  Sets the data rate for the LIS3DH (controls power consumption)
*/
/**************************************************************************/
void Adafruit_LIS3DH::setDataRate(lis3dh_dataRate_t dataRate)
{
  uint8_t ctl1 = readRegister8(LIS3DH_REG_CTRL1);
  ctl1 &= ~(0xF0); // mask off bits
  ctl1 |= (dataRate << 4);
  writeRegister8(LIS3DH_REG_CTRL1, ctl1);
}

/**************************************************************************/
/*!
    @brief  Sets the data rate for the LIS3DH (controls power consumption)
*/
/**************************************************************************/
lis3dh_dataRate_t Adafruit_LIS3DH::getDataRate(void)
{
  return (lis3dh_dataRate_t)((readRegister8(LIS3DH_REG_CTRL1) >> 4)& 0x0F);
}

/**************************************************************************/
/*! 
    @brief  Gets the most recent sensor event
*/
/**************************************************************************/
bool Adafruit_LIS3DH::getEvent(sensors_event_t *event) {
  /* Clear the event */
  memset(event, 0, sizeof(sensors_event_t));

  event->version   = sizeof(sensors_event_t);
  event->sensor_id = _sensorID;
  event->type      = SENSOR_TYPE_ACCELEROMETER;
  event->timestamp = 0;

  read();

  event->acceleration.x = x_g;
  event->acceleration.y = y_g;
  event->acceleration.z = z_g;
}

/**************************************************************************/
/*! 
    @brief  Gets the sensor_t data
*/
/**************************************************************************/
void Adafruit_LIS3DH::getSensor(sensor_t *sensor) {
  /* Clear the sensor_t object */
  memset(sensor, 0, sizeof(sensor_t));

  /* Insert the sensor name in the fixed length char array */
  strncpy (sensor->name, "LIS3DH", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name)- 1] = 0;
  sensor->version     = 1;
  sensor->sensor_id   = _sensorID;
  sensor->type        = SENSOR_TYPE_ACCELEROMETER;
  sensor->min_delay   = 0;
  sensor->max_value   = 0;             
  sensor->min_value   = 0;
  sensor->resolution  = 0;             
}

/**************************************************************************/
/*!
    @brief  Enable wake-on-move mode

    The movementThreshold value default is 16. Lower values are more sensitive.
*/
/**************************************************************************/

bool Adafruit_LIS3DH::setupLowPowerWakeMode(uint8_t movementThreshold) {

	// Enable 10 Hz, low power, with XYZ detection enabled
	writeRegister8(LIS3DH_REG_CTRL1, LIS3DH_CTRL_REG1_ODR1 | LIS3DH_CTRL_REG1_LPEN | LIS3DH_CTRL_REG1_ZEN | LIS3DH_CTRL_REG1_YEN | LIS3DH_CTRL_REG1_XEN);

	// High pass filters disabled
	// Enable reference mode LIS3DH_CTRL_REG2_HPM0 | LIS3DH_CTRL_REG2_HPIS1
	// Tried enabling CTRL_REG2_HPM0 | CTRL_REG2_HPM1 for auto-reset, did not seem to help
	writeRegister8(LIS3DH_REG_CTRL2, 0);

	// Enable INT1
	writeRegister8(LIS3DH_REG_CTRL3, LIS3DH_CTRL_REG3_I1_INT1);

	// Disable high resolution mode
	writeRegister8(LIS3DH_REG_CTRL4, 0);

	// Page 12 of the app note says to do this last, but page 25 says to do them in order.
	// Disable FIFO, enable latch interrupt on INT1_SRC
	writeRegister8(LIS3DH_REG_CTRL5, LIS3DH_CTRL_REG5_LIR_INT1);

	// CTRL_REG6_H_LACTIVE means active low, not needed here
	writeRegister8(LIS3DH_REG_CTRL6, 0);

	// In normal mode, reading the reference register sets it for the current normal force
	// (the normal force of gravity acting on the device)
	readRegister8(LIS3DH_REG_REFERENCE);

	// 250 mg threshold = 16
	writeRegister8(LIS3DH_REG_INT1THS, movementThreshold);

	//
	writeRegister8(LIS3DH_REG_INT1DUR, 0);


	if (intPin >= 0) {
		// There are instructions to set the INT1_CFG in a loop in the appnote on page 24. As far
		// as I can tell this never works. Merely setting the INT1_CFG does not ever generate an
		// interrupt for me.

		// Remember the INT1_CFG setting because we're apparently supposed to set it again after
		// clearing an interrupt.
		int1_cfg = LIS3DH_INT1_CFG_YHIE_YUPE | LIS3DH_INT1_CFG_XHIE_XUPE;
		writeRegister8(LIS3DH_REG_INT1CFG, int1_cfg);

		// Clear the interrupt just in case
		readRegister8(LIS3DH_REG_INT1SRC);
	}
	else {
		int1_cfg = 0;
		writeRegister8(LIS3DH_REG_INT1CFG, 0);
	}

	return true;
}

/**************************************************************************/
/*!
    @brief  When using setupLowPowerWakeMode to sleep until moved, this clears
    the interrupt on the WKP pin. Manual reset mode is used so you can tell the
    difference between timeout and movement wakeup when using stop mode sleep.
*/
/**************************************************************************/

uint8_t Adafruit_LIS3DH::clearInterrupt() {
	uint8_t int1_src = readRegister8(LIS3DH_REG_INT1SRC);

	if (intPin >= 0) {
		while(digitalRead(intPin) == HIGH) {
			delay(10);
			readRegister8(LIS3DH_REG_INT1SRC);
			writeRegister8(LIS3DH_REG_INT1CFG, int1_cfg);
		}
	}

	return int1_src;
}


/**************************************************************************/
/*! 
    @brief  Low level SPI
*/
/**************************************************************************/

uint8_t Adafruit_LIS3DH::spixfer(uint8_t x) {
  if (_sck == -1) 
    return SPI.transfer(x);
  
  // software spi
  //Serial.println("Software SPI");
  uint8_t reply = 0;
  for (int i=7; i>=0; i--) {
    reply <<= 1;
    digitalWrite(_sck, LOW);
    digitalWrite(_mosi, x & (1<<i));
    digitalWrite(_sck, HIGH);
    if (digitalRead(_miso)) 
      reply |= 1;
  }
  return reply;
}


/**************************************************************************/
/*!
    @brief  Writes 8-bits to the specified destination register
*/
/**************************************************************************/
void Adafruit_LIS3DH::writeRegister8(uint8_t reg, uint8_t value) {
  if (_cs == -1) {
    Wire.beginTransmission((uint8_t)_i2caddr);
    Wire.write((uint8_t)reg);
    Wire.write((uint8_t)value);
    Wire.endTransmission();
  } else {
	if (_sck == -1)
	  beginTransaction();

    digitalWrite(_cs, LOW);
    spixfer(reg & ~0x80); // write, bit 7 low
    spixfer(value);
    digitalWrite(_cs, HIGH);
    if (_sck == -1)
    	endTransaction();
  }
}

/**************************************************************************/
/*!
    @brief  Reads 8-bits from the specified register
*/
/**************************************************************************/
uint8_t Adafruit_LIS3DH::readRegister8(uint8_t reg) {
  uint8_t value;
  
  if (_cs == -1) {
    Wire.beginTransmission(_i2caddr);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();

    Wire.requestFrom(_i2caddr, 1);
    value = Wire.read();
  }  else {
	if (_sck == -1)
	  beginTransaction();

	digitalWrite(_cs, LOW);
    spixfer(reg | 0x80); // read, bit 7 high
    value = spixfer(0);
    digitalWrite(_cs, HIGH);
    if (_sck == -1)
    	endTransaction();
  }
  return value;
}


void Adafruit_LIS3DH::beginTransaction() {
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockSpeed(500000);
    SPI.setDataMode(SPI_MODE0);
}

void Adafruit_LIS3DH::endTransaction() {
}