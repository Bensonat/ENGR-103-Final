#include <Adafruit_CircuitPlayground.h>
#include <AsyncDelay.h>
#include <string>

float midi[127];
int A_four = 440; // a is 440 hz...


volatile int GameSelect = 0;
const int numGames = 2;
String NameSelect[numGames + 1] = {"Hot Cross Buns", "Test 1", "Test 2"};

volatile bool GameOn = false;

volatile bool PauseOn = false;

volatile bool ScoreRed;
volatile bool ScoreBlue;


int PlayArray[numGames + 1][50][2] = {
  {// 0=nothing, 1=red, 2=blue, 3=purple, 4=end
    {200, 0},
    {0, 127},
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
    {1, 0},
    {4, 127}
  },
  {
    {1, 0},
    {4, 127}
  }
};
bool GameOnState = true;
AsyncDelay BPM;
AsyncDelay ScoreTimer;
AsyncDelay MusicTimer;
volatile bool ScoredNow = false;

int PlayArrayIndex = 1;

int Score;
int ScoreMath;
int TotalScore = 0;

int RandRGB[3] = {0,0,0};

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();

  pinMode(4, INPUT_PULLDOWN);
  pinMode(5, INPUT_PULLDOWN);
  pinMode(7, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(4), RightPress, RISING);
  attachInterrupt(digitalPinToInterrupt(5), LeftPress, RISING);
  attachInterrupt(digitalPinToInterrupt(7), SlideChange, CHANGE);

  generateMIDI(); // Custom function for calculating and storing the frequency value associated for each MIDI pitch number

  BPM.start(10, AsyncDelay::MILLIS);
  GameSelectVisuals();
}

void RightPress(){
  delay(5);
  if(!GameOn){
    GameSelect += 1;

    if(GameSelect > numGames){
      GameSelect = 0;
    }
    GameSelectVisuals();
  }
  else{
    ScoreRed = true;

    if((PlayArray[GameSelect][PlayArrayIndex][0] == 1)){
      ScorePoints(1);
    }
    else if ((PlayArray[GameSelect][PlayArrayIndex][0] == 3) && (ScoreBlue == true)){
      ScorePoints(2);
    }

    delay(20);
    ScoreRed = false;

  }
}
void LeftPress(){
  delay(5);
  if(!GameOn){
    GameSelect -= 1;

    if(GameSelect < 0){
      GameSelect = numGames;
    }
    GameSelectVisuals();
  }
  else{
    ScoreBlue = true;

    if((PlayArray[GameSelect][PlayArrayIndex][0] == 2)){
      ScorePoints(1);
    }
    else if ((PlayArray[GameSelect][PlayArrayIndex][0] == 3) && (ScoreRed == true)){
      ScorePoints(2);
    }

    delay(20);
    ScoreBlue = false;
  }
}
void SlideChange(){
  delay(5);
  if(GameOn){
    PauseOn = !PauseOn;
  }
  else{
    GameOn = true;
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  if(GameOn){
    if(PlayArray[GameSelect][PlayArrayIndex + 1][0] == 4){//More elements in array?
      EndOfLevel();
    }
    else if (BPM.isExpired() && !PauseOn){//Main Meat, where the level happens.
      if(GameOnState){
        PlayArrayIndex += 1;
        ScoredNow = false;
        ScoreTimer.start(2 * PlayArray[GameSelect][0][0], AsyncDelay::MILLIS);
      }
      else{
        GamePlayVisuals();
        CircuitPlayground.playTone(PlayArray[GameSelect][PlayArrayIndex][1], 1.5 * PlayArray[GameSelect][PlayArrayIndex][0], false); 
      }
      BPM.start(PlayArray[GameSelect][0][0], AsyncDelay::MILLIS);
      GameOnState = !GameOnState;
    }
  }
}


void GamePlayVisuals(){
  for(int i = 0; i < 10; i++){
    SetLightPlay(i, PlayArray[GameSelect][PlayArrayIndex + i][0], 150);
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

void ScorePoints(int Mult){
  if (ScoredNow == false){
    ScoreMath = ScoreTimer.getDuration() - 4294960290;
    Score = (ScoreMath - PlayArray[GameSelect][0][0]);
    if(Score < 0){ Score = Score * -1; }
    Score += PlayArray[GameSelect][0][0];
    Score = Mult * map(Score, 0, PlayArray[GameSelect][0][0], 0, 1000);

    TotalScore += Score;
    Serial.println(ScoreTimer.getDuration());
    Serial.println(ScoreMath);

    Serial.print("Scored ");
    Serial.print(Score);
    Serial.print(" points! Total Score now at ");
    Serial.print(TotalScore);
    Serial.println("!");

    ScoredNow = true;
    SetLightPlay(0, PlayArray[GameSelect][PlayArrayIndex][0], 255);
  }
  else{
    Serial.println("Double Tap!");
  }
}

void GameSelectVisuals(){
  CircuitPlayground.clearPixels();

  for (int i = 0; i <= GameSelect; i++){
    CircuitPlayground.setPixelColor(i, 255, 255, 255);
  }

  Serial.print("Level ");
  Serial.print(GameSelect);
  Serial.print(": ");
  Serial.println(NameSelect[GameSelect]);
}

void EndOfLevel(){
  Serial.println("|");
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

  for (int i = 0; i < 4; i++){
    for (int k = 0; k < 10; k++){
      for(int h = 0; h < 3; h++){
        RandRGB[h] = random(255);
      }
      CircuitPlayground.setPixelColor(k, RandRGB[0], RandRGB[1], RandRGB[2]);
    }
    delay(200);
    for (int k = 0; k < 10; k++){
      CircuitPlayground.setPixelColor(k, 0, 0, 0);
    }
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