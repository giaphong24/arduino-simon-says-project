/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID       "TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME     "Simple Simon Say project"
#define BLYNK_AUTH_TOKEN        "AUTH_TOKEN"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <ThingSpeak.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "INSERT_WIFI_NAME_HERE";
char pass[] = "WIFI_PASSWORD";

// for Thingspeak
unsigned long channelID = ...;
const char* apiKey = "APIKEY";

#define NOTE_C3  131
#define NOTE_G3  196
#define NOTE_E3  165
#define NOTE_G2  98

#define NOTE_B5 988
#define NOTE_G5 784

#define NOTE_C5 523
#define NOTE_G4 392
#define NOTE_E4 330
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_GS4 415
#define NOTE_AS4 466
#define NOTE_D4 294
#define NOTE_E4 330

int marioGameStart[] = {NOTE_E3, NOTE_E3, NOTE_E3, 0, NOTE_C3, NOTE_E3, 0, NOTE_G3, 0, NOTE_G2, 0, 0, 0};
int correct[] = {NOTE_B5, NOTE_G5, NOTE_B5, NOTE_G5};
int gameOver[] = {NOTE_C5, 0, NOTE_G4, 0, NOTE_E4, NOTE_A4, 
                  NOTE_B4, NOTE_A4, NOTE_GS4, NOTE_AS4, NOTE_GS4, NOTE_G4, NOTE_D4, NOTE_E4};
const int MAX_LEVEL  = 100;
int sequence[MAX_LEVEL];
int your_sequence[MAX_LEVEL];
int level = 1;

int score = 0;

int velocity = 1000;

// LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
// initialize the LCD with I2C module
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define ledG D3
#define ledY D4
#define ledB D5
#define ledR D6

#define buzzer D8

int updatedPin = -1;
int updatedValue = -1;

BlynkTimer timer;

void blynkUpdate();

void setup() {
  // Debug console
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(ledR, OUTPUT);       // Red LED
  pinMode(ledB, OUTPUT);       // Blue LED
  pinMode(ledY, OUTPUT);       // Yellow LED
  pinMode(ledG, OUTPUT);       // Green LED

  pinMode(buzzer, OUTPUT);         // pin of the buzzer

  digitalWrite(ledR, LOW); 
  digitalWrite(ledB, LOW);
  digitalWrite(ledG, LOW);
  digitalWrite(ledY, LOW);

  digitalWrite(buzzer, LOW);

  // lcd.begin(16, 2);  // Set the number of columns and rows on the LCD
  // initialize the LCD
  lcd.init();
  lcd.backlight();

  timer.setInterval(1000L, blynkUpdate); // Run Blynk communication every 1 second
}

void loop()
{
  Blynk.run();
  timer.run();

  if  (level == 1){
    lcd.setCursor(0, 0);
    lcd.print("Simon Says Game");
    lcd.setCursor(1, 1);
    lcd.print("By Group: 6868");

    delay(5000);
    
    generate_sequence();//generate a sequence;
  }

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Level: ");
  lcd.print(level);
  lcd.setCursor(4, 1);
  lcd.print("Score: ");
  lcd.print(score);

  // play the starting music
  for (int i = 0; i < sizeof(marioGameStart)/sizeof(int); i++) {
    // play the start music
    int noteDuration = 200;
    tone(buzzer, marioGameStart[i], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzer);
  }
  
  delay(500);         // pause for 0.5 seconds before playing
  // Check if any virtual pin has been updated
  if (updatedPin != -1) {
    int pin = updatedPin;
    int value = updatedValue;

    // Process the updated pin and value here

    // Reset the updated pin and value
    updatedPin = -1;
    updatedValue = -1;
  }
  show_sequence();    //show the sequence
  get_sequence();     //wait for your sequence  
}

// Blynk virtual pin update handler for V1
BLYNK_WRITE(V1) {
  int value = param.asInt();
  // Update the updatedPin and updatedValue variables
  updatedPin = V1;
  updatedValue = value;
}

// Blynk virtual pin update handler for V2
BLYNK_WRITE(V2) {
  int value = param.asInt();
  // Update the updatedPin and updatedValue variables
  updatedPin = V2;
  updatedValue = value;
}

// Blynk virtual pin update handler for V3
BLYNK_WRITE(V3) {
  int value = param.asInt();
  // Update the updatedPin and updatedValue variables
  updatedPin = V3;
  updatedValue = value;
}

// Blynk virtual pin update handler for V4
BLYNK_WRITE(V4) {
  int value = param.asInt();
  // Update the updatedPin and updatedValue variables
  updatedPin = V4;
  updatedValue = value;
}

void blynkUpdate() {
  Blynk.run();
}

void  show_sequence()   // show a different sequence for each level
{
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Loading...");
  
  digitalWrite(ledR, LOW); 
  digitalWrite(ledB, LOW);
  digitalWrite(ledG, LOW);
  digitalWrite(ledY, LOW);

  for (int i = 0; i < level; i++)
  {
    int pin = sequence[i];
    switch (pin)
    {
      case 2:
        digitalWrite(ledR, HIGH);
        play_tone(pin, 150);
        delay(velocity);
        digitalWrite(ledR, LOW);
        break;
      case 3:
        digitalWrite(ledB, HIGH);
        play_tone(pin, 150);
        delay(velocity);
        digitalWrite(ledB, LOW);
        break;
      case 4:
        digitalWrite(ledY, HIGH);
        play_tone(pin, 150);
        delay(velocity);
        digitalWrite(ledY, LOW);
        break;
      case 5:
        digitalWrite(ledG, HIGH);
        play_tone(pin, 150);
        delay(velocity);
        digitalWrite(ledG, LOW);
        break;
    }
    delay(200);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please input the");

  lcd.setCursor(3, 1);
  lcd.print("Sequence!");
}

// play_tone() method for each sound of the LEDs
void play_tone(int pin, int duration)
{
  switch (pin)
  {
    case 2:
      tone(buzzer, 1500, duration);
      break;
    case 3:
      tone(buzzer, 1400, duration);
      break;
    case 4:
      tone(buzzer, 1300, duration);
      break;
    case 5:
      tone(buzzer, 1200, duration);
      break;
  }
}

void  get_sequence()
{
  int flag = 0; // this flag indicates if the sequence is correct

  for  (int i = 0; i < level; i++)
  {
    flag = 0;
    while(flag == 0)
    {
      
      // Check if any virtual pin has been updated
      if (updatedPin != -1)
      {
        int pin = updatedPin;
        int value = updatedValue;

        if (pin == V1 && value == HIGH)
        {
          digitalWrite(ledG, HIGH);
          tone(buzzer, 1200, 200);
          your_sequence[i] = 5;
          flag = 1;
          delay(200);
          if (your_sequence[i] != sequence[i])
          {
            wrong_sequence();
            return;
          }
          digitalWrite(ledG, LOW);
        }

        if (pin == V2 && value == HIGH)
        {
          digitalWrite(ledY, HIGH);
          tone(buzzer, 1300, 200);
          your_sequence[i] = 4;
          flag = 1;
          delay(200);
          if (your_sequence[i] != sequence[i])
          {
            wrong_sequence();
            return;
          }
          digitalWrite(ledY, LOW);
        }

        if (pin == V3 && value == HIGH)
        {
          digitalWrite(ledB, HIGH);
          tone(buzzer, 1400, 200);
          your_sequence[i] = 3;
          flag = 1;
          delay(200);
          if (your_sequence[i] != sequence[i])
          {
            wrong_sequence();
            return;
          }
          digitalWrite(ledB, LOW);
        }

        if (pin == V4 && value == HIGH)
        {
          digitalWrite(ledR, HIGH);
          tone(buzzer, 1500, 200);
          your_sequence[i] = 2;
          flag = 1;
          delay(200);
          if (your_sequence[i] != sequence[i])
          {
            wrong_sequence();
            return;
          }
          digitalWrite(ledR, LOW);
        }

        // Reset the updated pin and value
        updatedPin = -1;
        updatedValue = -1;
      }

      Blynk.run(); // Process Blynk communications
    }
  }
  right_sequence();
}

void generate_sequence()
{
  randomSeed(millis());  // Generate random sequence

  for (int i = 0; i < MAX_LEVEL; i++)
  {
    sequence[i]  = random(2,6);
  }

  // // Convert the sequence to a string
  // String sequenceString = "";
  // for (int i = 0; i < MAX_LEVEL; i++) {
  //   sequenceString += String(sequence[i]) + ",";
  // }
  // // Remove the trailing comma
  // sequenceString = sequenceString.substring(0, sequenceString.length() - 1);

  // // Upload the sequence to ThingSpeak
  // ThingSpeak.writeField(channelID, 1, sequenceString, apiKey);
}

void wrong_sequence()
{
  // displays game over message and final score
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Final Score: ");
  lcd.print(score);

  for (int i = 0; i < sizeof(gameOver)/sizeof(int); i++) {
      digitalWrite(ledR, HIGH); 
      digitalWrite(ledB, HIGH);
      digitalWrite(ledG, HIGH);
      digitalWrite(ledY, HIGH);
      // play the wrong/game over music
      int noteDuration = 200;
      tone(buzzer, gameOver[i], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(250);
      digitalWrite(ledR, LOW); 
      digitalWrite(ledB, LOW);
      digitalWrite(ledG, LOW);
      digitalWrite(ledY, LOW);
      noTone(buzzer);
  }

  score = 0;      // set the initial score to 0
  level = 1;     // set the initial level back to 1
  velocity = 1000;
  
  lcd.clear();
}

void right_sequence()
{
  // display the correct message
  for (int i = 0; i < 2; i++){
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Correct!");
    delay(500);
  }
  
  for (int i = 0; i < sizeof(correct)/sizeof(int); i++) {
      // play the correct music
      digitalWrite(ledR, HIGH); 
      digitalWrite(ledB, HIGH);
      digitalWrite(ledG, HIGH);
      digitalWrite(ledY, HIGH);
      int noteDuration = 200;
      tone(buzzer, correct[i], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      digitalWrite(ledR, LOW); 
      digitalWrite(ledB, LOW);
      digitalWrite(ledG, LOW);
      digitalWrite(ledY, LOW);
      noTone(buzzer);
      delay(150);
  }

  // Increment the score and display on LCD
  score += level;

  if  (level < MAX_LEVEL);
  level++;        // increase the level (number of blinking LEDs)

  velocity -= 50; // increase difficulty
  delay(500);
}
