#include <Arduino.h>

const int buttonPins[] = {2, 3, 4}; // Button pins
const char* commands[] = {"Push it!", "Pull it!", "Twist it!"};
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

unsigned long lastCommandTime;
int currentButton = -1;
long responseTime = 5000; // Initial time to respond
int lives = 3; // Player lives
bool gameOver = false; // Game over status
int score = 0; // Player score
bool waitingForRelease = false;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  randomSeed(analogRead(0));
  giveCommand();
}

void loop() {
  if (millis() - lastCommandTime > responseTime && !gameOver) {
    Serial.println("Too slow!");
    loseLife();
  }

  if (!gameOver) {
    for (int i = 0; i < numButtons; i++) {
      if (digitalRead(buttonPins[i]) == LOW && !waitingForRelease) {
        if (i == currentButton) {
          Serial.println("Correct!");
          score++;
          giveCommand();
          if (score % 10 == 0) {
            levelUp();
          }
        } else {
          Serial.println("Wrong button!");
          loseLife();
        }
        waitingForRelease = true;
      } else if (digitalRead(buttonPins[i]) == HIGH) {
        waitingForRelease = false;
      }
    }
  }

  if (gameOver) {
    handleRestart();
  }
}

void giveCommand() {
  currentButton = random(0, numButtons);
  Serial.println(commands[currentButton]);
  lastCommandTime = millis();
}

void endGame() {
  Serial.println("Game over!");
  Serial.print("Score: ");
  Serial.println(score);
  Serial.print("Level: ");
  Serial.println(score / 10);
  gameOver = true;
}

void loseLife() {
  Serial.println("You lose a life!");
  lives--;
  if (lives <= 0) {
    endGame();
  } else {
    giveCommand();
  }
}

void handleRestart() {
  Serial.println("Press any button to restart.");
  delay(500);
  while (true) {
    for (int i = 0; i < numButtons; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        gameOver = false;
        lives = 3;
        score = 0;
        responseTime = 3000;
        giveCommand();
        return;
      }
    }
  }
}

void levelUp() {
  responseTime = max(1000, responseTime - 500); // Never go below 1 second
  Serial.println("Level up!");
}
