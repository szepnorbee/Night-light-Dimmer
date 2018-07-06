// Night lamp dimmer by Norbert Szep - 2018
#define buttonPin A2
#define ledPin 6
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

void setup() {
  Serial.begin(115200);
  Serial.println("START");
  pinMode(buttonPin,INPUT_PULLUP);
  pinMode(ledPin,OUTPUT);
  analogWrite(ledPin,0);
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
    // Shutting on=off after ramping done
    if (light == false && rampReady == true) analogWrite(ledPin,0);
    if (light == true && rampReady == true) analogWrite(ledPin,pwm);
    ////////////////////////////
  }

  // short push
  if (digitalRead(buttonPin) == HIGH && timerCounter < 70 && timerCounter > 5) {
    if (light == false) {
      if (pwm >150) pwmDirection = 1;
      light = true;
      rampReady = false;
      }
      else {
        pwmDirection = 0;
        light = false;
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
  
}
