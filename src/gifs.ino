// bweh
//

#include <SD.h>
#include <GifDecoder.h>
#include <FilenameFunctions.h>
#include <global.h>

GifDecoder<kMatrixWidth, kMatrixHeight, 12> decoder;

#define SD_CS BUILTIN_SDCARD

#define DISPLAY_TIME_SECONDS 15
#define NUMBER_FULL_CYCLES   10

bool gif_bool = false;

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
    Serial.println("Starting GIF decoder setup");

    decoder.setScreenClearCallback(screenClearCallback);
    decoder.setUpdateScreenCallback(updateScreenCallback);
    decoder.setDrawPixelCallback(drawPixelCallback);

    decoder.setFileSeekCallback(fileSeekCallback);
    decoder.setFilePositionCallback(filePositionCallback);
    decoder.setFileReadCallback(fileReadCallback);
    decoder.setFileReadBlockCallback(fileReadBlockCallback);

    // NOTE: new callback function required after we moved to using the external AnimatedGIF library to decode GIFs
    decoder.setFileSizeCallback(fileSizeCallback);

    Serial.println("Performing GIF checks...");

    if(initFileSystem(SD_CS) < 0) {
        scrollingLayer1.start("No SD card", -1);
        Serial.println("No SD card");
        while(1);
    }

// //    num_files = enumerateGIFFiles(GIF_DIRECTORY, true);

//     if(num_files < 0) {
//         scrollingLayer1.start("No gifs directory", -1);
//         Serial.println("No gifs directory");
//         while(1);
//     }

//     if(!num_files) {
//         scrollingLayer1.start("Empty gifs directory", -1);
//         Serial.println("Empty gifs directory");
//         while(1);
//     }
}

static int next_gif_trigger = false;

void gif_loop(char *dirname, int num_files) {
    // if(gif_bool) {

        // these variables keep track of when it's time to play a new GIF
        static bool playNextGif = true;     // we haven't loaded a GIF yet on first pass through, make sure we do that
        static unsigned long displayStartTime_millis;

        // these variables keep track of when we're done displaying the last frame and are ready for a new frame
        static uint32_t lastFrameDisplayTime = 0;
        static unsigned int currentFrameDelay = 0;

        static int index = 0;

        unsigned long now = millis();

        if(next_gif_trigger) {
                playNextGif = true;
                next_gif_trigger = false;
        }


        // We only decode a GIF frame if the previous frame delay is over
        if((millis() - lastFrameDisplayTime) > currentFrameDelay) {
            if(playNextGif)
            {
                playNextGif = false;

                if (openGifFilenameByIndex(dirname, index) >= 0) {
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

            // if((now - displayStartTime_millis) > DISPLAY_TIME_SECONDS) {
            //     playNextGif = true;
            //     next_gif_trigger = true;
            // }
        }
//    }
}

void next_gif() {
    next_gif_trigger = true;
    gif_bool = true;
}

void no_gif() {
    gif_bool = false;
}
