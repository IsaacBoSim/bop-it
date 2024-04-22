#include <Arduino.h>

const int buttonPush = 2; // Button pin for "Push it!"
const int buttonPull = 3; // Button pin for "Pull it!"
const int buttonTwist = 4; // Corrected pin for "Twist it!"

const char* commands[] = {"Push it!", "Pull it!", "Twist it!"};

unsigned long lastCommandTime;
int currentButton = -1;
long responseTime = 5000; // Initial time to respond
int lives = 3; // Player lives
bool gameOver = false; // Game over status
int score = 0; // Player score

void setup() {
  Serial.begin(9600);
  pinMode(buttonPush, INPUT);
  pinMode(buttonPull, INPUT);
  pinMode(buttonTwist, INPUT);

  randomSeed(analogRead(0));
  giveCommand();
}

void loop() {
  if (millis() - lastCommandTime > responseTime) {
    Serial.print("Too slow! ");
    loseLife();
  }

  if (!gameOver) {
    checkButton(buttonPush, 0);
    checkButton(buttonPull, 1);
    checkButton(buttonTwist, 2);
  }

  if (gameOver) {
    handleRestart();
  }
}

void checkButton(int buttonPin, int buttonId) {
  if (digitalRead(buttonPin) == HIGH) {
    while (digitalRead(buttonPin) == HIGH) {
      delay(10); // Debouncing
    }
    if (currentButton == buttonId) {
      incrementScore();
    } else {
      loseLife();
    }
  }
}

void giveCommand() {
  currentButton = random(0, 3); // Selects 0, 1, or 2 corresponding to Push, Pull, Twist
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
  Serial.print("You lose a life! ");
  Serial.print("You have ");
  lives--;
  Serial.print(lives);
  Serial.println(" lives left!");
  if (lives <= 0) {
    endGame();
  } else {
    giveCommand();
  }
}

void handleRestart() {
  Serial.println("Press any button to restart.");
  while (true) {
    if (digitalRead(buttonPush) == HIGH || digitalRead(buttonPull) == HIGH || digitalRead(buttonTwist) == HIGH) {
      while (digitalRead(buttonPush) == HIGH || digitalRead(buttonPull) == HIGH || digitalRead(buttonTwist) == HIGH) {
        delay(10); // Wait for all buttons to be released
      }
      gameOver = false;
      lives = 3;
      score = 0;
      responseTime = 5000;
      giveCommand();
      break;
    }
  }
}

void levelUp() {
  responseTime = max(1000, responseTime - 500); // Never go below 1 second
  Serial.println("Level up!");
  delay(1500);
}

void incrementScore() {
  score++;
  Serial.print("Correct! ");
  Serial.print("Your Score: ");
  Serial.println(score);
  if (score % 10 == 0) {
    levelUp();
  }
  giveCommand();
}
