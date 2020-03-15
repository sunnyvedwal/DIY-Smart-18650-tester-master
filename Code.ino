/*
                     _                                   ______                 _            
     /\             | |                                 |  ____|               | |           
    /  \   _ __  ___| |__  _   _ _ __ ___   __ _ _ __   | |__ __ _ _   _ ______| | __ _ _ __ 
   / /\ \ | '_ \/ __| '_ \| | | | '_ ` _ \ / _` | '_ \  |  __/ _` | | | |_  / _` |/ _` | '__|
  / ____ \| | | \__ \ | | | |_| | | | | | | (_| | | | | | | | (_| | |_| |/ / (_| | (_| | |   
 /_/    \_\_| |_|___/_| |_|\__,_|_| |_| |_|\__,_|_| |_| |_|  \__,_|\__,_/___\__,_|\__,_|_|   
                                                                                            
*/                                                                                                                                                                        
                                                                                                                                                                        
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

Adafruit_BMP280 bmp; // I2C

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define MOSFET_Pin 2
#define Bat_Pin A0
#define Res_Pin A1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
float Capacity = 0.0; // Capacity in mAh
float Res_Value = 10.0;  // Resistor Value in Ohm
float Vcc = 4.76; // Voltage of Arduino 5V pin ( Mesured by Multimeter )
float Current = 0.0; // Current in Amp
float mA=0;         // Current in mA
float Bat_Volt = 0.0;  // Battery Voltage 
float Res_Volt = 0.0;  // Voltage at lower end of the Resistor 
float Bat_High = 4.2; // Battery High Voltage
float Bat_Low = 3.3; // Discharge Cut Off Voltage
unsigned long previousMillis = 0; // Previous time in ms
unsigned long millisPassed = 0;  // Current time in ms
float sample1 =0;
float sample2= 0;
int x = 0;
int row = 0;



//*******************************Setup Function ***************************************************************
  
  void setup() {
   Serial.begin(115200);
    
    Serial.println(F("BMP280 test"));

  if (!bmp.begin(0x76)) {
    Serial.println(F("Bhai nhi chal rha, wiring check kar!"));
    while (1);
  }

bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
 
   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  
   pinMode(MOSFET_Pin, OUTPUT);   
   digitalWrite(MOSFET_Pin, LOW);  // MOSFET is off during the start
   Serial.println("CLEARDATA");
   Serial.println("LABEL,Time,Bat_Volt,capacity");
   
   //Serial.println("Arduino Battery Capacity Tester v1.0");
   Serial.println("BattVolt Current mAh");
  
display.setCursor(0,10);
display.println("Battery Tester v1.0");
display.display();
delay(1000);
display.clearDisplay();
  
  }

  
  
//********************************Main Loop Function***********************************************************
  
  void loop() {

//Vcc = readVcc()/1000.0; // Conevrrt mV to Volt

  
//Voltage devider Out = Bat_Volt * R2/(R1+R2 ) // R1 =10K and R2 =10K 
  
  //************ Measuring Battery Voltage ***********

if( Bat_Volt < 1){
  display.setCursor(0,10);
  display.println("No Battery");
   display.display();  
    }
else if ( Bat_Volt > Bat_High){
  display.setCursor(0,10);
  display.println("High Voltage"); 
  display.display(); 

  
  } 
else if(Bat_Volt < Bat_Low){
  display.setCursor(0,10);
  display.println("Low Voltage"); 
  display.display(); 
  }
else if(Bat_Volt >= Bat_Low && Bat_Volt < Bat_High){
  display.setCursor(0,10);
  display.println("Volt:");
  display.setCursor(0,20);
  display.println("Current:");
   display.setCursor(0,30);
  display.println("mAh:");
  
  display.setCursor(30,10);
  display.print(Bat_Volt,2);
  display.println("V");
  display.display(); 
  display.setCursor(55,20);
  display.print(mA,0);
  display.println("mA");
  display.display(); 
  display.setCursor(20,30);
  display.print(Capacity, 1);
  display.display(); 
  }
  
//************ Measuring Temperature ***********
 display.setCursor(0,55);
 display.println("T:");
 display.display();
 display.setCursor(12,55);
 display.println(bmp.readTemperature());
 display.display(); 
 display.setCursor(40,55);
 display.println((char)247);
 display.display(); 
 display.setCursor(45,55);
 display.println("C");
 display.display();
 display.clearDisplay();
 


Serial.print(F("Temperature = "));
Serial.print(bmp.readTemperature());
Serial.println(" *C");

Serial.print(F("Pressure = "));
Serial.print(bmp.readPressure());
Serial.println(" Pa");

Serial.print(F("Approx altitude = "));
Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
Serial.println(" m");

Serial.println();
delay(100);


  
  for(int i=0;i< 100;i++)
  {
   sample1=sample1+analogRead(Bat_Pin); //read the voltage from the divider circuit
   delay (2);
  }
  sample1=sample1/100; 
  Bat_Volt = 1* sample1 *Vcc/ 1024.0; 

  // *********  Measuring Resistor Voltage ***********

   for(int i=0;i< 100;i++)
  {
   sample2=sample2+analogRead(Res_Pin); //read the voltage from the divider circuit
   delay (2);
  }
  sample2=sample2/100;
  Res_Volt = 1* sample2 * Vcc/ 1024.0;

  //********************* Checking the different conditions *************
  
  if ( Bat_Volt > Bat_High){
    digitalWrite(MOSFET_Pin, LOW); // Turned Off the MOSFET // No discharge 
    Serial.println( "Warning High-V! ");
    delay(100);
   }
   
   else if(Bat_Volt < Bat_Low){
      digitalWrite(MOSFET_Pin, LOW);      
      Serial.println( "Warning Low-V! ");
      delay(100);
  }
  else if(Bat_Volt > Bat_Low && Bat_Volt < Bat_High  ) { // Check if the battery voltage is within the safe limit
      digitalWrite(MOSFET_Pin, HIGH);
      millisPassed = millis() - previousMillis;
      Current = (Bat_Volt - Res_Volt) / Res_Value;
      mA = Current * 1000.0 ;
      Capacity = Capacity + mA * (millisPassed / 3600000.0); // 1 Hour = 3600000ms
      previousMillis = millis();
      Serial.print("DATA,TIME,"); Serial.print(Bat_Volt); Serial.print(","); Serial.println(Capacity);
      row++;
      x++;
      delay(4000); 
 
     }
 
 }    
