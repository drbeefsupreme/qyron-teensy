// bweh
//

#include <SD.h>
#include <GifDecoder.h>
#include <FilenameFunctions.h>
#include <global.h>

GifDecoder<kMatrixWidth, kMatrixHeight, 12> decoder;

// Teensy SD library requires trailing slash in directory name
#define GIF_DIRECTORY "/gifs/"
#define SD_CS BUILTIN_SDCARD

#define DISPLAY_TIME_SECONDS 10
#define NUMBER_FULL_CYCLES   3

int num_files;

void screenClearCallback(void) {
  backgroundLayer.fillScreen({0,0,0});
}

void updateScreenCallback(void) {
  backgroundLayer.swapBuffers();
}

void drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue) {
    backgroundLayer.drawPixel(x, y, {red, green, blue});
}

// Setup method runs once, when the sketch starts
void gif_setup() {
    decoder.setScreenClearCallback(screenClearCallback);
    decoder.setUpdateScreenCallback(updateScreenCallback);
    decoder.setDrawPixelCallback(drawPixelCallback);

    decoder.setFileSeekCallback(fileSeekCallback);
    decoder.setFilePositionCallback(filePositionCallback);
    decoder.setFileReadCallback(fileReadCallback);
    decoder.setFileReadBlockCallback(fileReadBlockCallback);

    // NOTE: new callback function required after we moved to using the external AnimatedGIF library to decode GIFs
    decoder.setFileSizeCallback(fileSizeCallback);

    Serial.println("Starting AnimatedGIFs Sketch");

    if(initFileSystem(SD_CS) < 0) {
        scrollingLayer1.start("No SD card", -1);
        Serial.println("No SD card");
        while(1);
    }

    num_files = enumerateGIFFiles(GIF_DIRECTORY, true);

    if(num_files < 0) {
        scrollingLayer1.start("No gifs directory", -1);
        Serial.println("No gifs directory");
        while(1);
    }

    if(!num_files) {
        scrollingLayer1.start("Empty gifs directory", -1);
        Serial.println("Empty gifs directory");
        while(1);
    }
}

static int gif_index = 0;
static int nextGIF = 1;

void gif_loop() {
//    static unsigned long displayStartTime_millis;
//    static int nextGIF = 1;     // we haven't loaded a GIF yet on first pass through, make sure we do that

//     unsigned long now = millis();

// #if 1
//     // default behavior is to play the gif for DISPLAY_TIME_SECONDS or for NUMBER_FULL_CYCLES, whichever comes first
//     if((now - displayStartTime_millis) > (DISPLAY_TIME_SECONDS * 1000) || decoder.getCycleNumber() > NUMBER_FULL_CYCLES)
//         nextGIF = 1;
// #else
//     // alt behavior is to play the gif until both DISPLAY_TIME_SECONDS and NUMBER_FULL_CYCLES have passed
//     if((now - displayStartTime_millis) > (DISPLAY_TIME_SECONDS * 1000) && decoder.getCycleNumber() > NUMBER_FULL_CYCLES)
//         nextGIF = 1;
// #endif

    if(nextGIF)
    {
        if (++gif_index >= num_files) {
            gif_index = 0;
        }
        nextGIF = 0;

        if (openGifFilenameByIndex(GIF_DIRECTORY, gif_index) >= 0) {
            // Can clear screen for new animation here, but this might cause flicker with short animations
            // matrix.fillScreen(COLOR_BLACK);
            // matrix.swapBuffers();

            // start decoding, skipping to the next GIF if there's an error
            if(decoder.startDecoding() < 0) {
                next_gif();
//                nextGIF = 1;
                return;
            }

            // Calculate time in the future to terminate animation
//            displayStartTime_millis = now;
        }

        // get the index for the next pass through
    }

    if(decoder.decodeFrame() < 0) {
        // There's an error with this GIF, go to the next one
//        nextGIF = 1;
        next_gif();
    }
}

void next_gif() {
    nextGIF = 1;
    gif_index++;
    // if (++gif_index >= num_files) {
    //     gif_index = 0;
    // } else {
    //     ++gif_index;
    // }

    // if (openGifFilenameByIndex(GIF_DIRECTORY, gif_index) >= 0) {
    //     if (decoder.startDecoding() < 0) {
    //         next_gif();
    //         return;
    //     }
    // }

    // if (decoder.decodeFrame () < 0) {
    //     next_gif();
    // }
}
