#include <LiquidCrystal.h>
#include <Keypad.h>

// Keypad
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// LCD Display
// Connect to the row pinouts of the keypad
byte rowPins[ROWS] = {
  A5, A4, A3, A2
  };

// Connect to the column pinouts of the keypad
byte colPins[COLS] = {
  A1, 8, 9
  };

// Pins
const byte speakerPIN = 12;
const byte ledPIN = 11;

// Variables
bool beep = true; // Should the speaker beep?
bool entered = false;
bool codeMatches = false;
bool exploded = false;
int tries = 3;
int secondCounter = 0; // Count seconds
int minuteCounter = 0; // Count minutes
int hourCounter = 0; // Count hours
int ledState = LOW;
int frequency = 5000; // Speaker frequency
long interval = 500; // 80 is fastest beep, 500 is default beep
unsigned long bombTimeInSeconds = 0;
unsigned long previousMillis = 0;
unsigned long previousTimerMillis = 0;
char code[7] = {'7', '3', '5', '5', '6', '0', '8'}; // Maximum code length is 7 digits
char enteredCode[7] = {'*', '*', '*', '*', '*', '*', '*'};
char bombTimer[3] = {' ', ' ', '\0'};
short unsigned int currentNumber = 7;

// Initialise Keypad & LCD Display
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

// Setup function, code here runs only once
void setup() {
  Serial.begin(9600); // Initialise serial connection
  pinMode(A0, OUTPUT); // Open pin A0 for output
  pinMode(ledPIN, OUTPUT); // Open ledPIN for output
  analogWrite(A0, 0); // Send the smallest possible signal to A0, controlling the backlight of the LCD Display

  keypad.setDebounceTime(100);

  // Initialise the LCD Display
  lcd.begin(16, 2);

  // Enter password
  lcd.setCursor(0, 0);
  lcd.print("Enter Code: ");

  for (int i = 0; i < 7; i++) {
    lcd.setCursor(i, 1);
    lcd.print(code[i]);
  }

  lcd.cursor();

  // Turn on the LED
  digitalWrite(ledPIN, HIGH);

  while (entered != true) {
    char key = keypad.getKey();

    if (currentNumber <= 6) {
      lcd.setCursor(currentNumber, 1);

      if (key != NO_KEY) {
        if (key != '*' && key != '#') {
          lcd.print(key);
          tone(speakerPIN, 3000, 150);
          code[currentNumber] = key;
          currentNumber++;
        } else {
          deleteOrAccept(key);
        }
      }
      // All digits have been filled in
    } else {
      if (key != NO_KEY) {
        deleteOrAccept(key);
      }
    }
  }
  entered = false;

  // Enter time
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter time:");

  // Hours
  lcd.setCursor(0, 1);
  lcd.print("Hours:");
  lcd.cursor();

  while (entered != true) {
    char key = keypad.getKey();
    if (currentNumber <= 1) {
      lcd.setCursor(currentNumber + 7, 1);

      if (key != NO_KEY) {
        if (key != '*' && key != '#') {
          lcd.print(key);
          tone(speakerPIN, 3000, 150);
          bombTimer[currentNumber] = key;
          currentNumber++;
        } else {
          switch (key) {
            case '*':
              tone(speakerPIN, 4000, 150);
              if (currentNumber > 0) {
                bombTimer[currentNumber - 1] = ' ';
                currentNumber--;

                for (int i = 0; i < 2; i++) {
                  lcd.setCursor(i + 7, 1);
                  lcd.print(bombTimer[i]);
                }

              }
              break;
            case '#':
              if (currentNumber == 1) {
                char temp[2];
                temp[0] = bombTimer[0];
                temp[1] = '\0';
                hourCounter = atoi(temp);
              } else {
                hourCounter = atoi(bombTimer);
              }
              tone(speakerPIN, 1500, 150);
              entered = true;
              break;
          }
        }
      }
      // All digits have been filled in
    } else {
      if (key != NO_KEY) {
        switch (key) {
          case '*':
            tone(speakerPIN, 4000, 150);
            if (currentNumber > 0) {
              bombTimer[currentNumber - 1] = ' ';
              currentNumber--;

              for (int i = 0; i < 2; i++) {
                lcd.setCursor(i + 7, 1);
                lcd.print(bombTimer[i]);
              }

            }
            break;
          case '#':
            if (currentNumber == 1) {
              char temp[2];
              temp[0] = bombTimer[0];
              temp[1] = '\0';
              hourCounter = atoi(temp);
            } else {
              hourCounter = atoi(bombTimer);
            }
            tone(speakerPIN, 1500, 150);
            entered = true;
            break;
        }
      }
    }
  }
  entered = false;
  bombTimer[0] = ' ';
  bombTimer[1] = ' ';
  currentNumber = 0;

  // Minutes
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter time:");

  lcd.setCursor(0, 1);
  lcd.print("Minutes:");
  lcd.cursor();

  while (entered != true) {
    char key = keypad.getKey();
    if (currentNumber <= 1) {
      lcd.setCursor(currentNumber + 9, 1);

      if (key != NO_KEY) {
        if (key != '*' && key != '#') {
          lcd.print(key);
          tone(speakerPIN, 3000, 150);
          bombTimer[currentNumber] = key;
          currentNumber++;
        } else {
          switch (key) {
            case '*':
              tone(speakerPIN, 4000, 150);
              if (currentNumber > 0) {
                bombTimer[currentNumber - 1] = ' ';
                currentNumber--;

                for (int i = 0; i < 2; i++) {
                  lcd.setCursor(i + 9, 1);
                  lcd.print(bombTimer[i]);
                }

              }
              break;
            case '#':
              if (currentNumber == 1) {
                char temp[2];
                temp[0] = bombTimer[0];
                temp[1] = '\0';
                minuteCounter = atoi(temp);
              } else {
                minuteCounter = atoi(bombTimer);
              }
              tone(speakerPIN, 1500, 150);
              entered = true;
              break;
          }
        }
      }
      // All digits have been filled in
    } else {
      if (key != NO_KEY) {
        switch (key) {
          case '*':
            tone(speakerPIN, 4000, 150);
            if (currentNumber > 0) {
              bombTimer[currentNumber - 1] = ' ';
              currentNumber--;

              for (int i = 0; i < 2; i++) {
                lcd.setCursor(i + 9, 1);
                lcd.print(bombTimer[i]);
              }

            }
            break;
          case '#':
            if (currentNumber == 1) {
              char temp[2];
              temp[0] = bombTimer[0];
              temp[1] = '\0';
              minuteCounter = atoi(temp);
            } else {
              minuteCounter = atoi(bombTimer);
            }
            tone(speakerPIN, 1500, 150);
            entered = true;
            break;
        }
      }
    }
  }
  entered = false;
  bombTimer[0] = ' ';
  bombTimer[1] = ' ';
  currentNumber = 0;

  // Seconds
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter time:");

  lcd.setCursor(0, 1);
  lcd.print("Seconds:");
  lcd.cursor();

  while (entered != true) {
    char key = keypad.getKey();
    if (currentNumber <= 1) {
      lcd.setCursor(currentNumber + 9, 1);

      if (key != NO_KEY) {
        if (key != '*' && key != '#') {
          lcd.print(key);
          tone(speakerPIN, 3000, 150);
          bombTimer[currentNumber] = key;
          currentNumber++;
        } else {
          switch (key) {
            case '*':
              tone(speakerPIN, 4000, 150);
              if (currentNumber > 0) {
                bombTimer[currentNumber - 1] = ' ';
                currentNumber--;

                for (int i = 0; i < 2; i++) {
                  lcd.setCursor(i + 9, 1);
                  lcd.print(bombTimer[i]);
                }

              }
              break;
            case '#':
              if (currentNumber == 1) {
                char temp[2];
                temp[0] = bombTimer[0];
                temp[1] = '\0';
                secondCounter = atoi(temp);
              } else {
                secondCounter = atoi(bombTimer);
              }
              tone(speakerPIN, 1500, 150);
              entered = true;
              break;
          }
        }
      }
      // All digits have been filled in
    } else {
      if (key != NO_KEY) {
        switch (key) {
          case '*':
            tone(speakerPIN, 4000, 150);
            if (currentNumber > 0) {
              bombTimer[currentNumber - 1] = ' ';
              currentNumber--;

              for (int i = 0; i < 2; i++) {
                lcd.setCursor(i + 9, 1);
                lcd.print(bombTimer[i]);
              }

            }
            break;
          case '#':
            if (currentNumber == 1) {
              char temp[2];
              temp[0] = bombTimer[0];
              temp[1] = '\0';
              secondCounter = atoi(temp);
            } else {
              secondCounter = atoi(bombTimer);
            }
            tone(speakerPIN, 1500, 150);
            entered = true;
            break;
        }
      }
    }
  }
  entered = false;
  bombTimer[0] = ' ';
  bombTimer[1] = ' ';
  currentNumber = 0;

  bombTimeInSeconds = (hourCounter * 60 * 60) + (minuteCounter * 60) + secondCounter; // Set bomb time in seconds for later reference

  if (bombTimeInSeconds <= 5) {
    secondCounter = 45; // Count seconds
    bombTimeInSeconds = 45;
  }
  lcd.noCursor();
  lcd.clear();

  Serial.print("planted");
}

void loop() {
  while (exploded != true && codeMatches != true) {
    countdown();
    lcd.setCursor(currentNumber, 1);

    char key = keypad.getKey();
    unsigned long currentMillis = millis();

    // Timed loop (currentMillis - previousMillis = time since last save)
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      if (ledState == LOW) {
        ledState = HIGH;
        lcd.cursor();
      } else {
        ledState = LOW;
        lcd.noCursor();
      }

      // Switch LED ON & OFF
      digitalWrite(ledPIN, ledState);

      // Only beep when LED is ON
      if (beep && ledState == HIGH) {
        tone(speakerPIN, frequency, 150);
      }
    }

    if (key != NO_KEY) {
      if (key != '*' && key != '#') {
        if (currentNumber <= 6) {
          lcd.print(key);
          enteredCode[currentNumber] = key;
          currentNumber++;
          tone(speakerPIN, 3000, 150);
        }
      } else {
        switch (key) {
          case '*':
            tone(speakerPIN, 4000, 150);
            if (currentNumber > 0) {
              enteredCode[currentNumber - 1] = '*';
              currentNumber--;

              for (int i = 0; i < 7; i++) {
                lcd.setCursor(i, 1);
                if (enteredCode[i] != '*') {
                  lcd.print(enteredCode[i]);
                } else {
                  lcd.print(' ');
                }
              }
            }
            break;
          case '#':
          tries--;
            tone(speakerPIN, 1500, 150);
            codeMatches = true;
            for (int i = 0; i < 7; i++) {
              if (code[i] != enteredCode[i]) {
                codeMatches = false;
              }
            }
            if (!codeMatches) {
              unsigned long timeLeftInSeconds = (hourCounter * 60 * 60) + (minuteCounter * 60) + secondCounter;

              timeLeftInSeconds = timeLeftInSeconds / 2;
              
              // Return timeLeftInSeconds to their original properties
              hourCounter = 0;
              minuteCounter = 0;
              secondCounter = 0;
          
              while (timeLeftInSeconds >= 3600) { // 3600 seconds == 1 hour
                hourCounter++;
                timeLeftInSeconds = timeLeftInSeconds - 3600;
              }
          
              while (timeLeftInSeconds >= 60) { // 60 seconds == 1 minute
                minuteCounter++;
                timeLeftInSeconds = timeLeftInSeconds - 60;
              }
          
              secondCounter = timeLeftInSeconds; // Give the rest to secondCounter
              if (tries <= 0) {
                exploded = true;
              }
            }
            break;
        }
      }
    }
  }
  if (exploded) {
    //BOMB HAS EXPLODED
    Serial.print("explode");
    lcd.clear();
    lcd.noCursor();
    lcd.setCursor(0, 0);
    delay(100);
    Serial.print("twin");
    lcd.print("Terrorists Win!");
    digitalWrite(ledPIN, HIGH);
    delay(60000);
  }
  if (codeMatches) {
    Serial.print("defused");
    lcd.clear();
    lcd.noCursor();
    lcd.setCursor(0, 0);
    delay(100);
    Serial.print("ctwin");
    lcd.print("Counter");
    lcd.setCursor(0, 1);
    lcd.print("Terrorists Win!");
    digitalWrite(ledPIN, LOW);

    delay(60000);
  }
}

// Countdown timer
void countdown() {
  unsigned long currentMillis = millis();
  unsigned long timeLeftInSeconds = (hourCounter * 60 * 60) + (minuteCounter * 60) + secondCounter;

  if (timeLeftInSeconds <= bombTimeInSeconds * 0.5) { // 50% time left
    interval = 250;
  }

  if (timeLeftInSeconds <= bombTimeInSeconds * 0.25) { // 25% time left
    interval = 150;
  }

  if (timeLeftInSeconds <= bombTimeInSeconds * 0.1) { // 10% time left
    interval = 100;
  }

  if (timeLeftInSeconds <= bombTimeInSeconds * 0.05) { // 5% time left
    interval = 80;
  }

  if (timeLeftInSeconds <= 1) { // 1 second left
    interval = 10;
  }

  // Timed loop (currentMillis - previousMillis = time since last save)
  if (currentMillis - previousTimerMillis >= 1000) { // If difference since last change is greater than one second
    previousTimerMillis = currentMillis;

    // Calculate total time left in seconds only to later divide it into hours, minutes and seconds
    timeLeftInSeconds = (hourCounter * 60 * 60) + (minuteCounter * 60) + secondCounter;
    // Count down one second
    if (timeLeftInSeconds >= 1) {
      timeLeftInSeconds--;
    } else {
      // BOMB HAS EXPLODED
      exploded = true;
      return;
    }

    // Return timeLeftInSeconds to their original properties
    hourCounter = 0;
    minuteCounter = 0;
    secondCounter = 0;

    while (timeLeftInSeconds >= 3600) { // 3600 seconds == 1 hour
      hourCounter++;
      timeLeftInSeconds = timeLeftInSeconds - 3600;
    }

    while (timeLeftInSeconds >= 60) { // 60 seconds == 1 minute
      minuteCounter++;
      timeLeftInSeconds = timeLeftInSeconds - 60;
    }

    secondCounter = timeLeftInSeconds; // Give the rest to secondCounter
  }

  lcd.setCursor(0, 0);
  lcd.print("Time: ");

  // If hours is greater or equal to 1
  lcd.setCursor(6, 0);
  if (hourCounter >= 1) {
    if (hourCounter >= 10) {
      lcd.print(hourCounter);
    } else {
      lcd.print('0');
      lcd.setCursor(7, 0);
      lcd.print(hourCounter);
    }
    lcd.setCursor(8, 0);
    lcd.print(':');
  } else {
    lcd.print("00:");
  }

  // If minutes is greater or equal to 1
  lcd.setCursor(9, 0);
  if (minuteCounter >= 1) {
    if (minuteCounter >= 10) {
      lcd.print(minuteCounter);
    } else {
      lcd.print('0');
      lcd.setCursor(10, 0);
      lcd.print(minuteCounter);
    }
    lcd.setCursor(11, 0);
    lcd.print(':');
  } else {
    lcd.print("00:");
  }

  // If seconds is greater or equal to 1
  lcd.setCursor(12, 0);
  if (secondCounter >= 1) {
    if (secondCounter >= 10) {
      lcd.print(secondCounter);
    } else {
      lcd.print('0');
      lcd.setCursor(13, 0);
      lcd.print(secondCounter);
    }
  } else {
    lcd.print("00");
  }
}

// Used in setup
void deleteOrAccept(char key) {
  switch (key) {
    case '*':
      tone(speakerPIN, 4000, 150);
      if (currentNumber > 0) {
        code[currentNumber - 1] = '*';
        currentNumber--;
        lcd.setCursor(currentNumber, 1);
        lcd.print('*');
      }
      break;
    case '#':
      tone(speakerPIN, 1500, 150);
      if (currentNumber >= 4) {
        entered = true;
        currentNumber = 0;

        lcd.noCursor();
        lcd.clear();
        lcd.home();
        lcd.print("You've entered: ");

        for (int i = 0; i < 7; i++) {
          lcd.setCursor(i, 1);
          if (code[i] != '*') {
            lcd.print(code[i]);
          }
        }

        delay(2500);

        lcd.clear();
        lcd.setCursor(0, 1);
      } else {
        lcd.clear();
        lcd.noCursor();
        lcd.home();
        lcd.print("Code too short");

        delay(1000);

        lcd.clear();
        lcd.print("Enter Code: ");
        lcd.setCursor(0, 1);

        for (int i = 0; i < 7; i++) {
          lcd.setCursor(i, 1);
          lcd.print(code[i]);
        }

        lcd.cursor();
      }
      break;
  }
}
