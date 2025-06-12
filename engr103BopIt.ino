//after finish the whole game, find a way to store score in CPX flash memory, erase it once the array is length of 

#include <Adafruit_CircuitPlayground.h>
#include <AsyncDelay.h>

//Initiating the pins
const int switchPin = 7;
const int leftButtonPin = 4;
const int rightButtonPin = 5;
const int ledPin = 13;

//The 5 recent scores will be stored. The most recent value will be taken from the array and shown as highscore
int player_1_highscore[5] = {};
int player_2_highscore[5] = {};

//player score that will be increased by 1
int playerscore = 0;

float speed = 2000; //initial speed at which the commands will be given. 2 second delay between each command to begin with
float speedDecrementPercent = 10; //delay will decrease by 10% as score increases
float minSpeed = 300; //delay will be capped at 800 milli seconds

bool gameInitialized = false; //redundency for gameStart flag
bool gamestart = false; //when game is started, set to true
bool player1 = false; //when player 1 is playing, set to true
bool player2 = false; //when player 2 is plating, set to true

bool commandExecuted = false; //if player executed the command or not
bool commandGiven = false; //is the command given

//flag for the inputs. if the input is give, flags will be set to true
volatile bool switchFlag = false;
volatile bool button1Flag = false;
volatile bool button2Flag = false;
volatile bool accelFlag = false;
volatile bool switchState = HIGH; 

void setup() {
  
  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(leftButtonPin, INPUT_PULLDOWN);
  pinMode(rightButtonPin, INPUT_PULLDOWN);

  CircuitPlayground.begin();
  Serial.begin(9600);
  while(!Serial); //waits for the code to be upploaded to the board and then prints everything in the serial monitor 
  CircuitPlayground.setAccelRange(LIS3DH_RANGE_8_G); //initiating the accelerometer

  attachInterrupt(digitalPinToInterrupt(switchPin), switchISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(leftButtonPin), button1ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(rightButtonPin), button2ISR, RISING);

}

void loop() {
  //monitors the switch flag and checks if player change happens. otherwise even with playerchange, it won't pring that player change happened
  //because mainGameFunction() only runs once
  if(!gamestart && switchFlag){
    playerChange();
    switchFlag = false;
  }
  
  //run the main game function when game start and game initialized is false. game start before actual game play
  if(!gamestart && !gameInitialized){
    mainGameFunction();
    gameInitialized = true;
  }
  //run the game start function is the gamestart flag is false. Funtionality before the main game starts
  if(!gamestart){
    gameStart();
    return;
  }
  //start the main game when gamestart flag is set to true
  if(gamestart){
    delay(1000);
    bopTwistPullit();
  }

}

//This function has the pre-game functionality such as switching the player, and showing the high scores
void mainGameFunction(){

  bool switchPos = digitalRead(switchPin);

  Serial.println("\nCurrent Player: ");
  if(switchPos == HIGH){
    Serial.println("Player 1");
  }
  else if(switchPos == LOW){
    Serial.println("Player 2");
  }

  Serial.println("\nPlayer 1 highscore:");
  Serial.println(player_1_highscore[0]);
  Serial.println("Player 2 highscore:");
  Serial.println(player_2_highscore[0]);

  Serial.println("Bop It(Purple) = Right Button");
  Serial.println("Pull It(blue) = Both Button");
  Serial.println("Shake It(Yellow) = Shake");


  Serial.println("\nUse the switch to decide who will be playing");

  Serial.println("\nPress both buttons to start game:");

  CircuitPlayground.setPixelColor(2, 225, 225, 225);
  CircuitPlayground.setPixelColor(7, 225, 225, 225);

  //when the player toggles the switch, run the playerChange function which has the functionality to change the player
  if(switchFlag){
    playerChange();
    switchFlag = false;
  }

}

//this function will contain the logic to give commands and get input. The main game functionality 
void bopTwistPullit(){
  //the game loops till gamestart flag is true. It will be set to false when the game ends, the the loop breaks and the game stops
  while(gamestart){

    Serial.println("Player Score: ");
    Serial.println(playerscore);
    //random number between 1 and 3 is generated. Each number is assigned to either bopit, pull it, or push it
    int randomNum = random(1,4);
    //delay before start
    delay(200);

    if(commandGiven == false && commandExecuted == false && randomNum == 1){
      bopIt();
      delay(speed);
    }
    else if(commandGiven == false && commandExecuted == false && randomNum == 2){
      shakeIt();
      delay(speed);
    }
    else if(commandGiven == false && commandExecuted == false && randomNum == 3){
      pullIt();
      delay(speed);
   }

  }

}

//function containing the bopIt functionality 
void bopIt() {
  //Pink color will be lit when bop it is called
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 255, 0, 180); // Pink color
  }
  CircuitPlayground.playTone(880, 150);

  //command given is set true
  commandGiven = true;
  commandExecuted = false;

  unsigned long startTime = millis(); //starts a timer in milliseconds since the command is given.
  //timer starts when while loop is started and subtracted from the startTime and compared to the delay speed
  //This checks if the players has given an input before the the command expires
  //initially, the player has 2 seconds to give an input
  while (millis() - startTime < speed) { 
    //if the right button is pressed, LED will light up green, playerscore will be increased by 1, "speed" delay will be lowered by 10%
    if (commandGiven && button1Flag) {
      CircuitPlayground.playTone(1000, 200);
      for (int i = 0; i < 10; i++) {
        CircuitPlayground.setPixelColor(i, 0, 225, 0);
      }
      delay(500);
      playerscore++;
      speed = max(speed * (1 - speedDecrementPercent / 100), minSpeed); // increment the speed by 10% as the score increases, capping at 800ms
      CircuitPlayground.clearPixels();
      button1Flag = false;
      commandGiven = false;
      return;
    }
    //if the left button is pressed which is the wrong input, game ends and restart function is called
    if (button2Flag) {
      CircuitPlayground.playTone(100, 150);
      for (int i = 0; i < 10; i++) {
        CircuitPlayground.setPixelColor(i, 225, 0, 0); 
      }
      delay(1500);
      CircuitPlayground.clearPixels();
      Serial.println("\nGame Ended!!");
      delay(500);
      restart();
      return;
    }
  }

  if (commandGiven && !button1Flag) {
    CircuitPlayground.playTone(100, 150);
    for (int i = 0; i < 10; i++) {
      CircuitPlayground.setPixelColor(i, 225, 0, 0); 
    }
    delay(3000);
    CircuitPlayground.clearPixels();
    Serial.println("\nGame Ended!!");
    delay(500);
    restart();
  }  
}


//function containing the shakeIt functionality
void shakeIt(){
  //yellow color for shake it
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 255, 225, 0); // Yellow Color
  }
  CircuitPlayground.playTone(784, 150);

  commandGiven = true;
  commandExecuted = false;

  unsigned long startTime = millis();
  while (millis() - startTime < speed) {

    //Calculate acceleration
    int X = 0;
    int Y = 0;
    int Z = 0;
    //X, Y, and Z positions are stored from the accelerometer reading
    for (int i=0; i<10; i++) {
      X += CircuitPlayground.motionX();
      Y += CircuitPlayground.motionY();
      Z += CircuitPlayground.motionZ();
      delay(1);
    }

    X /= 10;
    Y /= 10;
    Z /= 10;
    //total acceleration is calculated using the X, Y, and Z values
    int totalAccel = sqrt(X*X + Y*Y + Z*Z);
    //if the command is given and the total acceleration is between 15 and 65, count that as an input 
    if (commandGiven && (totalAccel > 15 && totalAccel < 65)) {
      accelFlag = true; 

      for (int i = 0; i < 10; i++) {
        CircuitPlayground.setPixelColor(i, 0, 225, 0);
      }
      CircuitPlayground.playTone(1000, 200);
      delay(500);
      playerscore++;
      speed = max(speed * (1 - speedDecrementPercent / 100), minSpeed);
      CircuitPlayground.clearPixels();
      button1Flag = false;
      commandGiven = false;
      return;
    }
    //added a grade period of 1.5 seconds so the game doesn't end right after shake it is called because it didn't detect motion
    //if the total acceleration is less than 10 and the time since the command given is expired, end the game
    if (commandGiven  && (millis() - startTime > speed) && totalAccel<10) {
      for (int i = 0; i < 10; i++) {
        CircuitPlayground.setPixelColor(i, 225, 0, 0); 
      }
      CircuitPlayground.playTone(100, 150);
      delay(3000);
      CircuitPlayground.clearPixels();
      Serial.println("\nGame Ended!!");
      delay(500);
      restart();
    } 

    //if any of the two buttons is pressed rather than shaked, end the game
    if (button2Flag || button1Flag) {
      for (int i = 0; i < 10; i++) {
        CircuitPlayground.setPixelColor(i, 225, 0, 0); 
      }
      CircuitPlayground.playTone(100, 150);
      delay(1500);
      CircuitPlayground.clearPixels();
      Serial.println("\nGame Ended!!");
      delay(500);
      restart();
      return;
    }
  }
}

//function containing pullIt functionality
void pullIt() {
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 31, 81, 225); // Neon blue
  }
  CircuitPlayground.playTone(523, 150);

  commandGiven = true;
  commandExecuted = false;

  unsigned long startTime = millis();

  //if both buttons are pressed before the timer runs out, count as an input
  while (millis() - startTime < speed) {
    if (button1Flag && button2Flag) {
      for (int i = 0; i < 10; i++) {
        CircuitPlayground.setPixelColor(i, 0, 225, 0); // Green
      }
      CircuitPlayground.playTone(1000, 200);
      delay(500);
      playerscore++;
      speed = max(speed * (1 - speedDecrementPercent / 100), minSpeed);
      CircuitPlayground.clearPixels();
      button1Flag = false;
      button2Flag = false;
      commandGiven = false;
      return;
    }
  }

  //if only one of the buttons is pressed, end the game
  if ((button1Flag && !button2Flag) || (!button1Flag && button2Flag) || (!button1Flag && !button2Flag)) {
    for (int i = 0; i < 10; i++) {
      CircuitPlayground.setPixelColor(i, 225, 0, 0); // Red
    }
    CircuitPlayground.playTone(100, 150);
    delay(3000);
    CircuitPlayground.clearPixels();
    Serial.println("\nGame Ended!!");
    delay(500);
    restart();
  }
}



//controls player switch
void playerChange(){
  //read the state of the switch
  bool state = digitalRead(switchPin);
  
  //if switch state is HIGH, thats player 1. Otherwisem its player 2
  if (state == HIGH) {
    player1 = true;
    player2 = false;
    Serial.println("\nSwitched to Player 1");
    Serial.println("\nPress both buttons to start game:");
  } else {
    player1 = false;
    player2 = true;
    Serial.println("\nSwitched to Player 2");
    Serial.println("\nPress both buttons to start game:");
  }
  delay(100);  // Small debounce delay
}

void gameStart(){
  //before the game starts, when both the buttons is pressed, set the gamestart flag to ture so the main game starts. Reset the button flags and clear the LEDs
  if(button1Flag && button2Flag){
    CircuitPlayground.playTone(1500, 150);
    gamestart = true;
    button1Flag = false;
    button2Flag = false;
    commandExecuted = false;
    CircuitPlayground.clearPixels();
  }
  //based on the switch state, set the corrosponding player flags to true
  bool switchState = digitalRead(switchPin);
  if(switchState == HIGH){
    player1 = true;
    player2 = false;
  } else {
    player1 = false;
    player2 = true;
  }
  
}

// //function containing game restart functionality
void restart(){
  //print the final score
  Serial.println("\nFinal Score:");
  Serial.print(playerscore);
  //sets high score based on which player played
  //based on which player is playing, store the playerscore to the corrosponding array
  if(player1){
    // Shift previous scores down
    //store a max of 4 values in the array
    //store the latest player score in the first index
    for (int i = 4; i > 0; i--) {
      //pushes the values back by 1 index. 
      player_1_highscore[i] = player_1_highscore[i - 1];
    }
    // Insert new score at the front
    player_1_highscore[0] = playerscore;
  }
  else if(player2){
    for(int i = 4; i > 0; i--){
      player_2_highscore[i] = player_2_highscore[i - 1];
    }
    player_2_highscore[0] = playerscore;
  }

 // Reset all relevant flags and state
  gamestart = false;
  gameInitialized = false;
  playerscore = 0;
  speed = 2000;

  commandGiven = false;
  commandExecuted = false;

  button1Flag = false;
  button2Flag = false;
  accelFlag = false;
  switchFlag = false;
}

void button1ISR(){
  if(button1Flag){
    button1Flag = false;
  }
  else{
    button1Flag = true;
  }
}

void button2ISR(){
  if(button2Flag){
    button2Flag = false;
  }
  else{
    button2Flag = true;
  }
}

void switchISR() {
  delay(5);
  switchFlag = !switchFlag;
}
