//Blynk IOT Multimeter on Nodemcu
//This code is developed by Ananth_vipas
// Fill-in information from your Blynk Template here
//https://github.com/ananthvipas
//IF you have any doubt / requirements Discuss in github
/****************************************************************************/

#define BLYNK_TEMPLATE_ID "TMPL35bnLsw5L"
#define BLYNK_TEMPLATE_NAME "DCpowerMETER"
#define BLYNK_FIRMWARE_VERSION "0.1.0"

#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_SSD1306.h>
#include <BlynkSimpleEsp8266.h>

/****************************************************************************/
// OLED Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);  //Display 

Adafruit_INA219 ina219;   // INA219 Current Sensor

// WiFi credentials
char auth[] = "hIHltgNrITosa3EtirSlvfDLbO3rjHzE";
char ssid[] = "Type your SSID";   //SSID-wifi (or) Router Name
char pass[] = "Type your password";   //wifi (or) Router password

// Blynk Virtual Pins
#define LOADVOLTAGE_VIRTUAL_PIN V0    
#define CURRENT_VIRTUAL_PIN V1    
#define POWER_VIRTUAL_PIN V2
#define ENERGY_VIRTUAL_PIN V3
#define CAPACITY_VIRTUAL_PIN V4
#define ENERGYCOST_VIRTUAL_PIN V5

//Set the all initial values are zero
unsigned long previousMillis = 0;
unsigned long interval = 2000;
float shuntvoltage = 0.00;
float busvoltage = 0.00;
float current = 0.00;
float loadvoltage = 0.00;
float energy = 0.00,  energyCost, energyPrevious, energyDifference;
float energyPrice = 6.50 ;      //price of the electricity
float power = 0.00;
float capacity = 0.00;

void setup()
{
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
  ina219.setCalibration_32V_2A();
  uint32_t currentFrequency;
  Serial.begin(115200);

  // initialize OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(10, 30);
  display.print("IoT Multimeter");
  display.display(); 
  
    if (! ina219.begin()) 
    {
    Serial.println("Failed to find INA219 chip");
    while (1) 
    {
      delay(10);
    }
  }

  Blynk.begin(auth, ssid, pass);
  Serial.println("IoT Multimeter Developed by Ananth_VipAS");
  Serial.println("IoT Multimeter with INA219 ...");

  
}

void loop()
{
  Blynk.run();
  delay(500);    // You can adjust the delay based on your requirements

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    readAndSendDataToBlynk();
    displaydata();
  }
}

void readAndSendDataToBlynk()  {

  //Calculation of all parameters
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current = ina219.getCurrent_mA();
  loadvoltage = current * 0.05;
  power = loadvoltage * current;
  energy = energy + power / 3600; //Wh
  capacity = capacity + current / 1000;
  energyDifference = energy - energyPrevious;
  energyPrevious = energy;
  energyCost = energyCost + (energyPrice / 1000 * energyDifference);
  
  // Send data to blynk

  Blynk.virtualWrite(LOADVOLTAGE_VIRTUAL_PIN, loadvoltage);
  Blynk.virtualWrite(CURRENT_VIRTUAL_PIN, current);
  Blynk.virtualWrite(POWER_VIRTUAL_PIN, power);
  Blynk.virtualWrite(ENERGY_VIRTUAL_PIN, energy);
  Blynk.virtualWrite(CAPACITY_VIRTUAL_PIN, capacity);
  Blynk.virtualWrite(ENERGYCOST_VIRTUAL_PIN, energyCost);
  
// Serial monitoring display part 
//shortcut  Ctrl+Shift+M

  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power); Serial.println(" mW");
  Serial.print("Energy:        "); Serial.print(energy); Serial.println(" Wh");
  Serial.print("Capacity:      "); Serial.print(capacity); Serial.println(" Ah");
  Serial.print("Energy Cost:   "); Serial.print("Rs. "); Serial.println(energyCost);
  Serial.println("-------------------------");
}
// Display the data in OLED 
void displaydata() 
{
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 5);     //Screen alignment point
  display.print(loadvoltage, 2);
  display.print(" V");



  // CURRENT
  if (current > 1000)
   {
    display.setCursor(60, 5);     //Screen alignment point
    display.print((current / 1000), 2);
    display.println(" A");
    display.setCursor(0, 15);     //Screen alignment point
    display.println("--------------------");
  }
  else
  {
  
    display.setCursor(60, 5);   //Screen alignment point
    display.print(current, 1);
    display.println(" mA");
    display.setCursor(0, 15);     //Screen alignment point
    display.println("--------------------");
  }


  display.setCursor(60, 20);     //Screen alignment point
  display.print("|");
  display.setCursor(60, 24);     //Screen alignment point
  display.print("|");
  display.setCursor(60, 28);     //Screen alignment point
  display.print("|");
  display.setCursor(60, 32);     //Screen alignment point
  display.print("|");
  display.setCursor(60, 36);     //Screen alignment point
  display.print("|");
  display.setCursor(60, 40);     //Screen alignment point
  display.print("|");
  display.setCursor(0, 46);     //Screen alignment point
  display.print("--------------------");

  // POWER
  if (power > 1000) 
  {
    display.setCursor(0, 24);     //Screen alignment point
    display.print(String((power / 1000), 2));
    display.println(" W");
  }
  else {
    display.setCursor(0, 24);     //Screen alignment point
    display.print(power, 2);
    display.println(" mW");
  }

  //Energy Comsumption
  if (energy > 1000) {
    display.setCursor(0, 36);     //Screen alignment point
    display.print((energy / 1000), 2);
    display.println(" kWh");
  }
  else {
    display.setCursor(0, 36);     //Screen alignment point
    display.print(energy, 2);
    display.println(" Wh");
  }


  // CAPACITY
  if (capacity > 1000)
   {
    display.setCursor(65, 24);     //Screen alignment point
    display.println("Capacity:");
    display.setCursor(65, 36);     //Screen alignment point
    display.print((capacity / 1000), 2);
    display.println(" Ah");
    
  }
  else
  {
    display.setCursor(65, 24);     //Screen alignment point
    display.println("Capacity:");
    display.setCursor(65, 36);     //Screen alignment point
    display.print(capacity, 2);
    display.println(" mAh");
 
  }


  // ENERGY COST
  display.setCursor(10, 54);     //Screen alignment point
  display.print("E Cost:Rs ");
  display.println(energyCost, 5);
  display.display();
}
