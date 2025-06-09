#include <Adafruit_CircuitPlayground.h>
#include <AsyncDelay.h>
#include <string>

float midi[127];
int A_four = 440; // a is 440 hz...


volatile int GameSelect = 0;
const int numGames = 2; // input the total number of playable levels here, counting 0.
String NameSelect[numGames + 1] = {"Hot Cross Buns", "Lost Lady Found", "Test 1"}; // name the playable levels

volatile bool GameOn = false; // Are we in a game orin the menu?

volatile bool PauseOn = false; //Should the game be paused?

volatile bool ScoreRed; //Used for purple scoring
volatile bool ScoreBlue;

int x = 0;

int PlayArray[numGames + 1][110][2] = {
  //First level array is for which level we're in, second is for what note in that game we're on, and third specifies the light color or midi note
  // 0=nothing, 1=red, 2=blue, 3=purple, 4=end
  {
    {100, 0},  // First element in every level array is dedicated to information, [level][0][0] is the BPM for that level
    {0, 127},  // first element specifies color, 0 for nothing, and 127 for no note.
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {1, 62},
    {0, 127},
    {2, 60},
    {0, 127},
    {3, 58},
    {0, 127},
    {0, 127},
    {0, 127},
    {1, 62},
    {0, 127},
    {2, 60},
    {0, 127},
    {3, 58},
    {0, 127},
    {0, 127},
    {0, 127},
    {1, 58},
    {1, 58},
    {1, 58},
    {1, 58},
    {2, 60},
    {2, 60},
    {2, 60},
    {2, 60},
    {1, 62},
    {0, 127},
    {2, 60},
    {0, 127},
    {3, 58},
    {0, 127},
    {0, 127},
    {4, 127}
  },
  {
    {68, 0},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 58}, // start A section
    {0, 127},
    {1, 60},
    {0, 127},
    {1, 60},
    {0, 127},
    {3, 62},
    {0, 127},
    {2, 58},
    {0, 127},
    {2, 58},
    {0, 127},
    {3, 60},
    {0, 127}, // end A section
    {1, 67},
    {0, 127},
    {1, 67},
    {2, 65},
    {1, 62},
    {2, 63},
    {3, 60},
    {0, 127},
    {0, 127},
    {0, 127}, // end B section
    {0, 58}, // start A section
    {0, 127},
    {1, 60},
    {0, 127},
    {1, 60},
    {0, 127},
    {3, 62},
    {0, 127},
    {2, 58},
    {0, 127},
    {2, 58},
    {0, 127},
    {3, 60},
    {0, 127}, // end A section
    {1, 67},
    {0, 127},
    {2, 70},
    {1, 69},
    {2, 67},
    {1, 65},
    {3, 67},
    {0, 127},
    {0, 127}, // end C section
    {0, 65},
    {0, 65},
    {1, 67},
    {0, 127},
    {1, 67},
    {0, 127},
    {3, 72},
    {0, 127},
    {2, 67},
    {0, 127},
    {2, 67},
    {0, 127},
    {3, 69},
    {0, 127},
    {1, 65},
    {0, 127},
    {1, 65},
    {0, 127},
    {2, 62},
    {0, 127},
    {1, 60},
    {0, 127},
    {0, 127},
    {0, 127}, // end D section
    {0, 58}, // start A section
    {0, 127},
    {1, 60},
    {0, 127},
    {1, 60},
    {0, 127},
    {3, 62},
    {0, 127},
    {2, 58},
    {0, 127},
    {2, 58},
    {0, 127},
    {3, 60},
    {0, 127}, // end A section
    {1, 67},
    {0, 127},
    {1, 67},
    {2, 65},
    {1, 62},
    {2, 63},
    {3, 60},
    {0, 127},
    {0, 127},
    {0, 127}, // end B section
    {4, 127}
  },
  {
    {1, 0}, // empty level
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {0, 127},
    {4, 127}
  }
};
bool GameOnState = true; //used for the alternating nature of the loop
AsyncDelay BPM;

int ScoreTimer;

volatile bool ScoredNow = false;

AsyncDelay RedTimer;
AsyncDelay BlueTimer;

int PlayArrayIndex = 1; //specifies which element in the play array we're on

int Score;
int TotalScore = 0;

int RandRGB[3] = {0,0,0};

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();

  pinMode(4, INPUT_PULLDOWN); // Set up the input buttons
  pinMode(5, INPUT_PULLDOWN);
  pinMode(7, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(4), RightPress, RISING); // set up the intterupts
  attachInterrupt(digitalPinToInterrupt(5), LeftPress, RISING);
  attachInterrupt(digitalPinToInterrupt(7), SlideChange, CHANGE);

  generateMIDI(); // Custom function for calculating and storing the frequency value associated for each MIDI pitch number

  BPM.start(10, AsyncDelay::MILLIS); // make it so that BPM has exired by the time we start.
  GameSelectVisuals(); // Start the visuals up before the player touches anything
}

void RightPress(){
  if(!GameOn){ // When in the menu
    GameSelect += 1; // move to the next higher level, looping around at max

    if(GameSelect > numGames){
      GameSelect = 0;
    }
    GameSelectVisuals(); // Update visuals
  }
  else{
    ScoreRed = true;

    if((PlayArray[GameSelect][PlayArrayIndex][0] == 1)){ // If the current scoring element is red, score
      ScorePoints(1);
    }
    else if ((PlayArray[GameSelect][PlayArrayIndex][0] == 3) && (ScoreBlue == true)){ // If the current scoring element is purple and ScoreBlue was pressed in the last 20ms, score double
      ScorePoints(2);
    }

    RedTimer.start(20, AsyncDelay::MILLIS); // Set up the 20ms timer for purple scoring
  }
}
void LeftPress(){
  if(!GameOn){
    GameSelect -= 1; //move to the next lower level, looping around at min

    if(GameSelect < 0){
      GameSelect = numGames;
    }
    GameSelectVisuals(); // Update visuals
  }
  else{
    ScoreBlue = true;

    if((PlayArray[GameSelect][PlayArrayIndex][0] == 2)){ // If the current scoring element is blue, score
      ScorePoints(1);
    }
    else if ((PlayArray[GameSelect][PlayArrayIndex][0] == 3) && (ScoreRed == true)){ // If the current scoring element is purple and ScoreRed was pressed in the last 20ms, score double
      ScorePoints(2);
    }

    BlueTimer.start(20, AsyncDelay::MILLIS); // Set up the 20ms timer for purple scoring
  }
}
void SlideChange(){
  delay(5); // Time to process
  if(GameOn){ // If in a level, then set the pause state to whichever it wasn't previously.
    PauseOn = !PauseOn;
  }
  else{ // If in menu, start the game that was selected.
    GameOn = true;
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  if(GameOn){ //Only do things here if we are in a level, nothign happens in menu
    if(PlayArray[GameSelect][PlayArrayIndex + 1][0] == 4){// If the next element is 4, end the level.
      EndOfLevel();
    }
    else if (BPM.isExpired() && !PauseOn){//Main Meat, where the level happens. This only occors if the timer has expired and the pause isn't off, stoppoing the game until these two conditions are met
      if(GameOnState){ // loop works in an on-off way, every time it passes through, it does the one that didn't happen last time. On the up beat, it moves the current element along, allows it to be scored, and starts the timer for scoring
        PlayArrayIndex += 1; 
        ScoredNow = false;
        ScoreTimer = 0;
      }
      else{ // On the down beat it updates the visuals and sound
        GamePlayVisuals();
        CircuitPlayground.playTone(midi[PlayArray[GameSelect][PlayArrayIndex][1]], 1.5 * PlayArray[GameSelect][0][0], false); 
      }
      BPM.start(PlayArray[GameSelect][0][0], AsyncDelay::MILLIS); // Set the timer for the next note, depends on [level][0][0]
      GameOnState = !GameOnState; // Move to the other statae for the next loop
    }

    ScoreTimer += 1; // Move the timer along, up one per 1 ms
    delay(1);

    if(RedTimer.isExpired()){ // IF the timer has exired, then stop the possibility for purple scoring.
      ScoreRed = false;
    }
    if(BlueTimer.isExpired()){
      ScoreBlue = false;
    }
  }
}


void GamePlayVisuals(){
  for(int i = 0; i < 10; i++){
    SetLightPlay(i, PlayArray[GameSelect][PlayArrayIndex + i][0], 150); //Set each pixel to their corosponding color based on what type they are and how far along in the playarray we are
  }
}
void SetLightPlay(int Pixel, int Type, int Brightness){
  if(Type == 0){
    CircuitPlayground.setPixelColor(Pixel, 0, 0, 0);
  }
  else if(Type == 1){
    CircuitPlayground.setPixelColor(Pixel, Brightness, 0, 0);
  }
  else if(Type == 2){
    CircuitPlayground.setPixelColor(Pixel, 0, 0, Brightness);
  }
  else if(Type == 3){
    CircuitPlayground.setPixelColor(Pixel, Brightness, 0, Brightness);
  }
  else{
    CircuitPlayground.setPixelColor(Pixel, 255, 255, 255); 
  }
}

void ScorePoints(int Mult){ // Calculate who points are scored.
  if (ScoredNow == false){ // ONly happens if the element has not been scored before.

    // Do the math so that the function for how points are scored is an upwards facing triange, with the max being exactly the BPM timer, or half of the time between notes.
    Score = (ScoreTimer - PlayArray[GameSelect][0][0]); // Subtract by half the max, putting the lower half in the negitives.
    if(Score < 0){ Score = Score * -1; } // If score is greater than 0, reverse it, making the triange in the negitives with a peak at 0 and min at BPM
    Score += PlayArray[GameSelect][0][0]; // Add BPM, creating the math we want
    Score = Mult * map(Score, 0, PlayArray[GameSelect][0][0], 0, 1000); // Standardize scores.

    TotalScore += Score;
    //Serial.println(ScoreTimer); // this was here for debugging

    Serial.print("Scored "); // Print out the points scored and wat the new point total is.
    Serial.print(Score);
    Serial.print(" points! Total Score now at ");
    Serial.print(TotalScore);
    Serial.println("!");

    ScoredNow = true; // make sure this element can't be scored again
    SetLightPlay(0, PlayArray[GameSelect][PlayArrayIndex][0], 255); // make the newly scored point light up a bit, not that noticeable.
  }
  else{
    Serial.println("Double Tap!"); // If you try to score a scored point, tell the player that they can't do that
  }
}

void GameSelectVisuals(){
  CircuitPlayground.clearPixels(); // blank slate

  for (int i = 0; i <= GameSelect; i++){
    CircuitPlayground.setPixelColor(i, 255, 255, 255); // set set the pixels on to show what you're on.
  }

  Serial.print("Level "); // print the name
  Serial.print(GameSelect);
  Serial.print(": ");
  Serial.println(NameSelect[GameSelect]);
}

void EndOfLevel(){
  Serial.println("|"); // print total points
  Serial.println("|");
  Serial.println("|");
  Serial.println("|");
  Serial.print("You scored ");
  Serial.print(TotalScore);
  Serial.println(" points in total!");
  Serial.println("|");
  Serial.println("|");
  Serial.println("|");
  Serial.println("|");

  for (int i = 0; i < 4; i++){ // play celebration, four flashes of random colors on each pixel
    for (int k = 0; k < 10; k++){
      for(int h = 0; h < 3; h++){
        RandRGB[h] = random(255);
      }
      CircuitPlayground.setPixelColor(k, RandRGB[0], RandRGB[1], RandRGB[2]);
    }
    delay(200);
    CircuitPlayground.clearPixels();
    delay(200);
  }

  //Reset
  PlayArrayIndex = 1;
  GameOn = false;
  TotalScore = 0;
  //Visuals
  GameSelect += 1;

  if(GameSelect > numGames){
    GameSelect = 0;
  }
  GameSelectVisuals();
}

void generateMIDI(){
  for (int x = 0; x < 127; ++x)
  {
    midi[x] = (A_four / 32.0) * pow(2.0, ((x - 9.0) / 12.0));
    Serial.println(midi[x]);
  }
}
