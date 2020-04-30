#include <Arduino.h>
// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
#define NUM_EXPECTED_DT 5

typedef struct Dallas_IoT {
  const char *NodeKey;
  const char field[10];
  DeviceAddress address;
};

const char InsulationToWallNodeKey[23]="pw7k9YGPzYcgMZrzoSxMOg";
const char InsulationToPanelNodeKey[23]="u9UQgH7_H-Ka8mPQRhtAiA";
const char InRoomNodeKey[23]="m0ZmAENUJ0x44x3Ah0xMAw";

struct Dallas_IoT  TM[NUM_EXPECTED_DT] = {
  {InsulationToWallNodeKey, "dt1",{0x28, 0xFF, 0x48, 0x0E, 0x61, 0x16, 0x03, 0x20}},
  {InsulationToWallNodeKey, "dt2",{0x28, 0xFF, 0xC6, 0x20, 0x62, 0x16, 0x03, 0xC8}},
  {InsulationToWallNodeKey, "dt3",{0x28, 0xFF, 0xF1, 0x66, 0x63, 0x16, 0x03, 0x5F}},
  {InsulationToWallNodeKey, "dt4",{0x28, 0xFF, 0x29, 0x02, 0x36, 0x16, 0x03, 0xEF}},
  {InsulationToWallNodeKey, "dt5",{0x28, 0xFF, 0x2F, 0x49, 0x63, 0x16, 0x04, 0xCA}}
};

// Assign address manually. The addresses below will beed to be changed
// to valid device addresses on your bus. Device address can be retrieved
// by using either oneWire.search(deviceAddress) or individually via
// sensors.getAddress(deviceAddress, index)
// DeviceAddress insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
// DeviceAddress outsideThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

void printTemperature(DeviceAddress deviceAddress);
void printAddress(DeviceAddress deviceAddress);
void printData(DeviceAddress deviceAddress);

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin();

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  // Search for devices on the bus and assign based on an index. Ideally,
  // you would do this to initially discover addresses on the bus and then
  // use those addresses and manually assign them (see above) once you know
  // the devices on your bus (and assuming they don't change).
  //
  // method 1: by index
  /*if (!sensors.getAddress(TM1, 0)) Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(TM2, 1)) Serial.println("Unable to find address for Device 1");
  if (!sensors.getAddress(TM3, 2)) Serial.println("Unable to find address for Device 2");
  if (!sensors.getAddress(TM4, 3)) Serial.println("Unable to find address for Device 3");
  if (!sensors.getAddress(TM5, 4)) Serial.println("Unable to find address for Device 4");
*/

  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices,
  // or you have already retrieved all of them. It might be a good idea to
  // check the CRC to make sure you didn't get garbage. The order is
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  oneWire.reset_search();
  // assigns the first address found to insideThermometer
  for (int i=0; i < NUM_EXPECTED_DT; i++) {
    if (!oneWire.search(TM[i].address)) {
      Serial.print("Unable to find address for device ");
      Serial.println(i);
    } else {
      sensors.setResolution(TM[i].address, TEMPERATURE_PRECISION);
      Serial.print("Device ");
      Serial.print(i);
      Serial.print(" Resolution: ");
      Serial.print(sensors.getResolution(TM[i].address), DEC);
      Serial.println();
    }
}

// function to print a device address


void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  
  Serial.print(tempC);
  Serial.print("[C] ");
  //Serial.print(" Temp F: ");
  //Serial.print(DallasTemperature::toFahrenheit(tempC));
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}

/*
   Main function, calls the temperatures in a loop.
*/
void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");

  // print the device information
  for (int i=0; i < NUM_EXPECTED_DT; i++) {
    Serial.print(sensors.getTempC(TM[i].address));
    Serial.print("*C ");

  Serial.println();
  delay(1000);
}