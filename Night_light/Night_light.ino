// Night lamp dimmer by Norbert Szep - 2018
#define buttonPin 2
#define ledPin 0
#define pcbLed 1
//#define DEBUG

bool counterRun = false;
int interval = 10;
int timerCounter = 0;
unsigned long previousMillis = 0;
boolean light = false;
boolean hold = false;
byte pwm = 60; // Defauld brightness
boolean pwmDirection = 0;
boolean rampReady = false;
// Heartbeat
byte pcbLedpwm = 0;
boolean upward = true;
// Timer
int forCounter = 0;
int alarmSec = 1800;  //30 min = 1800 sec
int currentSec = 0;
boolean alarmOn = false;


void setup() {
  Serial.begin(115200);
  Serial.println("START");
  pinMode(buttonPin,INPUT_PULLUP);
  pinMode(ledPin,OUTPUT);
  pinMode(pcbLed,OUTPUT);
  analogWrite(ledPin,0);
  analogWrite(pcbLed,pcbLedpwm);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (digitalRead(buttonPin) == LOW) counterRun = true;
    else {
      counterRun = false;
    }

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (counterRun == true) timerCounter++;
    // Brightness control
    if (light == true && hold == true && pwmDirection == 0) {
      if (pwm <= 254) pwm++;
    } else if (light == true && hold == true && pwmDirection == 1) {
      if (pwm > 4) pwm--;
    }
    // Ramping up the Brightness
    if (light == true && rampReady == false) {
      for (int a=0;a<pwm;a++) {
        analogWrite(ledPin,a);
        delay(5);
      }
      rampReady = true;
    }
    // Ramping down the Brightness
    if (light == false && rampReady == false) {
      for (int a=pwm ;a>1;a--) {
        analogWrite(ledPin,a);
        delay(3);
      }
      rampReady = true;
    }
    // Shutting on / off after ramping done
    if (light == false && rampReady == true) analogWrite(ledPin,0);
    if (light == true && rampReady == true) analogWrite(ledPin,pwm);
    ////////////////////////////
    //// BUILTIN LED ///////////
    if (upward) pcbLedpwm+=3;
      else pcbLedpwm-=3;
    if (pcbLedpwm > 250) upward = false;
    if (pcbLedpwm < 1) upward = true;
    if (alarmOn == true) analogWrite(pcbLed,pcbLedpwm);
      else analogWrite(pcbLed,0);
    ////////////////////////////
    /// Second counter for Alarm
    if (alarmOn == true) forCounter++;
    if (forCounter >= 100) {
      currentSec++;
      forCounter = 0;
    }
    ////////////////////////////
  }

  // short push
  if (digitalRead(buttonPin) == HIGH && timerCounter < 70 && timerCounter > 5 ) {
    if (light == false) {
      if (pwm >150) pwmDirection = 1;
      light = true;
      rampReady = false;
      }
      else {
        pwmDirection = 0;
        light = false;
        alarmOn = false;
        currentSec = 0;
        rampReady = false;
      }
    #if defined(DEBUG)
      Serial.println("SHORT PUSH");
      Serial.print("timerCounter: ");Serial.println(timerCounter);
      Serial.print("Light: ");Serial.println(light);
      Serial.print("PWM:");Serial.println(pwm);
    #endif
    timerCounter = 0;
  }

  // hold button start
  if (digitalRead(buttonPin) == LOW && timerCounter > 70) {
    hold = true;
  } else {
    hold = false;
  }
  // hold button end
  if (digitalRead(buttonPin) == HIGH && timerCounter > 70) {
    timerCounter = 0;
    if (pwmDirection == 0) pwmDirection = 1;
      else pwmDirection = 0;
    #if defined(DEBUG)
      Serial.println("HOLD END");
      Serial.print("PWM:");Serial.println(pwm);
    #endif
  }
  // Alarm set
  if (light == false && hold == true && timerCounter >= 150) {
    alarmOn = true;
    light = true;
    rampReady = false;
  }
  if (currentSec >= alarmSec && alarmOn == true) {
    alarmOn = false;
    currentSec = 0;
    light = false;
    rampReady = false;
  }
  
}
