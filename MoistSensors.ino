
#include <I2CSoilMoistureSensor.h>
#include <Wire.h>

#define NO_OF_SENSORS 3

I2CSoilMoistureSensor sensor[3];

void setup() {
  Wire.begin(); 
  Serial.begin(9600);
//
//  Serial.println("Resetting sensors");
//  for (int i = 0; i < NO_OF_SENSORS; i++) {
//    sensor[i].begin();
//  }
//  delay(1000); // give some time to boot up
//
//  Serial.println();
  
  byte error, address;
  uint16_t device_idx = 0;

  Serial.println("Scanning for devices...");
  for(address = 1; address < 127; address++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address:");
      print_address(address);
      Serial.println("  !");

      if (device_idx > NO_OF_SENSORS) Serial.println("Found more devices than you have specified in the code!");
      else {
        Serial.println("Initialising device at address: ");
        print_address(address);
        sensor[device_idx++].setAddress(address, true);
      }
    }
    else if (error==4) {
      Serial.print("Unknown error at address: ");
      print_address(address);
    }
  }
  delay(1000); // give some time to boot up

  if (device_idx == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.print("Done... set up ");
    Serial.print(device_idx + 1);
    Serial.println(" devices.");
}


void print_address(uint16_t address) {
  Serial.print("0x");
  if (address<16) Serial.print("0");
  Serial.print(address,HEX);
}

void loop() {
  for (int i = 0; i < NO_OF_SENSORS; i++) {
    if (sensor[i].isBusy()) continue;

    Serial.print("Device ID ");
    Serial.print(i);
    Serial.print(" at address ");
    print_address(sensor[i].getAddress());
    Serial.print("\n");
    Serial.print("Soil Moisture Capacitance: ");
    Serial.print(sensor[i].getCapacitance()); //read capacitance register
    Serial.print(", Temperature: ");
    Serial.print(sensor[i].getTemperature()/(float)10); //temperature register
    Serial.print(", Light: ");
    Serial.println(sensor[i].getLight(true)); //request light measurement, wait and read light register    
    sensor[i].sleep();
  }
}
