
#include "sdc.h"


extern Adafruit_RA8875 tft;
  uint16_t row=0; 
  uint16_t column=0;

/***************/
void drawBorder(Box mybox, uint16_t border)
{

  tft.fillRect(mybox.x-border, mybox.y-border, mybox.w+border*2, mybox.h+border*2, RA8875_WHITE);
  tft.fillRect(mybox.x, mybox.y, mybox.w+border, mybox.h+border, RA8875_DARK_GREY);
  tft.fillTriangle( mybox.x-border, mybox.y+mybox.h+border, 
                    mybox.x, mybox.y + mybox.h,
                    mybox.x, mybox.y+mybox.h+border,
                    RA8875_DARK_GREY);

  tft.fillTriangle( mybox.x + mybox.w, mybox.y, 
                    mybox.x + mybox.w + border, mybox.y - border,
                    mybox.x + mybox.w + border, mybox.y,
                    RA8875_DARK_GREY);

  tft.drawLine(mybox.x+mybox.w, mybox.y+mybox.h, mybox.x+mybox.w + border, mybox.y+mybox.h+border, RA8875_BLACK);

}
/*****************/


/***************
bool Adafruit_RA8875 :: drawBorder(uint16_t x,uint16_t y, uint16_t w, uint16_t h, uint16_t border)
{
  if(border > 100)    // border too big 
    return false;

  this->fillRect(x-border, y-border, w + border*2, h + border*2, RA8875_WHITE);
  this->fillRect(x, y, w + border, h + border, RA8875_DARK_GREY);
  this->fillTriangle( x - border, y + h + border, 
                x, y + mybox.h,
                x, y + h + border,
                RA8875_DARK_GREY);

  this->fillTriangle( x + w, .y, 
                x + w + border, y - border,
                x + w + border, y,
                RA8875_DARK_GREY);

  this->drawLine(x + w, y + h, x + w + border, y + h + border, RA8875_BLACK);

  return true; 

}
*****************/





bool setvisiblecursor(enum RA8875tcursor c, bool blink)
{
  uint8_t cW = 0;
  uint8_t cH = 0;
  uint8_t temp_val = tft.readReg(RA8875_MWCR0);
  c == NOCURSOR ? temp_val &= ~(1 << 6) : temp_val |= (1 << 6); // turn cursor and blink off
  
  if(!blink)
  {
      temp_val &= ~(0x20);
      tft.writeReg(RA8875_MWCR0, temp_val);
  }
  
  else
  {
      temp_val|= 0x20;
      tft.writeReg(RA8875_MWCR0, temp_val); 
      switch (c) 
      {
        case IBEAM:
            cW = 0x01;
            cH = 0x1F;
            break;
        case UNDER:
            cW = 0x07;
            cH = 0x00;    // default is 0 in spec pg 29
            break;
        case BLOCK:
            cW = 0x07;
            cH = 0x1F;
            break;
        case NOCURSOR:
        default:
            break;
    }
  }
  tft.writeReg(RA8875_CURHS, cW);
  tft.writeReg(RA8875_CURVS, cH);
 return blink; 
}

/***************
 * 
 *  Blink rate (sec)  = BTCR reg value * (1/frame rate) fr = 60hz, pg 89
 *  for .5 seconds, rate = 0x1E (30)
 *  for 1 sec, rate = 0x3c (60)
 * 
 */

void setCursorBlinkRate(uint8_t rate)
{
  if(rate > 60)
    rate = 30;

  tft.writeReg(RA8875_BTCR,rate); 
  
}

// ============================================
void uploadUserChar(const uint8_t symbol[],enum RA8875_custom_font address)
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


// ===== drawsthe LCD Display image =====
void draw_map(const uint16_t scmap[],uint16_t r, uint16_t c, Box mapbox)
{
    int row, column =0;
    int j =0; 
    
    for(row=0; row<r; row++)    // was 163 for world 565, 213 for charmap fpt LCD2, 320 x144
    {
      yield(); 
      for(column=0; column<c; column++) 
      {
          //tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word(&charmap[j])); // 320x215
          //tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word(&world565[j]));  //320x163
          tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word( &(scmap[j]) ));    // GOOD
          // tft.drawPixel((column + mapbox.x), (row+mapbox.y), scmap[j]);    // GOOD
          //Serial.println(scmap[j]);
          //Serial.print("j ");
          //Serial.println(j);
          
          
          j=j+1;
      }
    }  
}






const uint8_t g1_cursor[] PROGMEM = {
0xFF,  0xFA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xEA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xEA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xEA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xEA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xEA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xEA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAB,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAB,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAB,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAB,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAB,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAF, 0xFF
};


// my ISS looking icon for test mode 
  const uint8_t bmSatLeft[]PROGMEM ={0x80, 0x80, 0x90, 0x91, 0x91, 0x91, 0x93, 0xFF, 0x93, 0x91, 0x91, 0x91, 0x90, 0x80, 0x80, 0x00 };
  const uint8_t bmSatright[]PROGMEM = {0x02, 0x02, 0x12, 0x12, 0x12, 0x12, 0x92, 0xFE, 0x92, 0x12, 0x12, 0x12, 0x12, 0x02, 0x02, 0x00 };





