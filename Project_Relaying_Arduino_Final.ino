#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial s(5,6);
// defining current and voltages pins.
#define iRpin A0
#define iYpin A3
#define iBpin A4
//Tripping Signals
#define tripR 10
#define tripY 12
#define tripB 11
#define theftPinR 4
#define theftPinY 3
#define theftPinB 2

//defining global variables
#define maxCurr 4
long count=0;

//for taking rms values
float maxCurrentR;
float maxCurrentY;
float maxCurrentB;
//current values
float iR;
float iY;
float iB;
//for timing of code
uint32_t power_send_time ;

//strcutures
struct faultPhaseDetail{
  uint32_t faultTime;
  boolean permanentFaultFlag;
  boolean theftFlag;
  int faultCheckCounter;
  };
  typedef struct faultPhaseDetail FaultStruct;
  FaultStruct rPhase;
  FaultStruct yPhase;
  FaultStruct bPhase;


void setup(){
  Serial.begin(9600);
   s.begin(115200);
  //Seting up current and voltages pins as inputs
  pinMode(iRpin, INPUT);
  pinMode(iYpin, INPUT);
  pinMode(iBpin, INPUT);
  pinMode(theftPinR,INPUT);
  pinMode(theftPinY,INPUT);
  pinMode(theftPinB,INPUT);

  //setting up trip signals as output
  pinMode(tripR,OUTPUT);
  pinMode(tripY,OUTPUT);
  pinMode(tripB,OUTPUT);
  digitalWrite(tripR,HIGH);
  digitalWrite(tripY,HIGH);
  digitalWrite(tripB,HIGH);
  //resetting flags
  rPhase.theftFlag=false;
  yPhase.theftFlag=false;
  bPhase.theftFlag=false;
  rPhase.permanentFaultFlag=false;
  yPhase.permanentFaultFlag=false;
  bPhase.permanentFaultFlag=false;
   rPhase.faultCheckCounter=0;
   bPhase.faultCheckCounter=0;
   yPhase.faultCheckCounter=0;
  
  delay(5000);
}

void loop(){
  long c1=0;
  long c2=0;
  long c3=0;

    if(digitalRead(theftPinR)==HIGH) rPhase.theftFlag= true;
     else   rPhase.theftFlag= false;
    if(digitalRead(theftPinY)==HIGH) yPhase.theftFlag= true;
    if(digitalRead(theftPinY)==LOW) yPhase.theftFlag= false;
    if(digitalRead(theftPinB)==HIGH) bPhase.theftFlag= true;
     else   bPhase.theftFlag= false;
//    
   uint32_t start_time = millis();
    
      while((millis()-start_time) < 50){
      double rawValue = analogRead(iRpin);
      double rawVoltage = (rawValue / 1024.0)*5000;
      double currValue = ((abs(2500-rawVoltage))/66);
      maxCurrentR+=(currValue*currValue);    
      c1++;
      }
      iR= sqrt(maxCurrentR/c1);  
    

//   if(!yPhase.theftFlag || !yPhase.permanentFaultFlag){
       start_time = millis();
       while((millis()-start_time) < 50){
        double rawValue = analogRead(iYpin);
        double rawVoltage = (rawValue / 1024.0)*5000;
        double currValue = ((abs(2500-rawVoltage))/66);
        maxCurrentY+=(currValue*currValue);    
        c2++;
       }
       iY= sqrt(maxCurrentY/c2);
//    }

//    if(!bPhase.theftFlag || !bPhase.permanentFaultFlag){
        start_time = millis();
        while((millis()-start_time) < 50){
          double rawValue = analogRead(iBpin);
          double rawVoltage = (rawValue / 1024.0)*5000;
          double currValue = ((abs(2500-rawVoltage))/66);
          maxCurrentB+=(currValue*currValue);    
          c3++;
        }
        iB= sqrt(maxCurrentB/c3);     
//      }
    Serial.print((iR));
    Serial.print(" ");
    Serial.print(iY);
    Serial.print(" ");
    Serial.print(iB);
    Serial.println(" ");

   //checking and fault signals
   char faultPhase=checkCurr(iR,iY,iB);
   tripSignal(faultPhase);
   recursiveFault();

  //resetting max current values
   maxCurrentR=0;
   maxCurrentB=0;
   maxCurrentY=0;

   //power calcultations
    float powerR= iR*12;
    float powerY= iY*12;
    float powerB= iB*12;
    s.flush();
   if(millis()-power_send_time>10000){
        
        s.print(powerR); s.print(" R ");
        s.print(powerY); s.print(" Y ");
        s.print(powerB); s.print(" B ");
        s.print("\n");
        power_send_time=millis();
        delay(500);
      }

   
   
}
//checking for overCurrent
char checkCurr(float r,float y,float b){
  if(r>maxCurr || y>maxCurr || b>maxCurr){
    if(r>maxCurr){
        return 'r';
    }
    if(y>maxCurr){
        return 'y';
    }
    if(b>maxCurr){
        return 'b';
    }
  }else{
    return 'a';
  }  
}
// tripping Signal 
void tripSignal(char c){
    if(c=='r' || rPhase.theftFlag || rPhase.permanentFaultFlag){
      digitalWrite(tripR,LOW);
      rPhase.faultTime=millis();
      rPhase.faultCheckCounter=1;
      Serial.println("Rphase Closed");
    }
    if(c=='y' || yPhase.theftFlag || yPhase.permanentFaultFlag){
      digitalWrite(tripY,LOW);
      yPhase.faultTime=millis();
      yPhase.faultCheckCounter=1;
      Serial.println("Yphase Closed");
    }
    if(c=='b' || bPhase.theftFlag || bPhase.permanentFaultFlag){
      digitalWrite(tripB,LOW);
      bPhase.faultTime=millis();
      bPhase.faultCheckCounter=1;
      Serial.println("Bphase Closed");
    }
    return;
}

//recursivefault function
void recursiveFault(){
  //for r phase
  if(rPhase.faultTime>0 && !rPhase.permanentFaultFlag){
    if(millis()-rPhase.faultTime>=5000){
      rPhase.faultCheckCounter++;
      digitalWrite(tripR,HIGH);
      delay(200);
      rPhase.faultTime=0;
      }
    if(rPhase.faultCheckCounter>4){
      rPhase.permanentFaultFlag=true;
      }
    }
    //for yphase
    if(yPhase.faultTime>0 && !yPhase.permanentFaultFlag){
    if(millis()-yPhase.faultTime>=5000){
      yPhase.faultCheckCounter++;
      digitalWrite(tripY,HIGH);
      yPhase.faultTime=0;
      }
    if(yPhase.faultCheckCounter>3){
      yPhase.permanentFaultFlag=true;
      }
    }
    //for bphase
    if(bPhase.faultTime>0 && !bPhase.permanentFaultFlag){
    if(millis()-bPhase.faultTime>=5000){
      bPhase.faultCheckCounter++;
      digitalWrite(tripB,HIGH);
      bPhase.faultTime=0;
      }
    if(bPhase.faultCheckCounter>3){
      bPhase.permanentFaultFlag=true;
      }
    } 
  }
  
