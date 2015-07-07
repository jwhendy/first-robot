#include <Servo.h>

class Enc
{

	int enc_pin_A = 2;
	int enc_pin_B = 4;
	int encPos;
	unsigned long encInterval;
	unsigned long encLastUpdate;
	unsigned char enc_A;
	unsigned char enc_B;
	unsigned char enc_A_prev;
	int inc;

  public:
  Enc(int interval)
	{
		encInterval = interval;
    encLastUpdate = 0;
		inc = 5;
	}

	int Update(int pos)
	{
    encPos = pos;
	
  	if((millis() - encLastUpdate) > encInterval)
		{

			encLastUpdate = millis();

			// Read encoder pins
			enc_A = digitalRead(enc_pin_A);    
  		enc_B = digitalRead(enc_pin_B);   

   		if((!enc_A) && (enc_A_prev))
   		{
     		// A has gone from high to low 
   			if(enc_B)
   			{
       		// B is high so clockwise
     			if(encPos + inc <= 155) 
          {
            encPos += inc;
          }
          else
          {
            encPos = 155;
          }
   			}	   
      			
  			else
   			{
      		// B is low so counter-clockwise      
     			if(encPos - inc >= 25)
          {
            encPos -= inc;
          }
          else
          {
            encPos = 25;
          }
   			}   
   		}   
    
   		enc_A_prev = enc_A;     // Store value of A for next time  
      Serial.println(encPos);
		}

   		return(encPos);
	}
};

class Sweeper
{
  Servo servo; 
  int increment;        // increment to move for each interval
  int  updateInterval;      // interval between updates
  unsigned long lastUpdate; // last update of position
 
  public: 
  Sweeper(int interval)
  {
    updateInterval = interval;
  }
  
  void Attach(int pin)
  {
    servo.attach(pin);
  }
  
  void Detach()
  {
    servo.detach();
  }
  
  void Update(int pos)
  {
    if((millis() - lastUpdate) > updateInterval)  // time to update
    {
      lastUpdate = millis();

      servo.write(pos);
      
    }
  }
};
 
class Heart
{

  int heartPin; 
  int onTime;
  int ledState;

  unsigned long previousMillis;
 
  public: 
  Heart(int pin)
  {
    heartPin = pin;
    pinMode(heartPin, OUTPUT);
    analogWrite(heartPin, 255);

    ledState = 0;
    previousMillis = 0;
    onTime = 40;
  }
  
  void Update(int off, int brightness)
  {
   
    unsigned long currentMillis = millis();

    // it's off and been off long enough
    if((ledState == 0) && (currentMillis - previousMillis >= off))  // time to update
    {
      
      analogWrite(heartPin, brightness);
      previousMillis = currentMillis;
      ledState = 1;

    }

    // it's off and been off long enough
    if((ledState == 1) && (currentMillis - previousMillis >= onTime))  // time to update
    {
      
      analogWrite(heartPin, 255);
      previousMillis = currentMillis;
      ledState = 0;
      
    }

  }
};

class Temp
{

  int tmpPin; 
  float tmp_val;
  unsigned long previousMillis;
  unsigned long updateInterval;
 
  public: 
  Temp(int interval)
  {
    previousMillis = 0;
    updateInterval = interval;
    tmpPin = 2;
  }
    
  float Update()
  {
    unsigned long currentMillis = millis();

    if(currentMillis - previousMillis > updateInterval) {
      tmp_val = analogRead(tmpPin);
      tmp_val = tmp_val * 5.0/1024;
      tmp_val = (((tmp_val - .5) * 100) * (9.0/5.0)) + 32;
      previousMillis = currentMillis;
      //Serial.println(tmp_val);
    }

    return(tmp_val);
  }
};

class LightEye
{

  int light_pin = 0;
  int light_val;
  int eye_pin = 3; 
  int eye_val;
  unsigned long previousMillis;
  unsigned long updateInterval;
  int eye_state;
  unsigned long lastOn;
 
  public: 
  LightEye(int interval)
  {
    pinMode(eye_pin, OUTPUT);
    digitalWrite(eye_pin, 0);
    previousMillis = 0;
    updateInterval = interval;
    int eye_state = 0;

  }
    
  void Update()
  {
    unsigned long currentMillis = millis();
    
    if(currentMillis - previousMillis > updateInterval) {
      light_val = analogRead(light_pin);
      //Serial.println(light_val);
      if ((eye_state == 0) && (light_val < 200)) {
        digitalWrite(eye_pin, 1);
        lastOn = currentMillis;
        eye_state = 1;
      }

      if((eye_state == 1) && (currentMillis - lastOn > 10000)) {
        digitalWrite(eye_pin, 0);
        eye_state = 0;
      }

      previousMillis = currentMillis;

    }
    
  }
};

class PresSpkr
{

  int pres_pin = 1;
  int pres_val;
  int spkr_pin = 5; 
  int spkr_val;
  unsigned long previousMillis;
  unsigned long updateInterval;
 
  public: 
  PresSpkr(int interval)
  {
    pinMode(spkr_pin, OUTPUT);
    previousMillis = 0;
    updateInterval = interval;
  }
    
  void Update()
  {
    unsigned long currentMillis = millis();

    if(currentMillis - previousMillis > updateInterval) {
      pres_val = analogRead(pres_pin);
      //Serial.println(pres_val);
      previousMillis = currentMillis;
      
      if (pres_val > 10) {
        tone(spkr_pin, map(pres_val, 10, 1000, 220, 1550));
      }

      else {
        noTone(spkr_pin);
      } 
    }
  }
};

Sweeper sweeper1(10);
Enc enc1(4);
Heart heart1(6);
Temp temp1(50);
PresSpkr presSpeak1(100);
LightEye lightEye1(100);

int pos = 90;
int newPos = 90;
float currTemp;
int ledBright;
int offTime;

void setup() 
{ 
  Serial.begin(9600);
  sweeper1.Attach(9);
} 
 
 
void loop() 
{ 
  
  if(newPos != pos) {
    sweeper1.Update(pos);
    pos = newPos;
  }

  newPos = enc1.Update(pos);

  currTemp = temp1.Update();
  ledBright = constrain(map(currTemp, 70, 80, 254, 50), 50, 250);
  offTime = constrain(map(currTemp, 65, 80, 4000, 20), 20, 3000);
  heart1.Update(offTime, ledBright);

  presSpeak1.Update();
  lightEye1.Update();


  
}