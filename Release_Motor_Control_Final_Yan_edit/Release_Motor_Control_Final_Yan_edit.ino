#include <Wire.h>
#include <RTClib.h>
#include <Servo.h>
#include <ArduinoBLE.h>

byte servoPin = 9;
Servo servo;
RTC_DS3231 rtc;  // Create an RTC object
bool dateTimeSet = false;  // Flag to check if the date and time have been set

// Variables for target date and time
int targetYear = 0;
int targetMonth = 0 ;
int targetDay = 0;
int targetHour = 0;
int targetMinute = 0;
int targetSecond = 0;
int val = 0;

BLEService motorService("180A"); // Standard service UUID
BLEStringCharacteristic dateTimeCharacteristic("2A57", BLEWrite, 20); // Custom characteristic for date/time

void setup() {
 Serial.begin(9600);
 servo.attach(servoPin); 
 // Start BLE
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("MotorController");
  BLE.setAdvertisedService(motorService);
  motorService.addCharacteristic(dateTimeCharacteristic);
  BLE.addService(motorService);
  BLE.advertise();
  

 if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, set the time!");
  }
  pinMode(LED_BUILTIN, OUTPUT);  // Set the onboard LED pin as output
  delay(7000); // delay to allow the ESC to recognize the stopped signal
}


void loop() {
    // Handle BLE connection and date/time setting
  BLEDevice central = BLE.central();   
  if (central) {
    Serial.print("Connected to central device");
    // Check if date and time data is received
    if (dateTimeCharacteristic.written()) {
      String dateTimeInput = dateTimeCharacteristic.value();
      Serial.print("Received date/time: ");
      Serial.println(dateTimeInput);

      // Parse the date/time input (e.g., "2024-09-05 14:30:00")
      int parsedValues = sscanf(dateTimeInput.c_str(), "%4d-%2d-%2d %2d:%2d:%2d", 
                                 &targetYear, &targetMonth, &targetDay, 
                                 &targetHour, &targetMinute, &targetSecond);

      Serial.print(parsedValues);
      Serial.println(parsedValues);


      if (parsedValues == 6) {
        Serial.println("Date and time set. Will now monitor the time...");

        // Blink the onboard LED 3 times to confirm date and time set
        for (int i = 0; i < 3; i++) {
          digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED on
          delay(200);  // Wait for 200ms
          digitalWrite(LED_BUILTIN, LOW);   // Turn the LED off
          delay(200);  // Wait for 200ms
        }

        dateTimeSet = true;  // Set the flag to indicate date and time are set
      } else {
        Serial.println("Failed to parse date/time. Ensure the format is YYYY-MM-DD HH:MM:SS");
      }
    }
  }

    // Continue running the motor control logic constantly

  
  DateTime now = rtc.now();  // Get the current time

  //Print the current date and time
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  delay(1000);




  // Check if the current date and time match the target date and time
  if (dateTimeSet && now.year() == targetYear && now.month() == targetMonth && now.day() == targetDay &&
      now.hour() == targetHour && now.minute() == targetMinute && now.second() == targetSecond) {

    Serial.println("Turning on motor...");
    servo.writeMicroseconds(1500);
    delay(1000);

    servo.writeMicroseconds(1400); 
    delay(1000);

    Serial.println("This is after the 1400");

    delay(1000);

    servo.writeMicroseconds(1500); 

    delay(2000);

    Serial.println("Code is done");

    dateTimeSet = false;

     // if(val < 1100 || val > 1900)
  //{
    //Serial.println("not valid");
  //}
  //else
  //{
    //servo.writeMicroseconds(val); // Send signal to ESC. //
      //}
  
  //}
  
  //Serial.println("Enter PWM signal value 1100 to 1900, 1500 to stop");
  
  //while (Serial.available() == 0);
  
  //int val = Serial.parseInt(); 
  
  //if(val < 1100 || val > 1900)
  //{
    //Serial.println("not valid");
  //}
  //else
  //{
    //servo.writeMicroseconds(val); // Send signal to ESC. //}
//}
    // Re-advertise BLE when disconnected
  if (!central) {
    BLE.advertise();
  }

}
}
