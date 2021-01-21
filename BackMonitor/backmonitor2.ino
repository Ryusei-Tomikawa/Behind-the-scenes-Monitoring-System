/*
 *  player.ino - Simple sound player example application
 *  Copyright 2018 Sony Semiconductor Solutions Corporation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <Wire.h>
#include <SparkFun_GridEYE_Arduino_Library.h>

#include <RTC.h>
#include <File.h>
#include <SDHCI.h>
#include <Audio.h>
#include <stdio.h>  /* for sprintf */
#include "ESP8266ATLib.h"

#define SAP_MODE
#define PORT "8080"
#define BAUDRATE                (115200)
#define TIME_HEADER 'T' // Header tag for serial time sync message
#define RECORD_FILE_NAME "fanfare.mp3"

#ifdef SAP_MODE
//#define SSID "Buffalo-G-8B18"
//#define PASS "v3nsrgfjw4yxc"
#define SSID "SprESP8266AP"
#define PASS "123123123"

#else
#define SSID "Buffalo-G-8B18"
#define PASS "v3nsrgfjw4yxc"
//#define SSID "SprESP8266AP"
//#define PASS "123123123"

#endif

SDClass theSD;
AudioClass *theAudio;

File myFile;

GridEYE grideye;

bool ErrEnd = false;
bool executed = false;


/*static const int32_t recoding_bitrate = 96000;

static const int32_t recoding_byte_per_second = (recoding_bitrate / 8);

static const int32_t recoding_size = recoding_byte_per_second * recoding_time;
*/

static void audio_attention_cb(const ErrorAttentionParam *atprm)
{
  puts("Attention!");
  
  if (atprm->error_code >= AS_ATTENTION_CODE_WARNING)
    {
      ErrEnd = true;
   }
}

/********************************************** RTC **********************************************/
void printClock(RtcTime &rtc)
{
  printf("%04d/%02d/%02d %02d:%02d:%02d\n",
         rtc.year(), rtc.month(), rtc.day(),
         rtc.hour(), rtc.minute(), rtc.second());
  
  myFile = theSD.open("dir/time.txt", FILE_WRITE);
  /* If the file opened okay, write to it */
  if (myFile) {
    Serial.print("Writing to time.txt...");
    myFile.print(rtc.year());
    myFile.print("/");
    myFile.print(rtc.month());
    myFile.print("/");
    myFile.print(rtc.day());
    myFile.print("/");
    myFile.print(rtc.hour());
    myFile.print(".");
    myFile.print(rtc.minute());
    myFile.print(".");
    myFile.print(rtc.second());
    myFile.println("/");
    /* Close the file */
    myFile.close();

    Serial.println("done.");
  } else {
    /* If the file didn't open, print an error */
    Serial.println("error opening time.txt");
  }
    
}

void updateClock()
{
  static RtcTime old;
  RtcTime now = RTC.getTime();

  // Display only when the second is updated
  if (now != old) {
    printClock(now);
    old = now;
  }
}

void gettime()
{
 // Synchronize with the PC time
  if (Serial.available()) {
    if(Serial.find(TIME_HEADER)) {
      uint32_t pctime = Serial.parseInt();
      RtcTime rtc(pctime);
      RTC.setTime(rtc);
    }
  }

  // Display the current time every a second
  updateClock();
}
/********************************************************  RTC  ************************************/

/*************************************************** esp8266  ***************************************/ 
boolean esp8266_communicate(double tem)
{
  String linkID = "";
  String s = String(esp8266at.espListenToClient(&linkID));
  String uri = "";
  Serial.print("tem:=");
  Serial.println(tem);

  if (!(s.startsWith("+IPD") && s.indexOf("HTTP/1"))) return; 
  if (s.indexOf("GET") < 0) return;  // only GET acceptable。
  Serial.println(s);

  linkID  = s.substring(5, 6);
  String msg = "HTTP/1.1 200 OK\r\n";
  msg += "Content-Type: text/html\r\n";
  msg += "\r\n";
  Serial.print(msg);
  esp8266at.sendMessageToClient(linkID, msg); //AT+CIPSENDBUF
  msg ="<html>\<head>\<meta http-equiv='refresh' content='3'/>\<title>Denshi Club ESP32 test</title>\</head>\<body>\<h1>Behind-the-scenes Monitoring System</h1>\<h2>Person Behind!.Be Careful!\n</h2></body>\</html>\r\n";      
  esp8266at.sendMessageToClient(linkID, msg);
  Serial.println("Connection closed: " + linkID);
  esp8266at.closeClientConnection(linkID);

  delay(100);

  return true;
}

/****************************************************esp8266 end  ***********************************************/

/**************************************************   speaker   ************************************************/
void audiostart()
{
  Serial.print("audio start!");
  theAudio->startPlayer(AudioClass::Player0);
  
  puts("loop!!");

  /* Send new frames to decode in a loop until file ends */
  int err = theAudio->writeFrames(AudioClass::Player0, myFile);

  /*  Tell when player file ends */
  if (err == AUDIOLIB_ECODE_FILEEND)
    {
      printf("Main player File End!\n");
    }

  /* Show error code from player and stop */
  if (err)
    {
      printf("Main player error code: %d\n", err);
      goto stop_player;
    }

  if (ErrEnd)
    {
      printf("Error End\n");
      goto stop_player;
    }

  /* This sleep is adjusted by the time to read the audio stream file.
     Please adjust in according with the processing contents
     being processed at the same time by Application.
  */

  sleep(5);

  /* Don't go further and continue play */
  return;

stop_player:
  sleep(1);
  theAudio->stopPlayer(AudioClass::Player0);
  myFile.close();
  exit(1);

}

void audiostop()
{
  theAudio->stopPlayer(AudioClass::Player0);
  myFile.close();
  exit(1);
}

/******************************************************* speaker ************************************************/

/**
 * @brief Setup audio player to play mp3 file
 *
 * Set clock mode to normal <br>
 * Set output device to speaker <br>
 * Set main player to decode stereo mp3. Stream sample rate is set to "auto detect" <br>
 * System directory "/mnt/sd0/BIN" will be searched for MP3 decoder (MP3DEC file)
 * Open "Sound.mp3" file <br>
 * Set master volume to -16.0 dB
 */
 
void setup()
{
   /* Create a new directory */
  theSD.mkdir("dir/");
  
  /* Open serial communications and wait for port to open */
  Serial.begin(BAUDRATE);
  while (!Serial);
  /*****************************************  RTC **********************************/
  RTC.begin();

  RtcTime compiledDateTime(__DATE__, __TIME__);
  RTC.setTime(compiledDateTime);
  
  /*********************************  esp8266 begin()  *****************************************/
  esp8266at.begin(BAUDRATE);
#ifdef SAP_MODE
  esp8266at.espStartAP(SSID, PASS);
#else
  esp8266at.espConnectAP(SSID, PASS);
#endif
  esp8266at.setupTcpServer(PORT);
  Serial.println("---------------------------------------------"); 
  Serial.println("Try to access the address below.");
  Serial.println("http://" + esp8266at.getLocalIP() + ":" + PORT);
  Serial.println(); 
  Serial.println("---------------------------------------------"); 
  
  /**********************************************************************************/
  
  /* Initialize SD */
  while (!theSD.begin())
    {
      /* wait until SD card is mounted. */
      Serial.println("Insert SD card.");
    }

  // start audio system
  theAudio = AudioClass::getInstance();

  theAudio->begin(audio_attention_cb);

  puts("initialization Audio Library");

  /* Set clock mode to normal */
  theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);

  theAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP, AS_SP_DRV_MODE_LINEOUT);

  /*
   * Set main player to decode stereo mp3. Stream sample rate is set to "auto detect"
   * Search for MP3 decoder in "/mnt/sd0/BIN" directory
   */
  err_t err = theAudio->initPlayer(AudioClass::Player0, AS_CODECTYPE_MP3, "/mnt/sd0/BIN", AS_SAMPLINGRATE_AUTO, AS_CHANNEL_STEREO);

  /* Verify player initialize */
  if (err != AUDIOLIB_ECODE_OK)
    {
      printf("Player0 initialize error\n");
      exit(1);
    }
    
  /* Open file placed on SD card */
  //myFile = theSD.open("fanfare.mp3");
  myFile = theSD.open("SOUND/Warning.mp3");

  /* Verify file open */
  if (!myFile)
    {
      printf("File open error\n");
      exit(1);
    }
  printf("Open! %d\n",myFile);

  /* Send first frames to be decoded */
  err = theAudio->writeFrames(AudioClass::Player0, myFile);

  if ((err != AUDIOLIB_ECODE_OK) && (err != AUDIOLIB_ECODE_FILEEND))
    {
      printf("File Read Error! =%d\n",err);
      myFile.close();
      exit(1);
    }

   puts("Play!");
   
   theAudio->setVolume(-25);

  // Start your preferred I2C object 
   Wire.begin();
   // Library assumes "Wire" for I2C but you can pass something else with begin() if you like
   grideye.begin();

   pinMode(LED0, OUTPUT);
   pinMode(LED1, OUTPUT);
   pinMode(LED2, OUTPUT);
   pinMode(LED3, OUTPUT);
}

/**
 * @brief Play stream
 *
 * Send new frames to decode in a loop until file ends
 */
void loop()
{
    digitalWrite(LED2,HIGH); //LED2 is light up    
    int CurrentState = digitalRead(LED2); //CurrentState = 1
  
   for(unsigned char i = 0; i < 64; i++)
  {
    Serial.print("Ambient temperature:=");
    Serial.println(grideye.getPixelTemperature(i));
    //Serial.print(",");
    
    // End each frame with a linefeed
    Serial.println();
    //audiostart();
    delay(1000);
    
    // 人感温度が18℃～の時webserver,
    if(grideye.getPixelTemperature(i) > 23.0)
    {  
      if(CurrentState == 1 && executed == false)
      {
        audiostart();
        executed = true;
      }
      else if(CurrentState == 0 && executed == true)
      {
        audiostart();
        executed = false;  
      }
    }
    else if(grideye.getPixelTemperature(i) > 18.5 && grideye.getPixelTemperature(i) < 23.0)
    {
      if(!esp8266_communicate(grideye.getPixelTemperature(i)))
        return false;
    }
    else
      Serial.println("Running loop");
      Serial.println("");
      
      digitalWrite(LED0,HIGH); //LED0 is light up
      delay(100);
      digitalWrite(LED1,HIGH); //LED1 is light up
      delay(100);
      digitalWrite(LED3,HIGH); //LED3 is light up
      delay(100);
      digitalWrite(LED0,LOW); //LED0 is light down
      delay(100);
      digitalWrite(LED1,LOW); //LED1 is light down
      delay(100);
      digitalWrite(LED3,LOW); //LED3 is light down
      delay(100);
      
  } 
  
}
