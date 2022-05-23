//initalize servos
#define LeftSensor 47
#define MiddleSensor 51
#define RightSensor 52
const int t = 40;
#include<Servo.h>
Servo leftServo;
Servo rightServo;

const int TxPin= 7;
#include <Wire.h>
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
#include <SoftwareSerial.h>
SoftwareSerial mySerial = SoftwareSerial(255,TxPin);
#include "Adafruit_TCS34725.h"
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

int redR, redG, redB;
int yellowR, yellowG, yellowB;
int greenR, greenG, greenB;
int blueR, blueG, blueB;
int magentaR, magentaG, magentaB;
int greyR, greyG, greyB;
const int tolerance = 100;

int data[128];
int sum = 0;
int j = 0;

String grandChallenge[7] = {"Nitro", "Heal ", "Medic", "VR   ", "Sci  ", "Brain", "Learn"};

void setup() {
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  Serial2.begin(9600);
  //Left & Right Servo
  leftServo.attach(12);
  rightServo.attach(11);

  //Color sensor
  tcs.begin();
  setRed(797,198,189);
  setYellow(1288,1132,505);
  setGreen(605,798,395);
  setBlue(288,340,421);
  setMagenta(588,314,384);
  setGrey(790,774,651);
  
  //Temp sensor
  mlx.begin();
  
  //LCD screen
  pinMode(TxPin, OUTPUT);
  digitalWrite(TxPin, HIGH);
  mySerial.begin(9600);

  delay(1000);
  lap();
  
  leftServo.detach();
  rightServo.detach();
  
  detectTemp();
  detectColour();
}

void loop() {
  process();
}

void detectColor(){
  uint16_t clear, red, green, blue;
  delay(1000);
  tcs.getRawData(&red, &green, &blue, &clear);

  if(green < 250 && blue < 250){
    mySerial.write(138);
    delay(5);
    mySerial.print("Red");
  } else if (clear < 950 && clear < 1150){
    mySerial.write(138);
    delay(5);
    mySerial.print("Blue");
  } else if (clear > 1450 && clear < 1680 && red < 550 && blue < 350){
    mySerial.write(138);
    delay(5);
    mySerial.print("Green");
  } else if (clear > 1050 && clear < 1350 && red < 650 && red > 520){
    mySerial.write(138);
    delay(5);
    mySerial.print("Purple");
  } else if (clear > 2750){
    mySerial.write(138);
    delay(5);
    mySerial.print("Yellow");
  } else if (clear < 2400 && clear > 2000){
    mySerial.write(138);
    delay(5);
    mySerial.print("Grey");
  }
}

void detectColour(){
  uint16_t clear, r, g, b;
  delay(1000);
  tcs.getRawData(&r, &g, &b, &clear);
  
    if(isRed(r,g,b)){
    mySerial.write(137);
    delay(5);
    mySerial.print("Red");
    } else if(isYellow(r,g,b)){
    mySerial.write(137);
    delay(5);
    mySerial.print("Yellow");
    } else if(isGreen(r,g,b)){
    mySerial.write(137);
    delay(5);
    mySerial.print("Green");
    } else if(isBlue(r,g,b)){
    mySerial.write(137);
    delay(5);
    mySerial.print("Blue");
    } else if(isMagenta(r,g,b)){
    mySerial.write(137);
    delay(5);
    mySerial.print("Purple");
    }else if(isGrey(r,g,b)){
    mySerial.write(137);
    delay(5);
    mySerial.print("Grey");
    }
}

void detectTemp(){
  bool dt = false;
  
  while(!dt){
  if (mlx.readObjectTempF() > 50) {
    mySerial.write(12);
    delay(5);
    mySerial.print("Warm");
    dt = true;
    char outgoingg = 'B';
    Serial2.print(outgoingg);
    digitalWrite(5, HIGH);
    
  } else if (mlx.readObjectTempF() < 50) {
    mySerial.write(12);
    delay(5);
    mySerial.print("Cold");
    char outgoing = 'V';
    Serial2.print(outgoing);
    digitalWrite(4, HIGH);
    delay(500);
    dt = true;
   }
  }
}

void lap(){
  bool hash = false;
  
  while(!hash) {
  boolean qtiLeft = qtiState(LeftSensor);
  boolean qtiMiddle = qtiState(MiddleSensor);
  boolean qtiRight = qtiState(RightSensor);

  if(qtiMiddle && !qtiLeft && !qtiRight){
    drive(50);
  }else if(!qtiMiddle && qtiLeft && !qtiRight){
    turn(-50);
  }else if(!qtiMiddle && !qtiLeft && qtiRight){
    turn(50);
  }else if(qtiMiddle && qtiLeft && !qtiRight){
    turn(-50);
  }else if(qtiMiddle && !qtiLeft && qtiRight){
    turn(50);
  }else if(qtiMiddle && !qtiLeft && !qtiRight){
    turn(50);
  }else if(qtiMiddle && qtiLeft && qtiRight){
    stop();
    drive(20);
    delay(500);
    stop();
    hash = true;
  }else{
    drive(50);
  }
 }
}

void drive(int percent){
  leftServo.writeMicroseconds(map(percent, -100,100, 1300, 1700));
  rightServo.writeMicroseconds(map(percent, -100,100, 1700, 1300));
}

void turn(int percent){
  leftServo.writeMicroseconds(map(percent, -100,100, 1300, 1700));
  rightServo.writeMicroseconds(map(percent, -100,100, 1300, 1700));
}

void stop(){
  drive(0);
}

long RCTime(int sensorIn){
   long duration = 0;
   pinMode(sensorIn, OUTPUT);
   digitalWrite(sensorIn, HIGH);
   delay(1);
   pinMode(sensorIn, INPUT);
   digitalWrite(sensorIn, LOW);
   while(digitalRead(sensorIn)){
    duration++;
   }
   return duration;
}

bool qtiState(int sensorIn){
  return RCTime(sensorIn)>t;
}

//Communication Code
void process(){
  
  if(Serial2.available()){
    char incoming = Serial2.read();
    data[incoming] = 1;
  }
  int k = 0;
  
  bool ft = false;
  while (!ft){
    if (mlx.readObjectTempF() < 50) {
      k++;
      ft = true;
    } else {
      ft = true;
    }
  }
  
  for (int i = 0; i < 128; i++) {
    if(data[i] == 1 && (i==85 || i==86 || i==87 || i==88 || i==89 || i==90)) {
      k+= data[i];
    }
  }
  
  mySerial.write(148);
  delay(5);
  mySerial.print(k);
  Serial.println(k);
  delay(5);
  mySerial.write(158);
  delay(5);
  Serial.println(grandChallenge[k]);
  mySerial.print(grandChallenge[k]);
}

//New Color Sensor Code
void setRed(int r, int g, int b){
    redR = r;
    redG = g;
    redB = b;
}

bool isRed(int r, int g, int b){
    if(r < redR + tolerance && r > redR - tolerance){
        if(g < redG + tolerance && g > redG - tolerance){
            if(b < redB + tolerance && b > redB - tolerance){
                return true;
            }
        }
    }
    return false;
}

void setYellow(int r, int g, int b){
    yellowR = r;
    yellowG = g;
    yellowB = b;
}

bool isYellow(int r, int g, int b){
    if(r < yellowR + tolerance && r > yellowR - tolerance){
        if(g < yellowG + tolerance && g > yellowG - tolerance){
            if(b < yellowB + tolerance && b > yellowB - tolerance){
                return true;
            }
        }
    }
    return false;
}

void setGreen(int r, int g, int b){
    greenR = r;
    greenG = g;
    greenB = b;
}

bool isGreen(int r, int g, int b){
    if(r < greenR + tolerance && r > greenR - tolerance){
        if(g < greenG + tolerance && g > greenG - tolerance){
            if(b < greenB + tolerance && b > greenB - tolerance){
                return true;
            }
        }
    }
    return false;
}

void setBlue(int r, int g, int b){
    blueR = r;
    blueG = g;
    blueB = b;
}

bool isBlue(int r, int g, int b){
    if(r < blueR + tolerance && r > blueR - tolerance){
        if(g < blueG + tolerance && g > blueG - tolerance){
            if(b < blueB + tolerance && b > blueB - tolerance){
                return true;
            }
        }
    }
    return false;
}

void setMagenta(int r, int g, int b){
    magentaR = r;
    magentaG = g;
    magentaB = b;
}

bool isMagenta(int r, int g, int b){
    if(r < magentaR + tolerance && r > magentaR - tolerance){
        if(g < magentaG + tolerance && g > magentaG - tolerance){
            if(b < magentaB + tolerance && b > magentaB - tolerance){
                return true;
            }
        }
    }
    return false;
}

void setGrey(int r, int g, int b){
    greyR = r;
    greyG = g;
    greyB = b;
}

bool isGrey(int r, int g, int b){
    if(r < greyR + tolerance && r > greyR - tolerance){
        if(g < greyG + tolerance && g > greyG - tolerance){
            if(b < greyB + tolerance && b > greyB - tolerance){
                return true;
            }
        }
    }
    return false;
}
