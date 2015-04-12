//Sample using LiquidCrystal library
#include <LiquidCrystal.h>
#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // twelve servo objects can be created on most boards
 
int pos = 0;    // variable to store the servo position 

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int buttonState=5;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
int buttonPressed=0;

const int DoorBellPin = 2;
int DoorBellState = 0; 

int lock = 0;
int unlock = 180;

bool timeout=0;
bool locked=1;


void setup()
{
 
 Serial.begin(9600);

 
 /*--------------------------------------------------*/
 myservo.attach(3);  // attaches the servo on pin 9 to the servo object 
 myservo.write(lock);
 pinMode(DoorBellPin, INPUT);
 
 lcd.begin(16, 2);              // start the library
 lcd.setCursor(0,0);  
 lcd.print("Door Pal  ");
 lcd.setCursor(0,1);  
 lcd.print("Initialized");
                                   Serial.println("DoorPal Initialized");
 delay(3000);
 lcd.clear();
}
 
boolean disconnectedMsg = false;
 
void loop()
{
  while (1){
      lcd.display();
      int endit=0;
      lcd.clear();
      while(endit==0){
        if (locked){
          if (Serial.available()){
            char inChar = (char)Serial.read();
            if(inChar == 'V'){ // end character for locking
                 if (!(myservo.read() >= 90)) {
                   lcd.setCursor(0,0); 
                   lcd.print("Door opening...   ");
                         Serial.println("Unlocking door");
                   myservo.write(unlock);
                   locked=0;
                   delay(3000);
                   lcd.setCursor(0,1); 
                   lcd.print("Welcome inside! ");
                       Serial.println("Welcome inside!!!");
                 }
               }
          }
        }
        if (!locked){
          if (Serial.available()){
            char inChar = (char)Serial.read();
            if(inChar == 'V'){ // end character for locking
               if (myservo.read() >= 90) {
                  lcd.setCursor(0,0);  
                  lcd.print("Locking door...");
                  Serial.println("Locking door...");
                  myservo.write(lock);
                  delay(3000);
                  locked=1;
                  lcd.setCursor(0,0);  
                  lcd.print("Your door is        ");
                  lcd.setCursor(0,1);  
                  lcd.print("locked.        ");
                  Serial.println("Your door is locked.");
                  delay(2000);
                  lcd.clear();                  
               }
            }
          }
        }
        
        readButtons();
        if (buttonPressed) {
          Serial.println("G");
          //sendEmail();
          delay(300);
          //SEND MESSAGE====================================
          lcd.setCursor(0,0);  
          lcd.print("Sorry, I'm not  ");
          lcd.setCursor(0,1);  
          lcd.print("home, but I am  ");
          delay(3000);
          lcd.setCursor(0,0);  
          lcd.print("being notified, ");
          lcd.setCursor(0,1);  
          lcd.print("Please wait...  ");
          delay(3000);
          lcd.clear();
          
                        Serial.println("Sorry, I'm not home, but I am being notified. Hold tight...");
          delay(1000);
          int t=millis();
          
          lcd.setCursor(0,0); 
          lcd.print("Waiting for  ");
          lcd.setCursor(0,1); 
          lcd.print("response...    ");       
          
                        Serial.println("Waiting for response...");
          delay(1000);
          do{
             // Recieve data from Node and write it to a String
             if (Serial.available()) {
               char inChar = (char)Serial.read();
               if(inChar == 'V'){ // end character for locking
                 if (!(myservo.read() >= 90)) {
                   lcd.setCursor(0,0); 
                   lcd.print("Door opening...   ");
                         Serial.println("Door opening...");
                   myservo.write(unlock);
                   locked=0;
                   delay(3000);
                   lcd.setCursor(0,1); 
                   lcd.print("Welcome inside! ");
                       Serial.println("Welcome inside!!!");
                 }
               }
             }
           if ((millis()-t)>=100000) timeout=1;
          }while((locked)&&(!(timeout)));
          lcd.clear();
          if (locked) {
            lcd.setCursor(0,0); 
            lcd.print("Sorry, come     ");
            lcd.setCursor(0,1); 
            lcd.print("back later.     ");
            Serial.println("Sorry, come back later.");
          }
          buttonPressed=0;
        }
        else if (buttonState==btnSELECT){
          endit=1;
        }
      }
      lcd.clear();
   }
}

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 
 if (adc_key_in > 1000) {
   return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 }
 // For V1.1 us this threshold
 if (adc_key_in < 50) {
   buttonPressed=1;
   //Serial.print("RIGHT");
   return btnRIGHT;  
 }
 if (adc_key_in < 150){  
   buttonPressed=1;
   //Serial.print("UP");
   return btnUP; 
 }
 if (adc_key_in < 300){  
   buttonPressed=1;
   //Serial.print("DOWN");
   return btnDOWN; 
 }
 if (adc_key_in < 500){  
   buttonPressed=1;
   //Serial.print("LEFT");
   return btnLEFT; 
 }
 if (adc_key_in < 700){ 
   buttonPressed=1;
   //Serial.print("SELECT");
   return btnSELECT;  
 }

 return btnNONE;  // when all others fail, return this...
}

void readButtons()
{
  int reading = read_LCD_buttons();
  
  buttonState = reading;
}
