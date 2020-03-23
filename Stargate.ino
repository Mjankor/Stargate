/**
 * StepDuino example
 */
/**
 * Demo of writing to the StepDuino LCD
 */
#include <Wire.h>
#include <LiquidCrystal_SR_LCD3.h>

int TotalStages = 6;
int TotalHZSteps = 40;

const int PIN_LCD_STROBE         =  4;  // Out: LCD IC4094 shift-register strobe
const int PIN_LCD_DATA           =  7;  // Out: LCD IC4094 shift-register data
const int PIN_LCD_CLOCK          =  8;  // Out: LCD IC4094 shift-register clock

LiquidCrystal_SR_LCD3 lcd(PIN_LCD_DATA, PIN_LCD_CLOCK, PIN_LCD_STROBE);

const int Stepper1Step      = 5;
const int Stepper1Direction = 2;
const int StepsPerRev1      = 2048;
const int TotalSteps1     = 16384;

const int Stepper2Step      = 6;
const int Stepper2Direction = 3;
const int StepsPerRev2      = 1600;
const int TotalSteps2        = 21000;
const int StartupSteps2       = 3000;
const int EndUnusedSteps2     = 2000;

int StepsPerStage = 0;
int StepsPerRotate = 0;
float TotalShots = 0;
float ShotCounter = 0;

const int LimitSwitch = 16;
const int CameraShutter = 14; //White
const int CameraFocus = 15; //Yellow

const int  HorizontalDelay = 500;
const int VerticalDelay = 2000;
const int ExposureDelay = 200;

bool resettoggle = HIGH;//tipping bucket variables
bool MissionComplete = LOW;
/**
 * Set pin assignments
 */
void setup() {
  pinMode(Stepper1Step,      OUTPUT);
  pinMode(Stepper1Direction, OUTPUT);
  
  pinMode(Stepper2Step,      OUTPUT);
  pinMode(Stepper2Direction, OUTPUT);
//camera
  pinMode(CameraShutter, OUTPUT);

      lcd.begin( 20, 4 );               // Initialize the LCD 
    lcd.home ();                      // Go to the home location
    lcd.setCursor (0, 0);
    lcd.print(F("** Stepduino v1.1 **"));  
    lcd.setCursor (0, 1);
    lcd.print(F("Dual channel stepper"));
    lcd.setCursor (0, 2);
    lcd.print(F("motor +servo outputs"));
    lcd.setCursor (0, 3);
    lcd.print(F(" freetronics.com/sd"));
    delay( 500 );
    lcd.setCursor( 0, 0 );
    lcd.print(F("                    "));
    lcd.setCursor( 0, 1 );
    lcd.print(F("                    "));
    lcd.setCursor( 0, 2 );
    lcd.print(F("                    "));
    lcd.setCursor( 0, 3 );
    lcd.print(F("                    "));

    //Reset the position of the main rotator
    //Set pin active first
    pinMode(LimitSwitch, INPUT_PULLUP);
    ResetPosition();
    lcd.setCursor( 0, 0 );
    lcd.print(F("                    "));
    lcd.setCursor( 0, 0 );
    lcd.print(F("Moving to stage 1"));
    stepper2GoRevSteps (StartupSteps2);
    //Move to start position
    //small pause to remove vibrations
    delay (5000);

    //count TotalShots
    TotalShots = TotalStages*TotalHZSteps;
    //Steps Per stage 
    if (TotalStages == 1){
    StepsPerStage = 0;
    } else {
    StepsPerStage = (TotalSteps2 - StartupSteps2 - EndUnusedSteps2) / (TotalStages-1);
    }
    //Steps per rotation
    StepsPerRotate = 16384 / TotalHZSteps;
    lcd.setCursor( 0, 0 );
    lcd.print(F("                    "));
    lcd.setCursor( 0, 0 );
    lcd.print(F("Beginning scan."));
}

/**
 * Main loop
 */
void loop() {
  if (MissionComplete == LOW){
    lcd.setCursor( 0, 0 );
    lcd.print(F("                    "));
    lcd.setCursor( 0, 0 );
    lcd.print(F("Scanning."));
    lcd.setCursor( 0, 1 );
    lcd.print(F("                    "));
    lcd.setCursor( 0, 1 );
    lcd.print(F("Percent."));
    lcd.setCursor( 0, 3 );
    lcd.print(F("Stage: "));
    lcd.setCursor( 6, 3 );
    lcd.print(1);    
    
    HorizontalCycle();
    for(int i = 1; i < TotalStages; i++){
    stepper2GoRevSteps (StepsPerStage);
    lcd.setCursor( 6, 3 );
    lcd.print(i+1);
    HorizontalCycle();
    }
    
    //return to start
    resettoggle = HIGH;
    ResetPosition();
    lcd.setCursor( 0, 0 );
    lcd.print(F("                    "));
    lcd.setCursor( 0, 1 );
    lcd.print(F("                    "));
    lcd.setCursor( 0, 2 );
    lcd.print(F("                    "));
    lcd.setCursor( 0, 3 );
    lcd.print(F("                    "));
    lcd.setCursor( 0, 0 );
    lcd.print(F("Ready for new scan"));
    MissionComplete = HIGH;
  }
    
}

/**
 * Rotate stepper 1 forward by 1 step
 */
void stepper1Forward()
{
  digitalWrite(Stepper1Direction, HIGH);
  digitalWrite(Stepper1Step, HIGH);
  delayMicroseconds(100); // 1uS minimum pulse duration for DRV8811
  digitalWrite(Stepper1Step, LOW);
  delayMicroseconds(4900);
}

/**
 * Rotate stepper 1 backwards by 1 step
 */
void stepper1Reverse()
{
  digitalWrite(Stepper1Direction, LOW);
  digitalWrite(Stepper1Step, HIGH);
  delayMicroseconds(100);
  digitalWrite(Stepper1Step, LOW);
  delayMicroseconds(4900);
}

/**
 * Rotate stepper 1 forward by 1 step
 */
void stepper2Forward()
{
  digitalWrite(Stepper2Direction, HIGH);
  digitalWrite(Stepper2Step, HIGH);
  delayMicroseconds(2); // 1uS minimum pulse duration for DRV8811
  digitalWrite(Stepper2Step, LOW);
  delayMicroseconds(2000);
}

void stepper2Reverse()
{
  digitalWrite(Stepper2Direction, LOW);
  digitalWrite(Stepper2Step, HIGH);
  delayMicroseconds(2); // 1uS minimum pulse duration for DRV8811
  digitalWrite(Stepper2Step, LOW);
  delayMicroseconds(2000);
}

void stepper2GoRevSteps(int StepsToMove){
for(int i = 0; i < StepsToMove; i++)
  {
    stepper2Reverse();
  }
}

void stepper1GoSteps(int StepsToMove){
for(int i = 0; i < StepsToMove; i++)
  {
    stepper1Forward();
  }
}

void HorizontalCycle (){
  delay (VerticalDelay);
    for(int i = 0; i < TotalHZSteps; i++){
        stepper1GoSteps(StepsPerRotate);
        delay( HorizontalDelay ); //in case any movement in object
        FireShutter();
        delay( ExposureDelay ); //wait in case of long exposure
        ShotCounter++;
        lcd.setCursor( 0, 2 );
        float percent = (ShotCounter * 100)/TotalShots;
        lcd.print(percent);
        }
    }  

void FireShutter(){
{
digitalWrite (CameraShutter, HIGH);
  delay(100);
digitalWrite(CameraShutter, LOW);
}
}

/**
 * Lock both steppers in place
 */
void applyBrakes()
{
  digitalWrite(Stepper1Direction, LOW);
  digitalWrite(Stepper1Step, LOW);
  digitalWrite(Stepper2Direction, LOW);
  digitalWrite(Stepper2Step, LOW);
  delayMicroseconds(50);
} 

void ResetPosition(){
  //Serial.print(F("Checking limit switch. Result = "));
  //Serial.println(digitalRead(LimitSwitch));
if (resettoggle == HIGH){
    lcd.setCursor( 0, 0 );
    lcd.print(F("Setting start posn."));
while (digitalRead(LimitSwitch) == HIGH){
      stepper2Forward();
        if (digitalRead(LimitSwitch) == LOW){
          resettoggle = LOW;
        break;
       }
      }
    }
    Serial.println(F("Photorig Reset."));
    lcd.setCursor( 0, 0 );
    lcd.print(F("                    "));
    lcd.setCursor( 0, 0 );
    lcd.print(F("Position reset."));
    delay( 1000 );
   
}
