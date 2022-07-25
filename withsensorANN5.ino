#include <doxygen.h>
#include <NexButton.h>
#include <NexCheckbox.h>
#include <NexConfig.h>
#include <NexCrop.h>
#include <NexDualStateButton.h>
#include <NexGauge.h>
#include <NexGpio.h>
#include <NexHardware.h>
#include <NexHotspot.h>
#include <NexNumber.h>
#include <NexObject.h>
#include <NexPage.h>
#include <NexPicture.h>
#include <NexProgressBar.h>
#include <NexRadio.h>
#include <NexRtc.h>
#include <NexScrolltext.h>
#include <NexSlider.h>
#include <NexText.h>
#include <NexTimer.h>
#include <Nextion.h>
#include <NexTouch.h>
#include <NexUpload.h>
#include <NexVariable.h>
#include <NexWaveform.h>

#include <Neurona.h>

#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

const int HX711_dout = 5;
const int HX711_sck = 4;

// Pengenalan Nextion
byte received_data[4];
int lembar;
int kol3;

NexNumber tb = NexNumber(1,3,"tb");
NexNumber n0 = NexNumber(1,4,"n0");
NexNumber tb2 = NexNumber(2,11,"tb");
NexNumber bb = NexNumber(2,12,"bb"); 

NexText t3 = NexText (3,4,"t3");
NexText t4 = NexText (3,5,"t4");

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);
const int calVal_eepromAdress = 0;
unsigned long t = 0;

#define echoPin 10 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 9 //attach pin D3 Arduino to pin Trig of HC-SR04
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement


//minmax
const PROGMEM float minMax[] = {1, 2, 80, 107, 8, 17, 1, 4};
//                  min max  = minGen, maxGen, minTinggi, maxTinggi, minBerat, maxBerat, minUsia, maxUsia

// Inisialisasi ANN Gizi
#define giziANN_INPUTS 3
#define giziANN_OUTPUTS 1
#define giziANN_LAYERS 2
double giziANN_input []={0,0,0};
double giziANN_nninput []={-1.0, 0.0 ,0.0 ,0.0}; 
double PROGMEM const giziANN_weight[]={ 
0.097499420196789,
0.22348850513000285,
0.9141996721449209,
-0.01427619342662145,
1.0968668481570054,
0.5528187951338989,
-0.653015782413097,
0.32855641886297093,
0.743259761112982,
0.9366500234150956,
0.34237937649660377,
1.0135121196459973,
1.0265697973699555,
0.8609046409323504,
0.6021172832562647,
0.11949257273441233,
0.8322374447120402,
0.15819737189760363,
0.14013178858910622,
0.39782867181713993,
1.0238468047965532,
0.43536997425367746,
0.8531823340361048,
0.23731696261214658,
0.616357302332425,
0.6677665553906056,
0.04002842219602463,
1.055912703934419,
1.0049768729200967,
0.4780904888516033,
0.7161022918533929,
0.5518289004245173,
1.1117360845884077,
0.8103227121063235,
-1.799946991755344,
0.44492420144942385,
0.426014658895636,
0.08943934950514495,
-0.6653656687662856,
0.8434716920326496,
0.7621572167242561,
0.3942422898509299,
2.0902661938797893,
-0.6647345637867917,
1.147925789868186,
0.3866307835130102,
0.3908938372785883,
0.09679955107060167,
1.613211603459016,
-0.7358202183417385,
0.7642184787962955,
0.25931389275489486,
-0.5565264980064449,
0.349143320438385,
-0.4469456417883551,
0.5468566396670785,
-0.42512992808825883,
1.6655289526220804,
1.0079760472077408,
-1.7420235115128733,
-0.01795639155454908
  };

  int giziANN_layerSizes[] = {12, giziANN_OUTPUTS, -1};
  MLP giziANN_mlp(giziANN_INPUTS, giziANN_OUTPUTS, giziANN_layerSizes,MLP::LOGISTIC,giziANN_weight,true);

// Inisialisasi ANN Stunting

#define stuntingANN_INPUTS 3
#define stuntingANN_OUTPUTS 1
#define stuntingANN_LAYERS 2
double stuntingANN_input []={0,0,0};
double stuntingANN_nninput []={-1.0, 0.0 ,0.0 ,0.0}; 
double PROGMEM const stuntingANN_weight[]={ 
0.740401000174284,
0.40163389645666886,
0.796622485948444,
0.5060775819289671,
0.4960772492906072,
0.5227046934990487,
-0.9149355556132495,
1.3268921073031228,
0.19406595300342355,
0.31632977372885596,
0.9494251032446572,
-0.061184533739785905,
0.3142923519011303,
0.3879233261396616,
0.567494788763633,
1.0171692783402069,
0.7276631991812523,
0.6967075906062666,
0.6436503880291288,
0.9227980324333177,
0.8360104797245641,
0.4774510247581404,
-0.9134737243494083,
1.6730177118897702,
0.8522080457924753,
-0.0027088373829132757,
-0.14265897160715854,
0.6922056778818678,
0.1987318051448773,
0.43793964644007405,
0.47895153256571626,
0.08770246222094102,
0.4268651334463243,
0.08059231785235545,
1.9369003395560085,
-0.5476564623236126,
0.019365333126408084,
0.45421519453884984,
0.2830123340133175,
0.7945995296219545,
0.14861662323683342,
0.4710074559907101,
0.76102730160343,
-0.09496432369123256,
0.09491869975476697,
0.1690094747586902,
0.9864230139539875,
0.18214062926440436,
1.8385039238983787,
-0.2879920132618586,
1.455904724403482,
-0.7837243786824651,
0.36002002434942026,
0.07730040890844474,
1.759834914865908,
0.4563287236129899,
-0.24349426114955536,
-1.7633664253897003,
0.22962802317605546,
-0.47943758548324406,
-0.582748249106078
  };

  int stuntingANN_layerSizes[] = {12, stuntingANN_OUTPUTS, -1};
  MLP stuntingANN_mlp(stuntingANN_INPUTS, stuntingANN_OUTPUTS, stuntingANN_layerSizes,MLP::LOGISTIC,stuntingANN_weight,true);

  double giziOut, stuntingOut;
  double gen,tinggi,berat;
  int usia;



//SETUP
void setup() {
  Serial.begin(9600);
  nexSerial.begin(9600);

  nexInit();


  
  
   pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  
   LoadCell.begin();
  float calibrationValue; // calibration value (see example file "Calibration.ino")
  calibrationValue = -20.54; // uncomment this if you want to set the calibration value in the sketch
#if defined(ESP8266)|| defined(ESP32)
  //EEPROM.begin(512); // uncomment this if you use ESP8266/ESP32 and want to fetch the calibration value from eeprom
#endif
  //EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch the calibration value from eeprom

  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
  }
}


//ANN void


//LOOP
void loop() {
  //ANN_withoutSensor();
  //cekinput();
  
  usiagen();
  ANN_sensor();
  nilai();
  ANN_gizi();
  ANN_stunting();
   delay (1000);
}

void ANN_sensor(){

static boolean newDataReady = 0;
  const int serialPrintInterval = 4500; //increase value to slow down serial print activity

 // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float i = LoadCell.getData();
      float kg = i/1000;
      int  kilo = static_cast<int>(kg);
      
      berat = kilo;
      nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    bb.setValue(berat);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    n0.setValue(berat);
    
      newDataReady = 0;
      // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
 
  tinggi = 120 - distance;
  nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
 tb.setValue(tinggi);
  tb2.setValue(tinggi);

 
      t = millis();
      
    }
  }

}

void usiagen(){
  int size = Serial.readBytesUntil('\n', received_data, 4);
  if(received_data[2] == 5){
      gen = 2;
  }
  else if(received_data[2] == 6){
    gen = 1;
  }
  if(received_data[2] == 1){
    usia = 1;
  }
  else if(received_data[2] == 2){
    usia = 2;
  }
  else if(received_data[2] == 3 && received_data[1]== 0){
    usia = 3;
  }
  else if(received_data[2] == 4){
    usia = 4;
  }
}

void nilai(){

 Serial.print("Received Data = "); Serial.println(received_data[0]);
 Serial.println(received_data[1]);
 Serial.println(received_data[2]);
 
    
  
Serial.print ("Usia = ");Serial.println(usia);
  Serial.print ("gen = ");Serial.println(gen);
  Serial.print ("kilo = ");Serial.println(berat);
  Serial.print ("tinggi = ");Serial.println(tinggi);
  Serial.println("--------------------------------------------");
}

void ANN_gizi()
{
  
  giziANN_input[0] = 0.8*((gen - minMax[0])/(minMax[1]-minMax[0]))+0.1 ; //gen
  //giziANN_input[1] = 0.8*((tinggi - minMax[2])/(minMax[3]-minMax[2]))+0.1 ; //tinggi
 // giziANN_input[1] = 0.8*((tinggi - 80)/(107-80))+0.1 ; //tinggi
  giziANN_input[1] = 0.8*((berat - minMax[4])/(minMax[5]-minMax[4]))+0.1; //berat
  giziANN_input[2] = 0.8*((usia - minMax[6])/(minMax[7]-minMax[6]))+0.1; //usia

  for (int k=0;k<4;k++)
  {if(k<3)
    { giziANN_nninput[k+1]=(double)giziANN_input[k];
     // Serial.print(giziANN_nninput[k]);
    }
    }
  

  //-----------------------------------------------------------//
  double *ANNgizi=giziANN_mlp.forward(giziANN_nninput);
   //-----------------------------------------------------------//

  delay(10);
  giziOut = (double)(*ANNgizi)*10;
  Serial.print ("Level gizi = ");Serial.println(giziOut);
  if (giziOut<1.55 )
    { Serial.print("Status Gizi = "); Serial.println("BAIK");
     nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    t3.setText("BAIK");
    }
   else{
    Serial.print("Status Gizi = "); Serial.println("KURANG");
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    t3.setText("KURANG");
   }
   Serial.println("--------------------------------------------");
   
}

void ANN_stunting()
{
  
  stuntingANN_input[0] = 0.8*((gen - minMax[0])/(minMax[1]-minMax[0]))+0.1 ; //gen
  stuntingANN_input[1] = 0.8*((tinggi - minMax[2])/(minMax[3]-minMax[2]))+0.1 ; //tinggi
 // giziANN_input[1] = 0.8*((tinggi - 80)/(107-80))+0.1 ; //tinggi
  //giziANN_input[1] = 0.8*((berat - minMax[4])/(minMax[5]-minMax[4]))+0.1; //berat
 stuntingANN_input[2] = 0.8*((usia - minMax[6])/(minMax[7]-minMax[6]))+0.1; //usia

  for (int k=0;k<4;k++)
  {if(k<3)
    { stuntingANN_nninput[k+1]=(double)stuntingANN_input[k];
      //Serial.println(stuntingANN_nninput[k]);
    }
    }
  

  //-----------------------------------------------------------//
  double *ANNstunting=stuntingANN_mlp.forward(stuntingANN_nninput);
   //-----------------------------------------------------------//

  delay(10);
  stuntingOut = (double)(*ANNstunting)*10;
  Serial.print ("Level stunting = ");Serial.println(stuntingOut);
  if (stuntingOut<1.55 )
    { Serial.print("Resiko Stunting = "); Serial.println("RENDAH");
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    t4.setText("RENDAH");
    }
   else{
    Serial.print("Resiko Stunting = "); Serial.println("TINGGI");
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    t4.setText("TINGGI");
   }
   Serial.println("--------------------------------------------");
   
}
