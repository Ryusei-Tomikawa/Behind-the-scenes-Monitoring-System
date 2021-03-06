/*
  Visualizing the Panasonic Grid-EYE Sensor Data using Processing
  By: Nick Poole
  SparkFun Electronics
  Date: January 12th, 2018
  
  MIT License: Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software without 
  restriction, including without limitation the rights to use, copy, modify, merge, publish, 
  distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
  Software is furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
  BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14568
  
  This example is intended as a companion sketch to the Processing sketch found in the same folder.
  Once this code is running on your hardware, open the accompanying Processing sketch and run it 
  as well. The Processing sketch will receive the comma separated values generated by this code and
  use them to generate a thermal image. If you don't have Processing, you can download it here:
  https://processing.org/
  
  Hardware Connections:
  Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the sensor onto the shield
*/

#include <Wire.h>
#include <SparkFun_GridEYE_Arduino_Library.h>

#include <RTC.h>
#include <File.h>
#include <SDHCI.h>
#include <Audio.h>
#include <stdio.h>  /* for sprintf */
#include <Camera.h>

#include "ESP8266ATLib.h"

#define SAP_MODE
#define PORT "8080"
#define BAUDRATE                (115200)
#define TIME_HEADER 'T' // Header tag for serial time sync message

#ifdef SAP_MODE
//#define SSID "Buffalo-G-8B18"
//#define PASS "v3nsrgfjw4yxc"
#define SSID "SprESP8266AP"
#define PASS "123123123"
//#define SSID "rsdlab-student-axacna"
//#define PASS "rsdlabstudent"

#else
//house
//#define SSID "Buffalo-G-8B18"
//#define PASS "v3nsrgfjw4yxc"
#define SSID "rsdlab-student-axacna"
#define PASS "rsdlabstudent"
//#define SSID "SprESP8266AP"
//#define PASS "123123123"

#endif

SDClass theSD;
AudioClass *theAudio;
int take_picture_count = 0;

File myFile;

bool ErrEnd = false;

GridEYE grideye;

static void audio_attention_cb(const ErrorAttentionParam *atprm)
{
  puts("Attention!");
  
  if (atprm->error_code >= AS_ATTENTION_CODE_WARNING)
    {
      ErrEnd = true;
   }
}

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


/***********************************************   Camera *********************************************************/
void printError(enum CamErr err)
{
  Serial.print("Error: ");
  switch (err)
    {
      case CAM_ERR_NO_DEVICE:
        Serial.println("No Device");
        break;
      case CAM_ERR_ILLEGAL_DEVERR:
        Serial.println("Illegal device error");
        break;
      case CAM_ERR_ALREADY_INITIALIZED:
        Serial.println("Already initialized");
        break;
      case CAM_ERR_NOT_INITIALIZED:
        Serial.println("Not initialized");
        break;
      case CAM_ERR_NOT_STILL_INITIALIZED:
        Serial.println("Still picture not initialized");
        break;
      case CAM_ERR_CANT_CREATE_THREAD:
        Serial.println("Failed to create thread");
        break;
      case CAM_ERR_INVALID_PARAM:
        Serial.println("Invalid parameter");
        break;
      case CAM_ERR_NO_MEMORY:
        Serial.println("No memory");
        break;
      case CAM_ERR_USR_INUSED:
        Serial.println("Buffer already in use");
        break;
      case CAM_ERR_NOT_PERMITTED:
        Serial.println("Operation not permitted");
        break;
      default:
        break;
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

void CamCB(CamImage img)
{
  if (img.isAvailable())
    {
      img.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
    }
  else
    {
      Serial.print("Failed to get video stream image\n");
    }
}

void takepicture()
{
  sleep(1); /* wait for one second to take still picture. */

  if (take_picture_count < 10)
    {
      Serial.println("call takePicture()");
      CamImage img = theCamera.takePicture();

      if (img.isAvailable())
        {
          /* Create file name */
    
          char filename[16] = {0};
          sprintf(filename, "PICT%03d.JPG", take_picture_count);
    
          Serial.print("Save taken picture as ");
          Serial.print(filename);
          Serial.println("");
          
          theSD.remove(filename);
          File myFile = theSD.open(filename, FILE_WRITE);
          myFile.write(img.getImgBuff(), img.getImgSize());
          myFile.close();
        }

      gettime();
      take_picture_count++;
    }
  else
    {
      theCamera.end();
    }
}
/********************************************************* camera end ************************************************/

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

void setup() {

  CamErr err;

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
  Serial.println("You can see \"Hello Client!\" on the page");
  Serial.println("---------------------------------------------"); 
  
  /**********************************************************************************/
  
  while (!Serial)
    {
      ; /* wait for serial port to connect. Needed for native USB port only */
    }

  /* Initialize SD */
  while (!theSD.begin()) 
    {
      /* wait until SD card is mounted. */
      Serial.println("Insert SD card.");
    }

  /* begin() without parameters means that
   * number of buffers = 1, 30FPS, QVGA, YUV 4:2:2 format */

  Serial.println("Prepare camera");
  err = theCamera.begin();
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }

  /* Start video stream.
   * If received video stream data from camera device,
   *  camera library call CamCB.
   */

  Serial.println("Start streaming");
  err = theCamera.startStreaming(true, CamCB);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }

  /* Auto white balance configuration */

  Serial.println("Set Auto white balance parameter");
  err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }
 
  /* Set parameters about still picture.
   * In the following case, QUADVGA and JPEG.
   */

  Serial.println("Set still picture format");
  err = theCamera.setStillPictureImageFormat(
     CAM_IMGSIZE_QUADVGA_H,
     CAM_IMGSIZE_QUADVGA_V,
     CAM_IMAGE_PIX_FMT_JPG);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }
   
   // Start your preferred I2C object 
   Wire.begin();
   // Library assumes "Wire" for I2C but you can pass something else with begin() if you like
   grideye.begin();

}

void loop() 
{
  // Print the temperature value of each pixel in floating point degrees Celsius
  // separated by commas 
  
  for(unsigned char i = 0; i < 64; i++)
  {
    Serial.print("Ambient temperature:=");
    Serial.println(grideye.getPixelTemperature(i));
    //Serial.print(",");
    
    // End each frame with a linefeed
    Serial.println();

    // Give Processing time to chew
    delay(1000);
    
    // 人感温度が28℃～の時webserver,
    if(grideye.getPixelTemperature(i) > 23.0)
    {  
      takepicture();
    }
    else if(grideye.getPixelTemperature(i) > 19.0 && grideye.getPixelTemperature(i) < 23.0)
    {
      if(!esp8266_communicate(grideye.getPixelTemperature(i)))
        return false;
    }
    else
      Serial.print("");
      
  } 

}
