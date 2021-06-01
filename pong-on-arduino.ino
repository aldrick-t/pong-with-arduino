
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int RESET_BUTTON = 6;
const int UP_BUTTON = 2;
const int DOWN_BUTTON = 3;
const int SUP_BUTTON = 4;
const int SDOWN_BUTTON = 5;
const int BEEPER = 13;

const unsigned long LEDinterval = 100;
const unsigned long PADDLE_RATE = 1;
const unsigned long BALL_RATE = 1;
const uint8_t PADDLE_HEIGHT = 12;

int p1Score = 0;
int p2Score = 0;
const int maxScore = 10;
bool resetBall = false;
int LEDflash = 1;

const int SCREEN_W = 128;
const int SCREEN_H = 64;
const int OLED_RESET = -1;
Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, OLED_RESET);

void drawCourt();
void drawScore(); 

void pongIntro(){
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2){
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, SSD1306_INVERSE);
    display.display();
    delay(1);
  }
  display.fillScreen(BLACK);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(36,16);
  display.println("PONG!");
  display.setCursor(17,32);
  display.println("2 player");

  delay(200);
  display.display();
  delay(3000);   
}

unsigned long ball_update;
uint8_t ball_x = 64, ball_y = 32;
uint8_t ball_dir_x = 1, ball_dir_y = 1;
  
unsigned long paddle_update;

const uint8_t sPlayer_x = 12;
uint8_t sPlayer_y = 16;

const uint8_t player_x = 115;
uint8_t player_y = 16;
 
void setup(){  
    unsigned long start = millis();

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    pinMode(BEEPER, OUTPUT);
    pinMode(RESET_BUTTON, INPUT_PULLUP);
    pinMode(7,OUTPUT);
    pinMode(8,OUTPUT);
    pinMode(9,OUTPUT);

    pinMode(UP_BUTTON, INPUT);
    pinMode(DOWN_BUTTON, INPUT);
    digitalWrite(UP_BUTTON,1);
    digitalWrite(DOWN_BUTTON,1);

    pinMode(SUP_BUTTON, INPUT);
    pinMode(SDOWN_BUTTON, INPUT);
    digitalWrite(SUP_BUTTON,1);
    digitalWrite(SDOWN_BUTTON,1);

    ball_update = millis();
    paddle_update = ball_update;

    while(millis() - start < 3000);
      pongIntro();

    display.clearDisplay();
    delay(100);
    drawCourt();
    drawScore();
}
void drawCourt(){
  display.drawRect(0, 0, 128, 64, WHITE);
}
void drawScore(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(45, 1);
  display.println(p2Score);
  display.setCursor(75, 1);
  display.println(p1Score);
}
void player1LED(){
  for(int LEDflashP1=0; LEDflashP1<5; LEDflashP1=LEDflashP1+1){
    digitalWrite(8,HIGH);
    delay(75);
    digitalWrite(8,LOW);
    delay(75);
  }
}
void player2LED(){
  for(int LEDflashP2=0; LEDflashP2<5; LEDflashP2=LEDflashP2+1){
    digitalWrite(9,HIGH);
    delay(75);
    digitalWrite(9,LOW);
    delay(75);
  }
}
void greenLED(){
  for(LEDflash=0; LEDflash<5; LEDflash=LEDflash+1){
    digitalWrite(7,HIGH);
    delay(75);
    digitalWrite(7,LOW);
    delay(75);
  }
}
void gameOver(){ 
  display.fillScreen(BLACK);
  if(p1Score>p2Score){
    display.setCursor(20,4);
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.print("1st Won");

    player1LED();
  }
  else if (p2Score > p1Score){
    display.setCursor(20,4);
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.print("2nd WON");

    player2LED();    
  }
  else{
    display.setCursor(40,4);
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.print("TIED!");
    display.setCursor(1,46);
    display.setTextSize(1);
    display.println("RESET: New game...");
  }
  delay(200);
  display.display();
  delay(2000);
  p2Score = p1Score = 0;
  
  unsigned long start = millis();
  while(millis() - start < 2000);
  ball_update = millis();    
  paddle_update = ball_update;
  resetBall=true;
}
void soundBounce(){
  tone(BEEPER, 530, 50);
}
void soundPoint(){
  tone(BEEPER, 250, 50);
}
void loop() {
    unsigned long time = millis();
    bool update = false;

    static bool up_state = false;
    static bool down_state = false;
    static bool sup_state = false;
    static bool sdown_state = false;  

    if(digitalRead(RESET_BUTTON) == LOW){
        greenLED();
        LEDflash = 0;
        gameOver();
        
    }
    if(resetBall){
        if(p1Score == maxScore || p2Score == maxScore){
            gameOver();
        }
        else{      
        display.fillScreen(BLACK);
        drawScore();
        drawCourt();       
        ball_x = random(25,70); 
        ball_y = random(3,63);
        do{
            ball_dir_x = random(-1,2);
        }
        while(ball_dir_x==0);

        do{
            ball_dir_y = random(-1,2);
        }
        while(ball_dir_y==0);
      
      resetBall=false;
      }
    }

    up_state |= (digitalRead(UP_BUTTON) == LOW);
    down_state |= (digitalRead(DOWN_BUTTON) == LOW);
    sup_state |= (digitalRead(SUP_BUTTON) == LOW);
    sdown_state |= (digitalRead(SDOWN_BUTTON) == LOW);
   
    if(time > ball_update) {
        uint8_t new_x = ball_x + ball_dir_x;
        uint8_t new_y = ball_y + ball_dir_y;

        // Check if we hit the vertical walls
        if(new_x == 0 || new_x == 127) {
         
          
          if(new_x == 0){
            p1Score+=1;
            display.fillScreen(BLACK);
            soundPoint();
            resetBall = true;
            
          }
          else if(new_x == 127){
            p2Score+=1;
            display.fillScreen(BLACK);
            soundPoint();
            resetBall = true;
          }       
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

        // Check if we hit the horizontal walls.
        if(new_y == 0 || new_y == 63) {
            soundBounce();
            ball_dir_y = -ball_dir_y;
            new_y += ball_dir_y + ball_dir_y;
        }

        // Check if we hit the player 2 paddle
        if(new_x == sPlayer_x && new_y >= sPlayer_y && new_y <= sPlayer_y + PADDLE_HEIGHT){
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

        // Check if we hit the player paddle
        if(new_x == player_x && new_y >= player_y && new_y <= player_y + PADDLE_HEIGHT){
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

        display.drawPixel(ball_x, ball_y, BLACK);
        display.drawPixel(new_x, new_y, WHITE);
        ball_x = new_x;
        ball_y = new_y; 

        ball_update += BALL_RATE;

        update = true;
    }

    if(time > paddle_update) {
        paddle_update += PADDLE_RATE;

        // Player 2 paddle
        display.drawFastVLine(sPlayer_x, sPlayer_y, PADDLE_HEIGHT, BLACK);
        //const uint8_t half_paddle = PADDLE_HEIGHT >> 1;
        if(sup_state) {
            sPlayer_y -= 1;
        }
        if(sdown_state) {
            sPlayer_y += 1;
        }
        if(sPlayer_y < 1) sPlayer_y = 1;
        if(sPlayer_y + PADDLE_HEIGHT > 63) sPlayer_y = 63 - PADDLE_HEIGHT;
        display.drawFastVLine(sPlayer_x, sPlayer_y, PADDLE_HEIGHT, WHITE);
        sup_state = sdown_state = false;
        // Player paddle
        display.drawFastVLine(player_x, player_y, PADDLE_HEIGHT, BLACK);
        if(up_state) {
            player_y -= 1;
        }
        if(down_state) {
            player_y += 1;
        }
        if(player_y < 1) player_y = 1;
        if(player_y + PADDLE_HEIGHT > 63) player_y = 63 - PADDLE_HEIGHT;
        display.drawFastVLine(player_x, player_y, PADDLE_HEIGHT, WHITE);
        up_state = down_state = false;
    }
        update = true;
    
    if(update){
        display.display();
        }
}
