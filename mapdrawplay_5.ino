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
#include "Fonts/FreeSerifItalic18pt8b.h"
#include "Fonts/FreeSerifItalic24pt7b.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_BME280.h"
#include "sdc.h"

#define RA8875_INT 3
#define RA8875_CS 2
#define RA8875_RESET 16

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
Adafruit_BME280 bme;  // I2C

uint16_t tx, ty;

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

  if (!tft.begin(RA8875_800x480)) 
  {
    Serial.println("RA8875 Not Found!");
    while (1);
  }

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255); 
  tft.fillScreen(RA8875_PURPLE);
  //tft.fillScreen(RA8875_LIGHT_GREY);
// ============ BASIC SETUP Complete =================


  char string1[15] = "start where!  ";
  const char sdceng[] ={0x53,0x74,0x65,0x76,0x65,0x6e,0xB0,0x0};
  //const char sdceng1[] ={0x53,0x74,0x65,0x76,0x65,0x6e,0xB0,0x0};
  const char sdceng1[] ={'S','t','e','v','e','n',0xB0,0x0};
  uint16_t row=0; 
  uint16_t column=0;
  int d_position = 0;   // draw position

  tft.textMode();     // =========TEXT MODE ON ====
  tft.textSetCursor(20,400);
  tft.textTransparent(RA8875_WHITE);
  tft.textWrite(string1);
  tft.textSetCursor(20,416); // one line down ? newline does not work the same 
  tft.textWrite(sdceng);

  
  // === load satellite icon =====
  uploadUserChar(bmSatLeft, location_1);   // sat bmp
  uploadUserChar(bmSatright, location_2);
  
  tft.textMode();   // =========TEXT MODE ON ====
  tft.writeReg(RA8875_FNCR0, 0xA0);   // select CGRAM 
  tft.writeReg(RA8875_MWCR1, 0x0);

// ========== Set up and display satellite icon  
  tft.textSetCursor(400,400);
  tft.textEnlarge(2);   // set to 3x size 
  tft.textColor(RA8875_WHITE, RA8875_RED);
  tft.writeCommand(RA8875_MRWC);
  tft.writeData(location_1);
  delay(1);       // need or the images on topc of each other if text Enlarge set 
  tft.writeData(location_2);
  tft.writeReg(RA8875_FNCR0, 0x00); // select CGROM 
// ========== Sat out done
  
  tft.textEnlarge(0);   // set back to 1x size 

// ============== graphics cursor items start
  tft.writeCommand(RA8875_GCC0);
  tft.writeData(0xE0);        // RGB RED
  tft.writeCommand(RA8875_GCC1);
  tft.writeData(0x1C);        // RGB BLUE
  
  tft.writeCommand(RA8875_MWCR1);
  tft.writeData(0x08);    //write to graphics cursor buffers & gr cursor off 
  tft.writeCommand(RA8875_MRWC);
  for(int i=0; i<255; i++)
  {
    tft.writeData(pgm_read_byte(&g1_cursor[i]));    // read from EEPROM 
  }  
  tft.writeCommand(RA8875_MWCR1);
  tft.writeData(0x80);    // graphics cursor on & write location set to layer 1

  tft.writeCommand(RA8875_GCHP0);   // horiz position 
  tft.writeData(0x64);              // 100 decimal
  tft.writeCommand(RA8875_GCVP0); // vertical position
  tft.writeData(0x64);            // 100 decimal
// ========== graphics cursor items  end



  tft.graphicsMode();   // ==== GRAPHICS MODE ON     
  tft.setFont(&FreeMonoBoldOblique9pt7b); // setFont sets the gfxFont variable in the GFX lib 
  //tft.drawCircle(400, 100, 50, RA8875_BLACK);
  tft.fillCircle(400, 100, 50, RA8875_GREEN);
  
  tft.setCursor(10,150);
  tft.write(0x34); // number 4 
  tft.write('S');
  tft.write('D');
  tft.write('C');
  tft.write(' ');
  tft.write('!');
  tft.drawRect(10, 300, 16*5,  18*1, RA8875_YELLOW);
  tft.print("9 Steven"); // this works too 

  tft.setFont(&FreeSerifItalic18pt8b);  
  tft.setCursor(10,250); 
  tft.print("18 Steven"); // this works too 


  tft.setFont(&FreeSerifItalic24pt7b);  
  tft.setCursor(10,300); 
  tft.print("24 Steven"); // this works too 

  

  tft.setFont(&FreeSerifItalic24pt7b);  
  const char testarr[]="Does this work?";
  for(int i=0; i<strlen(testarr); i++)
    tft.write(testarr[i]);

  tft.setFont(&FreeSerifItalic24pt8b);    
  tft.setCursor(235,400); 
  for(int i=0; i<strlen(sdceng1); i++)
    tft.write(sdceng1[i]); 

  tft.println(string1); // oh crap. this works! 

 tft.setFont(&FreeSerifItalic24pt7b);  
 tft.setXY(20,100);    // or should this be setcursor ???



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
  const uint16_t* cc=0;


  Box mapbox={150,127,500,225, RA8875_RED}; // was 321 x 163 for world 565, 213 for charmap fpt LCD2, 320 x144
  tft.drawRect(mapbox.x, mapbox.y, mapbox.w, mapbox.h, mapbox.c);


  Box LCDbox={400,200,321,145, RA8875_RED};  // LCD2, 321 145

  drawBorder(mapbox, border);
  //drawBorder(LCDbox, border);

// ===== draws the LCD Display image =====

/************************ KEEP JUST IN CASE *****************
    for(row=0; row<225; row++)    // was 163 for world 565, 213 for charmap fpt LCD2, 320 x144
    {
      yield(); 
      for(column=0; column<500; column++) 
      {
          //tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word(&charmap[j])); // 320x215
          //tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word(&world565[j]));  //320x163
          tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word(&lcd3[j]));    // GOOD
          j=j+1;
      }
    }
**************************************************************/
  
  draw_map(lcd3,225,500, mapbox); 
  
  Box testbox={10,20,321,145, RA8875_RED}; 
  draw_map(LCD2,144,320, testbox); 
    
    //int lcdsize = sizeof(LCD2)/sizeof(LCD2[0]);
    //Serial.print("lcdsize ");
    //Serial.println(lcdsize/320); 

    //tft.drawRect(200, 200, 272, 33, RA8875_YELLOW); // for row 1 box
    tft.drawRect(210, 200, 272, 33, RA8875_YELLOW); // for row 1 box    // better alignment 
    //  tft.drawRect(200, 250, 272, 33, RA8875_YELLOW); // for row 2 box



    //mapbox={10,150,660,330, RA8875_WHITE};
    //tft.drawRect(0,0,660,330, RA8875_WHITE); 
    //tft.drawRect(mapbox.x, mapbox.y, mapbox.w, mapbox.h, mapbox.c); 


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


//  setvisiblecursor(UNDER, true);
/* trun on cursor blink for 0.5 seconds  */
//  setCursorBlinkRate(0x1E);
//  tft.writeReg(RA8875_MWCR0, 0xF0);     // 0xf0 text mode, font write cursor visible, blink on 
  
  //tft.writeCommand(RA8875_MWCR0);
  //tft.writeData(0xF0);
  //tft.writeReg(RA8875_BTCR, 0x1E);    // cursor blink rate, see page 103 of spec for equation 
  //tft.writeCommand(0x44);
  //tft.writeData(0x1E);

  
  // ========== top header area =============
  /* Render some text! */

  Box header_L = {0,5,266,20};
  tft.drawRect(0,5,266,20, RA8875_WHITE);
  Box header_C = {266,5,268,20};
  tft.drawRect(266,5,268,20, RA8875_WHITE);
  Box header_R = {534,5,266,20};
  tft.drawRect(534,5,266,20, RA8875_WHITE);
  
  tft.textMode();   // ==== TEXT MODE ON ====
  char string[17] = " Hey de KC6FEW! ";
  tft.textSetCursor(header_L.x, header_L.y); 
  tft.textTransparent(RA8875_WHITE);
  tft.textWrite(string);
  tft.textColor(RA8875_WHITE, RA8875_RED);
  tft.textWrite(string);

  tft.graphicsMode(); // ==== GRAPHICS MODE ON ====
  tft.setFont(&FreeSerifItalic24pt7b);  
  tft.setCursor(header_C.x,header_C.y+40); 
  tft.print("-[ WELCOME! ]-"); // this works too 

  tft.textMode();   // ==== TEXT MODE ON ====
  
  tft.textSetCursor(header_R.x, header_R.y);
  //tft.textTransparent(RA8875_CYAN);
  //tft.textWrite(string);
  //tft.textTransparent(RA8875_GREEN);
  //tft.textWrite(string);
  
  tft.textColor(RA8875_YELLOW, RA8875_CYAN);
  tft.textWrite(string);
  tft.textColor(RA8875_BLACK, RA8875_MAGENTA);
  //tft.print(string); 
  tft.textWrite(string);


  

  /* Change the cursor location and color ... */  
  tft.textEnlarge(1);
  tft.textSetCursor(235, 200);
  tft.textTransparent(RA8875_YELLOW);
  //    tft.textColor(RA8875_YELLOW, RA8875_BLACK);
  char h_buffer[20];
  float bm_humid = bme.readHumidity(); 
  tft.textWrite(dtostrf(bm_humid, 3,3, h_buffer) );
  
  tft.textWrite(string);
  
  tft.textSetCursor(10, 40);
  tft.textEnlarge(2);

  float bm_temp = bme.readTemperature();
  char t_buffer[20];
  tft.textWrite(dtostrf(bm_temp, 3,3, t_buffer) );


  tft.drawRect(10,40,(8*3*16), 16*3+3, RA8875_YELLOW);
  // Serial.println("at sensor check ");
  printValues();
  

  tft.graphicsMode(); // ==== GRAPHICS MODE ON ====

  // === redraws data in the LCD image test 

  tft.setFont(&FreeSerifItalic24pt8b);    
  tft.setCursor(200,200); 
  for(int i=0; i<strlen(sdceng1); i++)
    tft.write(sdceng1[i]); 

  
  
  mapbox={150,127,500,225, RA8875_RED};
  for(row=50; row<100; row++) 
    {
      yield(); 
      for(column=50; column<100; column++) 
      {
          tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word(&lcd3[column + row*500]));    // GOOD
      }
    }


    
  tft.setFont(&FreeSerifItalic24pt8b);    
  tft.setCursor(200,200); 
  for(int i=0; i<strlen(sdceng1); i++)
    tft.write(sdceng1[i]); 


    
  tft.textMode();   // ==== TEXT MODE ON ====

/*************************************
  BTE_block_move  ( 0, //SourceX
              0, //SourceY 
              300, //Width
              225, //Height
              400, //DestX
              200, //DestY
              0x02);
************************************/              

              
 //Serial.println("by BTE .....");             

  tft.textMode(); 
   //Serial.println("by BTE 123 ....."); 

}
// ================== END =============
float bm_temp=0;
float bm_prior=0;
const char degreefont[] ={0xB0,0x0};
//const char newline[] = {0x0d, 0x0a, 0x00}; // newline is not in the internal font map 
uint8_t newline = '\n';     // still not working 

void loop() 
{
    uint16_t row=0; 
  uint16_t column=0;
  tft.textEnlarge(1);  
  printValues();
  tft.textMode();
  tft.textColor(RA8875_YELLOW, RA8875_PURPLE);
  tft.textSetCursor(10+1, 40+1);
  //Serial.println("in main loop ~~~~...");
  
  delay(1000);
  char t_buffer[20];
  
  tft.textSetCursor(10+1, 40+1);

  tft.textSetCursor(10+1, 40+1);
  bm_temp = bme.readTemperature();
  bm_temp = bm_temp*(9.0/5.0) + 32.0;
  
  tft.textWrite("Temp(");
  tft.textWrite(degreefont);
  tft.textWrite("f): ");
  
  tft.textWrite(dtostrf(bm_temp, 3,3, t_buffer) );

  float bm_humid = bme.readHumidity(); 
  tft.textSetCursor(10+1, 40+1+48);
  //tft.write(newline);

  tft.textWrite("Humidity: ");
  tft.textWrite(dtostrf(bm_humid, 3,3, t_buffer) );
  tft.textWrite("%");


 tft.graphicsMode(); // ==== GRAPHICS MODE ON ====
//              tft.drawRect(200, 200, 272, 33, RA8875_YELLOW); // for row 1 box
  // redraw     tft.drawRect(200, 250, 272, 33, RA8875_YELLOW); // for row 2 box
  // Box mapbox={150,127,500,225, RA8875_RED} for big LCD 
  //    tft.drawRect(475, 250, 90, 30, RA8875_WHITE);
  
  Box source = {471, 250, 90, 31, RA8875_WHITE};
  //tft.drawRect(source.x, source.y, source.w, source.h, source.c);

  // for flicker avoidance ============

/*************************************/
  BTE_block_move  ( source.x, //SourceX
                    source.y, //SourceY 
                    source.w, //Width 125
                    source.h, //Height
                    210, //DestX
                    250, //DestY
                    0x02);
/************************************/ 
  Box mapbox={150,127,500,225, RA8875_RED};
 // for(row=125; row<168; row++) 
 //   {
 //     yield(); 
 //     for(column=50; column<175; column++) 
 //     {
 //         tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word(&lcd3[column + row*500]));    // GOOD
 //     
 //     }
 //   }
  
  Box fontposition ={210, 250, 99, 24, RA8875_GREEN};
  tft.drawRect(fontposition.x, fontposition.y, fontposition.w, fontposition.h, fontposition.c);   // font posiioning rect 
  tft.setFont(&FreeSerifItalic18pt8b);    
  tft.setCursor(fontposition.x,fontposition.y + fontposition.h); // font position is lower left !! need pad ?
  tft.print(bm_temp); // this works too 
  tft.print(degreefont);
  

  //int16_t px;
  //int16_t py;
  //uint16_t pw;
  //uint16_t ph;
  //tft.getTextBounds(t_buffer, 205, 280, &px, &py, &pw, &ph);    // this works ok 
  //Serial.println(px);
  //Serial.println(py);
  //Serial.println(pw);
  //Serial.println(ph);

  tft.textMode();




  
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


