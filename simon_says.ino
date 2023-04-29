/*This sketch is a simple version of the famous Simon Says game. You can  use it and improved it adding
levels and everything you want to increase the  diffuculty!

There are five buttons connected to A0, A1, A2, A3 and A4.
The  buttons from A0 to A3 are used to insert the right sequence while A4 to start the  game.

When a wrong sequence is inserted all the leds will blink for three  time very fast otherwhise the
inserted sequence is correct.

Hardware needed:
5x  pushbuttons
1x Blue led
1x Yellow led
1x Red led
1x Green Led
4x  1k resistors
4x 10k resisors
10x jumpers
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

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

const int MAX_LEVEL  = 100;
int sequence[MAX_LEVEL];
int your_sequence[MAX_LEVEL];
int level = 1;

int score = 0;

int velocity = 1000;

int marioGameStart[] = {NOTE_E3, NOTE_E3, NOTE_E3, 0, NOTE_C3, NOTE_E3, 0, NOTE_G3, 0, NOTE_G2, 0, 0, 0};
int correct[] = {NOTE_B5, NOTE_G5, NOTE_B5, NOTE_G5};
int gameOver[] = {NOTE_C5, 0, NOTE_G4, 0, NOTE_E4, NOTE_A4, 
                  NOTE_B4, NOTE_A4, NOTE_GS4, NOTE_AS4, NOTE_GS4, NOTE_G4, NOTE_D4, NOTE_E4};

// LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
// initialize the LCD with I2C module
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  pinMode(6, OUTPUT);         // pin of the buzzer

  digitalWrite(2, LOW); 
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);

  digitalWrite(6, LOW);

  // lcd.begin(16, 2);  // Set the number of columns and rows on the LCD
  // initialize the LCD
  lcd.init();
  lcd.backlight();
}

void loop()
{
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
    tone(6, marioGameStart[i], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(6);
  }
  delay(500);         // pause for 0.5 seconds before playing
  show_sequence();    //show the sequence
  get_sequence();     //wait for your sequence  
}

void  show_sequence()   // show a different sequence for each level
{
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Loading...");
  
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);

  for (int i = 0; i < level; i++)
  {
    digitalWrite(sequence[i], HIGH);
    play_tone(sequence[i], 150);
    delay(velocity);
    digitalWrite(sequence[i], LOW);
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
      tone(6, 1500, duration);
      break;
    case 3:
      tone(6, 1400, duration);
      break;
    case 4:
      tone(6, 1300, duration);
      break;
    case 5:
      tone(6, 1200, duration);
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
      if (digitalRead(A0)  == LOW)
      {
        digitalWrite(5, HIGH);
        tone(6, 1200, 200);
        your_sequence[i] = 5;
        flag = 1;
        delay(200);
        if  (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(5, LOW);
      }

      if (digitalRead(A1) == LOW)
      {
        digitalWrite(4, HIGH);
        tone(6, 1300, 200);
        your_sequence[i]  = 4;
        flag = 1;
        delay(200);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(4,  LOW);
      }

      if (digitalRead(A2) == LOW)
      {
        digitalWrite(3, HIGH);
        tone(6, 1400, 200);
        your_sequence[i]  = 3;
        flag = 1;
        delay(200);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(3,  LOW);
      }

      if (digitalRead(A3) == LOW)
      {
        digitalWrite(2, HIGH);
        tone(6, 1500, 200);
        your_sequence[i]  = 2;
        flag = 1;
        delay(200);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(2,  LOW);
      }

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
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      // play the wrong/game over music
      int noteDuration = 200;
      tone(6, gameOver[i], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(250);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      noTone(6);
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
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      int noteDuration = 200;
      tone(6, correct[i], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      noTone(6);
      delay(150);
  }

  // Increment the score and display on LCD
  score += level;

  if  (level < MAX_LEVEL);
  level++;        // increase the level (number of blinking LEDs)

  velocity -= 50; // increase difficulty
  delay(500);
}