#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "AudioFileSourceSD.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
//#include "AudioOutputI2SNoDAC.h"
#include "AudioOutputI2S.h"
#include "AudioOutputMixer.h"
#include "SD.h"
#include "SPI.h"

// You may need a fast SD card. Set this as high as it will work (40MHz max).
#define SPI_SPEED SD_SCK_MHZ(20)

const int chipSelect = D0;

File dir;
AudioGeneratorMP3 *mp3;
AudioFileSourceSD *source = NULL;
//AudioOutputI2SNoDAC *out;
AudioOutputI2S *out;
AudioOutputMixer *mixer;
AudioOutputMixerStub *stub;
AudioFileSourceID3 *id3;

uint32_t Size = ESP.getFreeHeap();
int analogPin = A0;
int val = 0;  // variable to store the value read

void setup() {
  WiFi.mode(WIFI_OFF); 
  Serial.begin(115200);
  Serial.println();
  delay(1000);

  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect, SPI_SPEED))    
  {
    Serial.println("initialization failed!");
    return;
  }      
  Serial.println("initialization done.");
  dir = SD.open("/");
  
  // 檢查檔案是否存在
//  if (SD.exists("/track0.mp3"))
//  {
//    Serial.println("track0.mp3 exists.");
//    //SD.remove("example.txt");
//  } else {
//    Serial.println("track0.mp3 doesn't exist.");
//  }
//  delay(1000);

  val = analogRead(analogPin);  // read the input pin
  if (val <= 6) val =0;
  if (val >= 94) val = 100;
  
  source = new AudioFileSourceSD();
  //out = new AudioOutputI2SNoDAC();
  out = new AudioOutputI2S();
  mixer = new AudioOutputMixer(32, out);
  stub = mixer->NewInput();
  stub->SetGain(val / 100.0);
  mp3 = new AudioGeneratorMP3();
  //mp3->begin(file, stub);
}

void loop() {
  if ((mp3) && (mp3->isRunning())) {
    if (!mp3->loop()) mp3->stop();
  } else {
    File file = dir.openNextFile();
    if (file) {      
      if (String(file.name()).endsWith(".mp3")) {
        source->close();
        if (source->open(file.name())) { 
          Serial.printf_P(PSTR("Playing '%s' from SD card...\n"), file.name());
          mp3->begin(source, stub);
        } else {
          Serial.printf_P(PSTR("Error opening '%s'\n"), file.name());
        }
      } 
    } else {
      Serial.println(F("Playback form SD card done\n"));
      delay(1000);
    }       
  }
}

//void loop() {
//  if (mp3->isRunning()) {
//    if (!mp3->loop()) mp3->stop();
//  } else {
//    Serial.printf("MP3 done\n");
//    delay(1000);
//  }
//}
