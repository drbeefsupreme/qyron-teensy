/*
Firmware for the Teensy 3.6 for the Hyperstitional Metamandala Syzygyzer
*/

//#define USE_ADAFRUIT_GFX_LAYERS
#include <MatrixHardware_Teensy4_ShieldV5.h> //shield firmware
#include <SmartMatrix.h> //HUB75 library
#include <simpleRPC.h> //RPC library

//local source
#include "colorwheel.c" //for feature demo
#include "gimpbitmap.h"
#include "global.h"

/*  SmartMatrix initialization, with settings for the chyron */

// Ports
#define SERIAL_DEBUG Serial //Serial port for debugging

#include <GifDecoder.h>
#include <FilenameFunctions.h>

// Teensy SD library requires trailing slash in directory name
#define K_DIRECTORY "/gifs/koyaanisqatsi/"
#define O_DIRECTORY "/gifs/obama/"
#define F_DIRECTORY "/gifs/flag/"
#define D_DIRECTORY "/gifs/drugs/"
#define J_DIRECTORY "/gifs/joker/"
#define S_DIRECTORY "/gifs/spooky/"
#define B_DIRECTORY "/gifs/subgenius/"
#define T_DIRECTORY "/gifs/transparent/"
#define A_DIRECTORY "/gifs/aj/"
#define P_DIRECTORY "/gifs/blue/"
#define H_DIRECTORY "/gifs/hank/"


//comment this line out to disable debugging
//

#define DEBUG

#ifdef DEBUG

void debug(const char* str)
{
  SERIAL_DEBUG.println(str);
}

void debug(const char* str, uint16_t val, int fmt = DEC)
{
  SERIAL_DEBUG.print(str);
  SERIAL_DEBUG.println(val, fmt);
}

//finds amount of RAM still free (untested)
int freeRam()
{
   extern int __heap_start, *__brkval;
   int v;
   return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

#else
#  define debug( ... )
#endif


//My libraries
//#include "StreamingMode.h"
//#include "SmartMatrixParser.h"
//StreamingMode streamingMode;
//SmartMatrixParser smParser(1024);
//
// constants
//const int defaultBrightness = (18*255)/100;
const int defaultBrightness = 255;
const int defaultScrollOffset = 6; //not sure if i use this
const rgb24 blackColor = {0, 0, 0};
const rgb24 redColor = {0x40, 0, 0};
const rgb24 defaultBackgroundColor = redColor;

bool blinking = false;
bool currentBG = false;

static int num_filesK;
static int num_filesO;
static int num_filesF;
static int num_filesD;
static int num_filesJ;
static int num_filesS;
static int num_filesB;
static int num_filesT;
static int num_filesP;
static int num_filesH;
static int num_filesA;

//float temperature;
//

static int currentMillis; // used in the main loop for timing

void setup() {
  delay(1000);
  Serial.begin(9600);

  delay(250);
  debug("Setup() starting");
  matrixSetup();  //initializes the matrix and demo layers
  gif_setup();

  delay(3000);

  currentMillis = millis();

  num_filesK = enumerateGIFFiles(K_DIRECTORY, false);
  num_filesO = enumerateGIFFiles(O_DIRECTORY, false);
  num_filesF = enumerateGIFFiles(F_DIRECTORY, false);
  num_filesD = enumerateGIFFiles(D_DIRECTORY, false);
  num_filesJ = enumerateGIFFiles(J_DIRECTORY, false);
  num_filesS = enumerateGIFFiles(S_DIRECTORY, false);
  num_filesB = enumerateGIFFiles(B_DIRECTORY, false);
  num_filesT = enumerateGIFFiles(T_DIRECTORY, false);
  num_filesP = enumerateGIFFiles(P_DIRECTORY, false);
  num_filesH = enumerateGIFFiles(H_DIRECTORY, false);
  num_filesA = enumerateGIFFiles(A_DIRECTORY, false);


  debug("entering loop...");
}

//TODO this shouldn't be needed, its in simpleRPC, but it can't find it in the scope of loop() for some reason
template <class... Args>
Tuple<Args...> pack(Args... args) {
  Tuple<Args...> t = {args...};

  return t;
}

static int shapes_loop = -1;
static int shapesMillis;
const int delayBetweenShapes = 250;

static int pixels_loop = -1;
static int pixelsMillis;
const uint pixelsTransitionTime = 15000;

static bool gifs_loop = 0;  // default is gif
static int gifsMillis;
const int delayBetweenGifs = 60000;

static int gif_dir = 5; // default joker loop

void loop() {
  //The following adds SmartMatrix functions to the interface to be passed over the wire to the controller
  interface(
    Serial,
    //start
    pack(&scrollingLayer1, &SMLayerScrolling<RGB_TYPE(COLOR_DEPTH), kScrollingLayerOptions>::start), "scrollingLayer1_start: display text on layer 1. @a: char* @return: none",
    pack(&scrollingLayer2, &SMLayerScrolling<RGB_TYPE(COLOR_DEPTH), kScrollingLayerOptions>::start), "scrollingLayer2_start: display text on layer 2. @a: char* @return: none",
    pack(&scrollingLayer3, &SMLayerScrolling<RGB_TYPE(COLOR_DEPTH), kScrollingLayerOptions>::start), "scrollingLayer3_start: display text on layer 3. @a: char* @return: none",
    pack(&scrollingLayer4, &SMLayerScrolling<RGB_TYPE(COLOR_DEPTH), kScrollingLayerOptions>::start), "scrollingLayer4_start: display text on layer 4. @a: char* @return: none",
    pack(&scrollingLayer5, &SMLayerScrolling<RGB_TYPE(COLOR_DEPTH), kScrollingLayerOptions>::start), "scrollingLayer5_start: display text on layer 5. @a: char* @return: none",
    pack(&scrollingLayerF, &SMLayerScrolling<RGB_TYPE(COLOR_DEPTH), kScrollingLayerOptions>::start), "scrollingLayerF_start: display text on layer F. @a: char* @return: none",

    /* //speed */
    pack(&scrollingLayer1, &SMLayerScrolling<RGB_TYPE(COLOR_DEPTH), kScrollingLayerOptions>::setSpeed), "scrollingLayer1_speed: change speed on layer 1. @a: unsigned char @return: none",
    pack(&scrollingLayer2, &SMLayerScrolling<RGB_TYPE(COLOR_DEPTH), kScrollingLayerOptions>::setSpeed), "scrollingLayer2_speed: change speed on layer 2. @a: unsigned char @return: none",
    pack(&scrollingLayer3, &SMLayerScrolling<RGB_TYPE(COLOR_DEPTH), kScrollingLayerOptions>::setSpeed), "scrollingLayer3_speed: change speed on layer 3. @a: unsigned char @return: none",
    pack(&scrollingLayer4, &SMLayerScrolling<RGB_TYPE(COLOR_DEPTH), kScrollingLayerOptions>::setSpeed), "scrollingLayer4_speed: change speed on layer 4. @a: unsigned char @return: none",
    pack(&scrollingLayer5, &SMLayerScrolling<RGB_TYPE(COLOR_DEPTH), kScrollingLayerOptions>::setSpeed), "scrollingLayer5_speed: change speed on layer 5. @a: unsigned char @return: none",
    pack(&scrollingLayerF, &SMLayerScrolling<RGB_TYPE(COLOR_DEPTH), kScrollingLayerOptions>::setSpeed), "scrollingLayerF_speed: change speed on layer F. @a: unsigned char @return: none",

    setBrightness, "setBrightness: sets brightness. @a: int @return: none.",

    //routines
    runFeatureDemo, "runFeatureDemo: runs the feature demo. @a: none @return: none.",
    drawRandomShapes, "drawRandomShapes: draws random shapes. @a: none @return: none.",
    drawRandomPixels, "drawRandomPixels: draws random pixels. @a: none @return: none.",
    hitShapes, "hitShapes: draws random shapes in parallel. @a: none @return: none.",
    hitPixels, "hitPixels: draws random pixels in paralell. @a: none @return: none.",

    //background
    setBlackBackground, "setBlackBackground: sets bg to black. @a: none @return: none.",
    setRedBackground, "setRedBackground: sets bg to red. @a: none @return: none.",
    toggleBlinking, "toggleBlinking: sets the background to flash black and red. @a: none @return: none.",

    //NOTE: YOU MUST INCLUDE THE NAME IN THE STRING!!!! IT HAS SEMANTIC MEANING!
    //gifs
    nextGif, "nextGif: select the next gif. @a: none @return: none",
    noGif, "noGif: turns off the gif. @a: none @return: none",
    enableGifsLoop, "enableGifsLoop: start cycling. @a: none @return: none",
    disableGifsLoop, "disableGifsLoop: stop cycline. @a: none @return: none",

    gifK, "gifK: koyaanisqatsi. @a: none @return: none",
    gifO, "gifO: obama. @a: none @return: none",
    gifF, "gifF: flag. @a: none @return: none",
    gifD, "gifD: drugs. @a: none @return: none",
    gifJ, "gifJ: joker. @a: none @return: none",
    gifS, "gifS: spooky. @a: none @return: none",
    gifB, "gifB: subgenius. @a: none @return: none",
    gifT, "gifT: transparent. @a: none @return: none",
    gifA, "gifA: alex jones. @a: none @return: none",
    gifP, "gifP: blue chew. @a: none @return: none",
    gifH, "gifH: hank. @a: none @return: none",

    clearLoops, "clearLoops: turns off loops. @a: none @return: none"

    //debug
//    getTemperature, "getTemperature: displays CPU temp. @a: none @return: none."
  );

  if(shapes_loop == 0) {
    shapesMillis = millis();
    shapes_loop++;
  }

  if(shapes_loop >= 0) {
    if (millis() > shapesMillis + delayBetweenShapes) {
      shapesMillis = millis();
      drawRandomShapesIteration();
      shapes_loop++;
    }
  }
  if(shapes_loop >= 10000) {
    shapes_loop = -1;
  }

  if(pixels_loop == 0) {
    pixelsMillis = millis();
    pixels_loop++;
  }

  if(pixels_loop >= 0) {
    drawRandomPixelsIteration();
    pixels_loop++;
  }
  if(pixels_loop >= 10000) {
    pixels_loop = -1;
  }

  if(gifs_loop == 0) {
      gifsMillis = millis();
      gifs_loop++;
  }
  if(gifs_loop == 1) {
      if (millis() > gifsMillis + delayBetweenGifs) {
          nextGif();
          gifs_loop = 0;
      }
  }

  if(blinking == true) {
    if(currentBG == true) {
      setBlackBackground();
      currentBG = false;
    } else {
      setRedBackground();
      currentBG = true;
    }
    delay(200);
  }

  if(gifs_loop >= 0) {
    if(gif_dir == 1) {
        gif_loop(K_DIRECTORY, num_filesK);
    }
    if(gif_dir == 2) {
        gif_loop(O_DIRECTORY, num_filesO);

    }
    if(gif_dir == 3) {
        gif_loop(F_DIRECTORY, num_filesF);

    }
    if(gif_dir == 4) {
        gif_loop(D_DIRECTORY, num_filesD);

    }
    if(gif_dir == 5) {
        gif_loop(J_DIRECTORY, num_filesJ);

    }
    if(gif_dir == 6) {
        gif_loop(S_DIRECTORY, num_filesS);

    }
    if(gif_dir == 7) {
        gif_loop(B_DIRECTORY, num_filesB);

    }
    if(gif_dir == 8) {
        gif_loop(T_DIRECTORY, num_filesT);

    }
    if(gif_dir == 9) {
        gif_loop(A_DIRECTORY, num_filesA);

    }
    if(gif_dir == 10) {
        gif_loop(P_DIRECTORY, num_filesP);

    }
    if(gif_dir == 11) {
        gif_loop(H_DIRECTORY, num_filesH);
    }
  }
}

void clearLoops() {
    pixels_loop = -1;
    shapes_loop = -1;
    gifs_loop = -1;
    gif_dir = 0;
}

void enableGifsLoop() {
    gifs_loop = 0;
}

void gifK() {
    gif_setup();
    disableGifsLoop();

    gif_dir = 1;
    gifs_loop = 0;
    nextGif();
}

void gifO() {
    gif_setup();
    disableGifsLoop();


    gif_dir = 2;
    gifs_loop = 0;
    nextGif();
}

void gifF() {
    gif_setup();
    disableGifsLoop();


    gif_dir = 3;
    gifs_loop = 0;
    nextGif();
}

void gifD() {
    gif_setup();
    disableGifsLoop();

    gif_dir = 4;
    gifs_loop = 0;
    nextGif();
}

void gifJ() {
    gif_setup();
    disableGifsLoop();

    gif_dir = 5;
    gifs_loop = 0;
    nextGif();
}

void gifS() {
    gif_setup();
    disableGifsLoop();

    gif_dir = 6;
    gifs_loop = 0;
    nextGif();
}

void gifB() {
    gif_setup();
    disableGifsLoop();

    gif_dir = 7;
    gifs_loop = 0;
    nextGif();
}

void gifT() {
    gif_setup();
    disableGifsLoop();

    gif_dir = 8;
    gifs_loop = 0;
    nextGif();
}

void gifA() {
    gif_setup();
    disableGifsLoop();

    gif_dir = 9;
    gifs_loop = 0;
    nextGif();
}

void gifP() {
    gif_setup();
    disableGifsLoop();

    gif_dir = 10;
    gifs_loop = 0;
    nextGif();
}

void gifH() {
    gif_setup();
    disableGifsLoop();

    gif_dir = 11;
    gifs_loop = 0;
    nextGif();
}

void disableGifsLoop() {
    gif_dir = 0;
    gifs_loop = -1;
    noGif();
}

void matrixSetup() {
  matrix.addLayer(&backgroundLayer);
  matrix.addLayer(&scrollingLayer1);
  matrix.addLayer(&scrollingLayer2);
  matrix.addLayer(&scrollingLayer3);
  matrix.addLayer(&scrollingLayer4);
  matrix.addLayer(&scrollingLayer5);

  //features demo layers
  matrix.addLayer(&scrollingLayerF);
  matrix.addLayer(&indexedLayer);

  matrix.begin();

  scrollingLayer1.setMode(wrapForward);
  scrollingLayer2.setMode(bounceForward);
  scrollingLayer3.setMode(bounceReverse);
  scrollingLayer4.setMode(wrapForward);
  scrollingLayer5.setMode(bounceForward);

  scrollingLayer1.setColor({0xff, 0xff, 0xff});
  scrollingLayer2.setColor({0xff, 0xf7, 0x0e});
  scrollingLayer3.setColor({0xc0, 0x23, 0x36});
  scrollingLayer4.setColor({0x00, 0x00, 0xff});
  scrollingLayer5.setColor({0xff, 0x00, 0x00});

  scrollingLayer1.setSpeed(10);
  scrollingLayer2.setSpeed(20);
  scrollingLayer3.setSpeed(40);
  scrollingLayer4.setSpeed(80);
  scrollingLayer5.setSpeed(120);

  scrollingLayer1.setFont(gohufont11b);
  scrollingLayer2.setFont(gohufont11);
  scrollingLayer3.setFont(font8x13);
  scrollingLayer4.setFont(font6x10);
  scrollingLayer5.setFont(font5x7);

  scrollingLayer4.setRotation(rotation270);
  scrollingLayer5.setRotation(rotation90);

  scrollingLayer1.setOffsetFromTop((kMatrixHeight/2) - 5);
  scrollingLayer2.setOffsetFromTop((kMatrixHeight/4) - 5);
  scrollingLayer3.setOffsetFromTop((kMatrixHeight/2 + kMatrixHeight/4) - 5);
  scrollingLayer4.setOffsetFromTop((kMatrixWidth/2 + kMatrixWidth/4) - 5);
  scrollingLayer5.setOffsetFromTop((kMatrixWidth/2 + kMatrixWidth/4) - 5);


  matrix.setBrightness(defaultBrightness);

  backgroundLayer.enableColorCorrection(true); //wat this?

}


void setBlackBackground() {
  backgroundLayer.fillScreen(blackColor);
  backgroundLayer.swapBuffers();
}

void setRedBackground() {
  backgroundLayer.fillScreen(redColor);
  backgroundLayer.swapBuffers();
}

void toggleBlinking() {
  if(blinking == true) {
    blinking = false;
  } else {
    blinking = true;
  }
}

void setBrightness(int brightness) {
  matrix.setBrightness(brightness);
}


//from FeatureDemo.ino
void drawBitmap(int16_t x, int16_t y, const gimp32x32bitmap* bitmap) {
  for(unsigned int i=0; i < bitmap->height; i++) {
    for(unsigned int j=0; j < bitmap->width; j++) {
      rgb24 pixel = { bitmap->pixel_data[(i*bitmap->width + j)*3 + 0],
                      bitmap->pixel_data[(i*bitmap->width + j)*3 + 1],
                      bitmap->pixel_data[(i*bitmap->width + j)*3 + 2] };

      backgroundLayer.drawPixel(x + j, y + i, pixel);
    }
  }
}

void runFeatureDemo() {
    backgroundLayer.fillScreen(defaultBackgroundColor);
    backgroundLayer.swapBuffers();

    scrollingLayerF.setColor({0xff, 0xff, 0xff});
    scrollingLayerF.setMode(wrapForward);
    scrollingLayerF.setSpeed(40);
    scrollingLayerF.setFont(font6x10);
    scrollingLayerF.start("THE TOO LATE SHOW WITH DR. BEELZEBUB CROW", 1);
}

void hitShapes() {
  shapes_loop = 0;
}

void hitPixels() {
  pixels_loop = 0;
}

void drawRandomShapesIteration() {
//    int i;

   const int delayBetweenShapes = 50;

   // for (i = 0; i < 10000; i += delayBetweenShapes) {
       // draw for 100ms, then update frame, repeat
//       currentMillis = millis();

       int x0, y0, x1, y1, x2, y2, radius, radius2;
       // x0,y0 pair is always on the screen
       x0 = random(matrix.getScreenWidth());
       y0 = random(matrix.getScreenHeight());

 #if 0
       // x1,y1 pair can be off screen;
       x1 = random(-matrix.getScreenWidth(), 2 * matrix.getScreenWidth());
       y1 = random(-matrix.getScreenHeight(), 2 * matrix.getScreenHeight());
 #else
       x1 = random(matrix.getScreenWidth());
       y1 = random(matrix.getScreenHeight());
 #endif
       // x2,y2 pair is on screen;
       x2 = random(matrix.getScreenWidth());
       y2 = random(matrix.getScreenHeight());

       // radius is positive, up to screen width size
       radius = random(matrix.getScreenWidth());
       radius2 = random(matrix.getScreenWidth());

       rgb24 fillColor = {(uint8_t)random(192), (uint8_t)random(192), (uint8_t)random(192)};
       rgb24 outlineColor = {(uint8_t)random(192), (uint8_t)random(192), (uint8_t)random(192)};

       switch (random(15)) {
       case 0:
           backgroundLayer.drawPixel(x0, y0, outlineColor);
           break;

       case 1:
           backgroundLayer.drawLine(x0, y0, x1, y1, outlineColor);
           break;

       case 2:
           backgroundLayer.drawCircle(x0, y0, radius, outlineColor);
           break;

       case 3:
           backgroundLayer.drawTriangle(x0, y0, x1, y1, x2, y2, outlineColor);
           break;

       case 4:
           backgroundLayer.drawRectangle(x0, y0, x1, y1, outlineColor);
           break;

       case 5:
           backgroundLayer.drawRoundRectangle(x0, y0, x1, y1, radius, outlineColor);
           break;

       case 6:
           backgroundLayer.fillCircle(x0, y0, radius, fillColor);
           break;

       case 7:
           backgroundLayer.fillTriangle(x0, y0, x1, y1, x2, y2, fillColor);
           break;

       case 8:
           backgroundLayer.fillRectangle(x0, y0, x1, y1, fillColor);
           break;

       case 9:
           backgroundLayer.fillRoundRectangle(x0, y0, x1, y1, radius, fillColor);
           break;

       case 10:
           backgroundLayer.fillCircle(x0, y0, radius, outlineColor, fillColor);
           break;

       case 11:
           backgroundLayer.fillTriangle(x0, y0, x1, y1, x2, y2, outlineColor, fillColor);
           break;

       case 12:
           backgroundLayer.fillRectangle(x0, y0, x1, y1, outlineColor, fillColor);
           break;

       case 13:
           backgroundLayer.fillRoundRectangle(x0, y0, x1, y1, radius, outlineColor, fillColor);
           break;

       case 14:
           backgroundLayer.drawEllipse(x0, y0, radius, radius2, outlineColor);

       default:
           break;
       }
   backgroundLayer.swapBuffers();
   //backgroundLayer.fillScreen({0,0,0});
//   while (millis() < currentMillis + delayBetweenShapes);
}

//TODO move the loop outside so that you can draw shapes while other things
//are happening with eg. gifs
void drawRandomShapes() {
    int i;
    unsigned long currentMillis;

    const int delayBetweenShapes = 250;

    for (i = 0; i < 10000; i += delayBetweenShapes) {
        // draw for 100ms, then update frame, repeat
        currentMillis = millis();
        int x0, y0, x1, y1, x2, y2, radius, radius2;
        // x0,y0 pair is always on the screen
        x0 = random(matrix.getScreenWidth());
        y0 = random(matrix.getScreenHeight());

 #if 0
        // x1,y1 pair can be off screen;
        x1 = random(-matrix.getScreenWidth(), 2 * matrix.getScreenWidth());
        y1 = random(-matrix.getScreenHeight(), 2 * matrix.getScreenHeight());
 #else
        x1 = random(matrix.getScreenWidth());
        y1 = random(matrix.getScreenHeight());
 #endif
        // x2,y2 pair is on screen;
        x2 = random(matrix.getScreenWidth());
        y2 = random(matrix.getScreenHeight());

        // radius is positive, up to screen width size
        radius = random(matrix.getScreenWidth());
        radius2 = random(matrix.getScreenWidth());

        rgb24 fillColor = {(uint8_t)random(192), (uint8_t)random(192), (uint8_t)random(192)};
        rgb24 outlineColor = {(uint8_t)random(192), (uint8_t)random(192), (uint8_t)random(192)};

        switch (random(15)) {
        case 0:
            backgroundLayer.drawPixel(x0, y0, outlineColor);
            break;

        case 1:
            backgroundLayer.drawLine(x0, y0, x1, y1, outlineColor);
            break;

        case 2:
            backgroundLayer.drawCircle(x0, y0, radius, outlineColor);
            break;

        case 3:
            backgroundLayer.drawTriangle(x0, y0, x1, y1, x2, y2, outlineColor);
            break;

        case 4:
            backgroundLayer.drawRectangle(x0, y0, x1, y1, outlineColor);
            break;

        case 5:
            backgroundLayer.drawRoundRectangle(x0, y0, x1, y1, radius, outlineColor);
            break;

        case 6:
            backgroundLayer.fillCircle(x0, y0, radius, fillColor);
            break;

        case 7:
            backgroundLayer.fillTriangle(x0, y0, x1, y1, x2, y2, fillColor);
            break;

        case 8:
            backgroundLayer.fillRectangle(x0, y0, x1, y1, fillColor);
            break;

        case 9:
            backgroundLayer.fillRoundRectangle(x0, y0, x1, y1, radius, fillColor);
            break;

        case 10:
            backgroundLayer.fillCircle(x0, y0, radius, outlineColor, fillColor);
            break;

        case 11:
            backgroundLayer.fillTriangle(x0, y0, x1, y1, x2, y2, outlineColor, fillColor);
            break;

        case 12:
            backgroundLayer.fillRectangle(x0, y0, x1, y1, outlineColor, fillColor);
            break;

        case 13:
            backgroundLayer.fillRoundRectangle(x0, y0, x1, y1, radius, outlineColor, fillColor);
            break;

        case 14:
            backgroundLayer.drawEllipse(x0, y0, radius, radius2, outlineColor);

        default:
            break;
        }
        backgroundLayer.swapBuffers();
        //backgroundLayer.fillScreen({0,0,0});
        while (millis() < currentMillis + delayBetweenShapes);
    }
}

void drawRandomPixelsIteration() {
    int i;
//    const uint transitionTime = 15000;

 //   backgroundLayer.fillScreen({0, 0, 0});
//    backgroundLayer.swapBuffers();

//    while (millis() - currentMillis < transitionTime) {
        int x0, y0;

        rgb24 color;
        float fraction = ((float)millis() - pixelsMillis) / ((float)pixelsTransitionTime / 2);

        if (millis() - pixelsMillis < pixelsTransitionTime / 2) {
            color.red = 255 - 255.0 * fraction;
            color.green = 255.0 * fraction;
            color.blue = 0;
        }
        else {
            color.red = 0;
            color.green = 255 - 255.0 / 2 * (fraction - 1.0);
            color.blue = 255.0 * (fraction - 1.0);
        }

        for (i = 0; i < 20; i++) {
            x0 = random(matrix.getScreenWidth());
            y0 = random(matrix.getScreenHeight());

            backgroundLayer.drawPixel(x0, y0, color);
        }
        backgroundLayer.swapBuffers();
//    }
}

void drawRandomPixels() {
    int i;
    unsigned long currentMillis;

    const uint transitionTime = 15000;

    backgroundLayer.fillScreen({0, 0, 0});
    backgroundLayer.swapBuffers();

    currentMillis = millis();

    while (millis() - currentMillis < transitionTime) {
        int x0, y0;

        rgb24 color;
        float fraction = ((float)millis() - currentMillis) / ((float)transitionTime / 2);

        if (millis() - currentMillis < transitionTime / 2) {
            color.red = 255 - 255.0 * fraction;
            color.green = 255.0 * fraction;
            color.blue = 0;
        }
        else {
            color.red = 0;
            color.green = 255 - 255.0 / 2 * (fraction - 1.0);
            color.blue = 255.0 * (fraction - 1.0);
        }

        for (i = 0; i < 20; i++) {
            x0 = random(matrix.getScreenWidth());
            y0 = random(matrix.getScreenHeight());

            backgroundLayer.drawPixel(x0, y0, color);
        }
        backgroundLayer.swapBuffers();
    }
}

void nextGif() {
  SERIAL_DEBUG.println("nextGif()");
  next_gif();
}

void noGif() {
  no_gif();
}

//debug fns

//TODO why doesn't this work?
// void getTemperature() {
//   scrollingLayerF.setColor({0xff, 0xff, 0xff});
//   scrollingLayerF.setMode(wrapForward);
//   scrollingLayerF.setSpeed(40);
//   scrollingLayerF.setFont(font6x10);
//   temperature = InternalTemperature.readTemperatureF();
//   char array[10];
//   sprintf(array, "%f", temperature);
//   scrollingLayer3.start(array, -1);
// }
