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

#define DISPLAY_TIME_SECONDS 100
#define NUMBER_FULL_CYCLES   10

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
static int next_gif_trigger = false;

void gif_loop_3() {
    // these variables keep track of when it's time to play a new GIF
    static unsigned long displayStartTime_millis;
    static bool playNextGif = true;     // we haven't loaded a GIF yet on first pass through, make sure we do that

    // these variables keep track of when we're done displaying the last frame and are ready for a new frame
    static uint32_t lastFrameDisplayTime = 0;
    static unsigned int currentFrameDelay = 0;

    static int index = 0;

    unsigned long now = millis();

#if 1
    // default behavior is to play the gif for DISPLAY_TIME_SECONDS or for NUMBER_FULL_CYCLES, whichever comes first
    if((now - displayStartTime_millis) > (DISPLAY_TIME_SECONDS * 1000) || decoder.getCycleNumber() > NUMBER_FULL_CYCLES || next_gif_trigger)
        playNextGif = true;
        next_gif_trigger = false;
#else
    // alt behavior is to play the gif until both DISPLAY_TIME_SECONDS and NUMBER_FULL_CYCLES have passed
    if((now - displayStartTime_millis) > (DISPLAY_TIME_SECONDS * 1000) && decoder.getCycleNumber() > NUMBER_FULL_CYCLES || next_gif_trigger)
        playNextGif = true;
        next_gif_trigger = false;
#endif

    // We only decode a GIF frame if the previous frame delay is over
    if((millis() - lastFrameDisplayTime) > currentFrameDelay) {
        if(playNextGif)
        {
            playNextGif = false;

            if (openGifFilenameByIndex(GIF_DIRECTORY, index) >= 0) {
                // start decoding, skipping to the next GIF if there's an error
                if(decoder.startDecoding() < 0) {
                    playNextGif = true;
                    return;
                }

                // Calculate time in the future to terminate animation
                displayStartTime_millis = now;
            }

            // get the index for the next pass through
            if (++index >= num_files) {
                index = 0;
            }
        }

        // decode frame without delaying after decode
        int result = decoder.decodeFrame(false);

        lastFrameDisplayTime = now;
        currentFrameDelay = decoder.getFrameDelay_ms();

        // it's time to start decoding a new GIF if there was an error, and don't wait to decode
        if(result < 0) {
            playNextGif = true;
            currentFrameDelay = 0;
        }
    }
}

void next_gif() {
    scrollingLayer2.start("next_gif()", -1);
    next_gif_trigger = true;
}
