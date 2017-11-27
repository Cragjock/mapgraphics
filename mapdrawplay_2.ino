/******************************************************************
  BSD license, check license.txt for more information.
 All text above must be included in any redistribution.
 ******************************************************************/

#include <SPI.h>
#include <stdint.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include "Fonts/FreeMonoBoldOblique9pt7b.h"
#include "Fonts/FreeSerifItalic24pt8b.h"    // includes extended ASCII characters 
#include "Fonts/FreeSerifItalic24pt7b.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_BME280.h"
#include "sdc.h"


#define RA8875_INT 3
#define RA8875_CS 2
#define RA8875_RESET 16

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
Adafruit_BME280 bme; // I2C

uint16_t tx, ty;

void uploadUserChar(const uint8_t symbol[],uint8_t address)
{
    tft.writeCommand(RA8875_MWCR1);
    uint8_t tempMWCR1 = tft.readData();

    /* Set graphics mode */
    tft.graphicsMode(); 
    
    //tft.writeCommand(RA8875_MWCR0);
    //uint8_t temp = tft.readData();
    //temp &= ~RA8875_MWCR0_TXTMODE; // bit #7
    //tft.writeData(temp);

    // set CGRAM address
    tft.writeCommand(RA8875_CGSR);    // reg 23
    tft.writeData(address);

    //set reg 21 bit 7 =0
    tft.writeReg(RA8875_FNCR0, 0x00); 
    tft.writeReg(RA8875_MWCR1, 0x04); 
    
    //tft.writeCommand(RA8875_FNCR0);
    //temp = tft.readData();
    //temp &= ~RA8875_FNCR0_CGROM;
    //tft.writeData(temp);

  // set reg 41 for CGRAM destination
    //tft.writeCommand(RA8875_MWCR1);
    //temp = tft.readData();
    //temp &= ~RA8875_MWCR1_CGRAM; // bit #2-3
    //tft.writeData(temp);

    tft.writeCommand(RA8875_MRWC);

  for (int i=0;i<16;i++)
  {
    //Serial.println("loading stuff ");
    tft.writeData(pgm_read_byte(&symbol[i]));   
    //Serial.println(pgm_read_byte(&symbol[i]));    
    
    
    //tft.writeData(symbol[i]);
    //Serial.println(symbol[i]); 
    //tft.writeData(pgm_read_byte(&g1_cursor[i])); 
  }
  //restore register
    //tft.writeCommand(RA8875_MWCR1);
    //tft.writeData(tempMWCR1);
}





/**************************************************
 * SETUP START
 */
void setup() 
{
  bool status;
  Serial.begin(115200);
  Serial.println("BME start");
  status = bme.begin();  
  if (!status) 
  {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
  }

  Serial.println("RA8875 start");
  
  #ifdef ESP8266    // THIS IS CORRECT 
  Serial.println("ESP8266 yes");
  #else
  Serial.println("ESP8266 = no");
  #endif

  /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
  if (!tft.begin(RA8875_800x480)) 
  {
    Serial.println("RA8875 Not Found!");
    while (1);
  }

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);   // was 255

  tft.fillScreen(RA8875_PURPLE);
  //tft.fillScreen(RA8875_LIGHT_GREY);

  
  tft.textMode();
  tft.textSetCursor(20,400);
  char string1[15] = "start where!  ";

  
  const char sdceng[] ={0x53,0x74,0x65,0x76,0x65,0x6e,0xB0,0x0};
  tft.textTransparent(RA8875_WHITE);
  tft.textWrite(string1);
  tft.textWrite(sdceng);

  uploadUserChar(bmSatLeft, 0);   // sat bmp
  uploadUserChar(bmSatright, 1);
  tft.textMode();
  tft.writeReg(RA8875_FNCR0, 0xA0); 
  tft.writeReg(RA8875_MWCR1, 0x0); 

 // Serial.println("enlarge check ");
 // tft.writeCommand(0x22);
  //Serial.println(tft.readData());
 tft.textEnlarge(2);
 // Serial.println(tft.readData());
  


  tft.textColor(RA8875_WHITE, RA8875_RED);
  tft.writeCommand(RA8875_MRWC);
  //for(int i=0; i<3; i++)
  //{
    tft.writeData(0);
    delay(1);       // need or the images on topc of each other if text Enlarge set 
    tft.writeData(1);
  //}
   tft.writeReg(RA8875_FNCR0, 0x00); 
    tft.textEnlarge(0);



// graphics cursor items start
  tft.writeCommand(0x84);
  tft.writeData(0xE0);
  tft.writeCommand(0x85);
  tft.writeData(0x1C);  
  
  tft.writeCommand(0x41);
  tft.writeData(0x08);    //setto write data
  tft.writeCommand(0x02);
  for(int i=0; i<255; i++)
  {
    //Serial.println("in the loop");
    //Serial.println(g1_cursor[i]);
    //Serial.println(pgm_read_byte(&g1_cursor[i]));
    tft.writeData(pgm_read_byte(&g1_cursor[i]));    // read from EEPROM 
  }  
  tft.writeCommand(0x41);
  tft.writeData(0x80);  


  tft.writeCommand(0x80);   // horiz position 
  tft.writeData(0x64);
  tft.writeCommand(0x82); // vertical position
  tft.writeData(0x64);  
// graphics cursor items  end

  // ESP.wdtDisable();

  /* Switch to text mode */  
  //tft.textMode();
  tft.graphicsMode();
  
  Serial.println("graphicsMode ");

    
  tft.setFont(&FreeMonoBoldOblique9pt7b); // setFont sets the gfxFont variable in the GFX lib 
  //tft.drawCircle(400, 100, 50, RA8875_BLACK);
  tft.fillCircle(400, 100, 50, RA8875_GREEN);
  
  tft.setCursor(200,300);
  tft.write(0x34); 
  tft.write('S');
  tft.write('D');
  tft.write('C');
  tft.write(' ');
  tft.write('!');
  tft.drawRect(200, 300, 16*5,  18*1, RA8875_YELLOW);
  tft.print("W T F "); // this works too 
  //const char sdceng1[] ={0x53,0x74,0x65,0x76,0x65,0x6e,0xB0,0x0};
  const char sdceng1[] ={'S','t','e','v','e','n',0xB0,0x0};


  

  tft.setFont(&FreeSerifItalic24pt7b);  
  const char testarr[]="Does this work?";
  for(int i=0; i<strlen(testarr); i++)
    tft.write(testarr[i]);

  tft.setFont(&FreeSerifItalic24pt8b);    
  tft.setCursor(300,425); 
  for(int i=0; i<strlen(sdceng1); i++)
    tft.write(sdceng1[i]); 


 tft.setFont(&FreeSerifItalic24pt7b);  

    
 Serial.println("testarr ");

    int row=0; 
    int column=0;
    int d_position = 0;   // draw position
    tft.setXY(20,100);
  Serial.println("starting loop ");


/************** this is hamclock type data *****
    for(row=0; row<EARTH_H; row++)
    {
      yield(); 
      for(d_position=0; d_position<(EARTH_W*2); (d_position++)*2)
      {
          tft.drawPixel((d_position), (row), pgm_read_word(&DEARTH[row][d_position/2]));   // this works to stretch it out
          tft.drawPixel(((d_position)+1), (row), pgm_read_word(&DEARTH[row][d_position/2]));   // this works
          
          //Serial.println(d_position);
      }
    }
 *******************/  
    
  int j=0; 
  uint16_t HB=0;
  uint16_t LB=0;
  int ALL=0;
  uint16_t border = 10; 

  Box mapbox={150,127,500,225, RA8875_RED}; // was 321 x 163 for world 565, 213 for charmap fpt LCD2, 320 x144
  tft.drawRect(mapbox.x, mapbox.y, mapbox.w, mapbox.h, mapbox.c);


  Box LCDbox={400,200,321,145, RA8875_RED};  // LCD2, 321 145

  drawBorder(mapbox, border);
  //drawBorder(LCDbox, border);

    for(row=0; row<225; row++)    // was 163 for world 565, 213 for charmap fpt LCD2, 320 x144
    {
      yield(); 
      for(column=0; column<500; column++) 
      {
          //tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word(&charmap[j])); // 320x215
          //tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word(&world565[j]));  //320x163
          tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word(&lcd3[j]));    // 320x144
          j=j+1;
          //Serial.println(j);
      }
    }
    int lcdsize = sizeof(LCD2)/sizeof(LCD2[0]);
    Serial.print("lcdsize ");
    Serial.println(lcdsize/320); 

    tft.drawRect(200, 200, 272, 33, RA8875_YELLOW); // for row 1 box
    tft.drawRect(200, 250, 272, 33, RA8875_YELLOW); // for row 2 box


    

    mapbox={10,150,660,330, RA8875_WHITE};
    //tft.drawRect(0,0,660,330, RA8875_WHITE); 
    tft.drawRect(mapbox.x, mapbox.y, mapbox.w, mapbox.h, mapbox.c); 


// byte mode draw map 
//    j=0;
//    for(row=0; row<144; row++)
//    {
//      yield(); 
//      for(column=0; column<320; column++)
//      {
//          HB = pgm_read_byte(&LCD6[j]); 
//          ALL = pgm_read_byte(&LCD6[j+1]);
//          ALL = ALL | (HB << 8);
          // ALL = ~ALL;
         // tft.drawPixel((column + mapbox.x), (row+mapbox.y), ALL);
//          j=j+2;
//      }
//    }

// byte mode draw map 


/************************
// byte mode draw map with stretch
    j=0;
    d_position=0;
    
    for(row=0; row<144; row++)
    {
      yield(); 
      for(d_position=0; d_position<640; (d_position++)*2)
      {
          HB = pgm_read_word(&LCD2[d_position/2 + (row*320) ]); 
          //ALL = pgm_read_byte(&LCD6[d_position/2+1]);
          //ALL = ALL | (HB << 8);
          // ALL = ~ALL;
          
          tft.drawPixel((d_position + mapbox.x), (row+mapbox.y), HB);
          tft.drawPixel((d_position+1 + mapbox.x), (row+mapbox.y), HB);
         
          //j=j+2;
        
      }
    }

// byte mode draw map 
*****************/



    

  Box CS_box={0,0,250,140, RA8875_RED};
  uint16_t main_width = 160;
  uint16_t main_height = 140;
  uint16_t small_width = 250;
  uint16_t small_height = 170;
  uint16_t CS_width = 250;
  uint16_t CS_height = 140;
  uint16_t GPS_height = 170; 
 
  
 /*************** 
  
  tft.fillRect(0, 0, 250, 140, RA8875_WHITE);
  tft.fillRect(CS_box.w+1, 0, main_width, main_height, RA8875_GREEN);
  tft.fillRect(CS_box.w+main_width*1+1, 0, main_width, main_height, RA8875_BLUE);
  tft.fillRect(CS_box.w+main_width*2+1, 0, main_width, main_height, RA8875_RED);

  tft.fillRect(0, CS_box.x+main_height+1, CS_width, GPS_height, RA8875_OLIVE);
  tft.fillRect(0, CS_box.x+main_height+small_height+1, CS_width, GPS_height, RA8875_RED);
**********************/
  

  
  /* Set a solid for + bg color ... */
  /* ... or a fore color plus a transparent background */
  /* Set the cursor location (in pixels) */
  tft.textSetCursor(10, 10);


  // check font set
  //tft.writeCommand(0x21);
  //Serial.print("font set selected: ");
  //tft.writeData(0x01); 
  //Serial.println(tft.readData());


  setvisiblecursor(UNDER, true);
/* trun on cursor blink for 0.5 seconds  */
  setCursorBlinkRate(0x1E);
  tft.writeReg(RA8875_MWCR0, 0xF0);     // 0xf0 text mode, font write cursor visible, blink on 
  
  //tft.writeCommand(RA8875_MWCR0);
  //tft.writeData(0xF0);
  //tft.writeReg(RA8875_BTCR, 0x1E);    // cursor blink rate, see page 103 of spec for equation 
  //tft.writeCommand(0x44);
  //tft.writeData(0x1E);
  
  /* Render some text! */
  char string[15] = "K C6FEW 098765";
  tft.textTransparent(RA8875_WHITE);
  tft.textWrite(string);
  tft.textColor(RA8875_WHITE, RA8875_RED);
  tft.textWrite(string);
  tft.textTransparent(RA8875_CYAN);
  tft.textWrite(string);
  tft.textTransparent(RA8875_GREEN);
  tft.textWrite(string);
  tft.textColor(RA8875_YELLOW, RA8875_CYAN);
  tft.textWrite(string);
  tft.textColor(RA8875_BLACK, RA8875_MAGENTA);
  //tft.print(string); 
  tft.textWrite(string);

  /* Change the cursor location and color ... */  

  tft.textSetCursor(235, 200);
  tft.textTransparent(RA8875_YELLOW);
  //    tft.textColor(RA8875_YELLOW, RA8875_BLACK);

  tft.textEnlarge(1);
  tft.textWrite(string);
  
  tft.textSetCursor(10, 40);
  tft.textEnlarge(2);

  float bm_temp = bme.readTemperature();
  char t_buffer[20];
  tft.textWrite(dtostrf(bm_temp, 3,3, t_buffer) );

  tft.drawRect(10,40,(8*3*16), 16*3+3, RA8875_YELLOW);
  // Serial.println("at sensor check ");
  printValues();
  
}
float bm_temp=0;
float bm_prior=0;
const char degreefont[] ={0xB0,0x0};

void loop() 
{
  //printValues();
  //tft.textMode();
  tft.textColor(RA8875_YELLOW, RA8875_PURPLE);
  tft.textSetCursor(10+1, 40+1);
  
  delay(1000);
  char t_buffer[20];
  
   tft.textSetCursor(10+1, 40+1);
  //  tft.textWrite(dtostrf(bm_prior, 3,3, t_buffer) );
  //  tft.textSetCursor(10, 40);

  //  tft.textTransparent(RA8875_YELLOW);
   tft.textSetCursor(10+1, 40+1);
  bm_temp = bme.readTemperature();
  bm_temp = bm_temp*(9.0/5.0) + 32.0;
  
  tft.textWrite("Temp(");
  tft.textWrite(degreefont);
  tft.textWrite("f): ");
  
  tft.textWrite(dtostrf(bm_temp, 3,3, t_buffer) );
  bm_prior = bm_temp; 
}

void printValues() 
{
    delay(100); // let sensor boot up 
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}


