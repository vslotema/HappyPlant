/****************************************************************************
 *
 ****************************************************************************/
#include <I2CSoilMoistureSensor.h>
#include <Wire.h>
#include <string.h>

/** Defines a struct holding information about plant sensor and motor */
typedef struct plant_info {
  uint8_t id;
  uint8_t motor_step_pin;
  uint8_t motor_dir_pin;
  I2CSoilMoistureSensor sensor;
} plant_info;
/** Number of plants this setup works for */
#define NO_OF_PLANTS 3
/** Array of plant information */
plant_info plant[NO_OF_PLANTS];

/** Water plant if this threshold surpassed */
#define CAPACITANCE_THRESHOLD 400
/** Water for this many stepper motor pulses */
#define WATERING_PULSES 800

///////////////
// PIN SETUP //
///////////////
/* Water levle indicating LEDs */
#define WATER_LVL_0_LED_PIN  A8
#define WATER_LVL_1_LED_PIN  A9
#define WATER_LVL_2_LED_PIN A10
#define WATER_LVL_3_LED_PIN A11
#define WATER_LVL_4_LED_PIN A12

/** The pin to trigger sending ultrasound */
#define ULTRASOUND_TRIGGERING_PIN 8
/** The pin receive echo signal indication */
#define ULTRASOUND_LISTENING_PIN  9

// Print a short hex address (I2C address) to serial console
void print_address(uint16_t address, bool newline) {
  Serial.print("0x");
  if (address<16) Serial.print("0");
  Serial.print(address,HEX);
  if (newline) Serial.println();
}

// Print a short hex address (I2C address) to serial console
void output_plant_info(plant_info *plant) {
    Serial.print("Device ID ");
    Serial.print(plant->id);
    Serial.print(" (at I2C address ");
    print_address(plant->sensor.getAddress(), true);
    Serial.print("):");
    Serial.print("\n  Soil Moisture Capacitance: ");
    Serial.print(plant->sensor.getCapacitance());
    Serial.print("\n  Temperature: ");
    Serial.print(plant->sensor.getTemperature()/(float)10);
    Serial.print("\n  Light: ");
    Serial.println(plant->sensor.getLight(true));
}

static void setup_sensors_and_motors() {
  Serial.print("Scanning I2C bus for devices...\n--\n");

  uint8_t sensor_idx = 0;

  // Run through all available 128 addresses
  for(uint8_t address = 1; address < 127; address++) {
    uint8_t error;

    // try sending something on address
    Wire.beginTransmission(address);
    // ended attempt - will return an error code (!= 0) if no device
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address: ");
      print_address(address, false);
      Serial.print((String)" !\n  Seting up sensor and motor for plant " + sensor_idx + "...");

      uint8_t step = sensor_idx * 2;
      uint8_t dir  = sensor_idx * 2;

      pinMode(step, OUTPUT);
      pinMode(dir , OUTPUT);

      digitalWrite(step, LOW); 
      digitalWrite(dir , LOW); 

      plant[sensor_idx] = {
        .id             = sensor_idx,
        .motor_step_pin = step,
        .motor_dir_pin  = dir,
        //TODO .sensor = sensor can this be null?
      };

      plant[sensor_idx].sensor.setAddress(address, true);
      plant[sensor_idx].sensor.begin();
      
      Serial.println(" done!");
    } else {
      Serial.print("Error at address: ");
      print_address(address, true);
    }
    sensor_idx++;
  }
  Serial.print((String)"--\nDone setting up I2C slaves. " + sensor_idx + " sensors found.\n\n");
}

void initialise_water_indicator() {
  Serial.print("Setting pins for water level indicator...");

  // Ultra sound sensor
  pinMode(ULTRASOUND_TRIGGERING_PIN, OUTPUT); // trigger
  pinMode(ULTRASOUND_LISTENING_PIN , INPUT );

  // Set LED pins
  pinMode(WATER_LVL_0_LED_PIN, OUTPUT);
  pinMode(WATER_LVL_1_LED_PIN, OUTPUT);
  pinMode(WATER_LVL_2_LED_PIN, OUTPUT);
  pinMode(WATER_LVL_3_LED_PIN, OUTPUT);
  pinMode(WATER_LVL_4_LED_PIN, OUTPUT);

  // Turn off LEDs
  digitalWrite(WATER_LVL_0_LED_PIN, LOW); 
  digitalWrite(WATER_LVL_1_LED_PIN, LOW); 
  digitalWrite(WATER_LVL_2_LED_PIN, LOW); 
  digitalWrite(WATER_LVL_3_LED_PIN, LOW); 
  digitalWrite(WATER_LVL_4_LED_PIN, LOW); 

  Serial.println(" done!");
}

uint8_t current_water_level() {
  Serial.print("Get water level...");

  int duration, distance; 

  // Send ultrasound
  digitalWrite(ULTRASOUND_TRIGGERING_PIN, HIGH);
  delayMicroseconds(1000); 
  digitalWrite(ULTRASOUND_TRIGGERING_PIN, LOW); 

  // Wait for sound wave return
  duration = pulseIn(ULTRASOUND_LISTENING_PIN,HIGH); 

  // Calculate the distance in cm
  distance = (duration/2)/29.1; 
  
  Serial.println((String)" done (water level at " + distance + " cm)!");

  // TODO could probably done with some modulus magic instead.
  if      (distance >  0.0 && distance <=  5.0) return 5;
  else if (distance >  5.0 && distance <= 10.0) return 4;
  else if (distance > 10.0 && distance <= 15.0) return 3;
  else if (distance > 15.0 && distance <= 20.0) return 2;
  else if (distance > 20.0 && distance <= 25.0) return 1;
  else /* if (distance > 30) */                 return 0;
}

void update_water_indicator(uint8_t water_level) {
  Serial.print("Updating water level indicator LEDs...");
  switch (water_level) {
    case 0: digitalWrite(WATER_LVL_0_LED_PIN, LOW ); 
            digitalWrite(WATER_LVL_1_LED_PIN, LOW ); 
            digitalWrite(WATER_LVL_2_LED_PIN, LOW ); 
            digitalWrite(WATER_LVL_3_LED_PIN, LOW ); 
            digitalWrite(WATER_LVL_4_LED_PIN, LOW ); 
    case 1: digitalWrite(WATER_LVL_0_LED_PIN, HIGH); 
    case 2: digitalWrite(WATER_LVL_1_LED_PIN, HIGH); 
    case 3: digitalWrite(WATER_LVL_2_LED_PIN, HIGH); 
    case 4: digitalWrite(WATER_LVL_3_LED_PIN, HIGH); 
    case 5: digitalWrite(WATER_LVL_4_LED_PIN, HIGH); 
  }
  Serial.print(" done!");
}

void water_plant(plant_info *plant) {
  Serial.print((String)"Watering plant " + plant->id + "...");
  for(int i = 0; i < WATERING_PULSES; i++) {
    digitalWrite(plant->motor_step_pin,HIGH);
    delayMicroseconds(200);
    digitalWrite(plant->motor_step_pin,LOW );
    delayMicroseconds(200);
  }
  Serial.println(" done!");
}

void setup() {
  Serial.print((String)
    "========================================================\n"+
    "The HappyPlant system is initialising... please hang on!\n"+
    "========================================================\n\n");

  setup_sensors_and_motors();
  initialise_water_indicator();

  delay(1000);

  Serial.print((String)"\n"+
    "=============================\n"+
    "HappyPlant is now operational\n"+
    "=============================\n\n");
}

void loop() {
  // Get water level in water tank
  uint8_t level = current_water_level();
  // ...and light appropriate LEDs
  update_water_indicator(level);

  // Don't even attempt to water plants if water tank is empty
  // TODO maybe still do/print the sensor updates
  if (level == 0) return;

  // Output sensor info and possibly water plants
  for (int i = 0; i < NO_OF_PLANTS; i++) {
    // If sensor cannot be accessed, skip to next
    if (plant[i].sensor.isBusy()) continue;

    // Print some information regarding plant
    output_plant_info(&plant[i]);

    // Water plants if they are dry
    if (plant[i].sensor.getCapacitance() > CAPACITANCE_THRESHOLD)
      water_plant(&plant[i]);

    // Put sensor to sleep until next reading to save power
    plant[i].sensor.sleep();
  }

  // Wait half a second before repeating
  delay(500);
}

// vim: ft=arduino sw=2 ts=2 sts=2 tw=78 cc=80 fdm=syntax
