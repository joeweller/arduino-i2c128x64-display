/**
 * this sketch listens for serial input and displays string to the i2c screen
 * supports 144 character strings spanning 8 lines.
 * will only display first 144 chars of string
 * all strings must end with \n (10, 0x0A)
 * each received string represents an image frame
 */

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

const unsigned char *WELCOME_MESSAGE = reinterpret_cast<const unsigned char *>("Boot Complete!\n");
unsigned char serialBuffer[64];
uint8_t lineNumber = 0;
uint8_t bufferLen = 0;

void setup() {
    Serial.begin(9600); // begin serial streamer

    // Display I2C Address (default 0x3D) for 128x64
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // don't proceed, loop forever if allocation fails
    }
    display.clearDisplay();
    if (drawTextLine_Small((unsigned char *)WELCOME_MESSAGE, (uint8_t)strlen((char *)WELCOME_MESSAGE), 0)) {
        Serial.println(F("WELCOME_MESSAGE too long"));
    }
    display.display();
    delay(2000);
}

void loop() {
    if (Serial.available() > 0) {
        bufferLen = Serial.readBytes(serialBuffer, 18);
        if (bufferLen) {

            // clear display
            if (0 == lineNumber) {
              display.clearDisplay();
            }
            
            if (drawTextLine_Small(serialBuffer, bufferLen, lineNumber)) {
                lineNumber++;
            } else {
                // end of string has been reached. refresh display
                display.display();
                Serial.println(F("frame complete"));
                lineNumber = 0;
            }
        }
    }
}

/**
 * prints string to line on display
 * 18 characters per line (display allows 8 lines or total 144 characters)
 * returns bool:
 *  true - string end character not found.
 *  false - string end character found (\n, 10, 0x0A).
 */
bool drawTextLine_Small(unsigned char* charArray, uint8_t bufferLength, uint8_t lineNumber) {
    if (lineNumber < 8) {
        for (int i = 0; i < bufferLength; i++) {
            if (0x0A != charArray[i]) {
                display.drawChar(i * 7, lineNumber * 8, charArray[i], 1, 0, 1);
            }
        }
    }
    return 0x0A != charArray[bufferLength-1];
}

/**
 * Draws medium text string to the screen
 * Currently does not work with current loop and needs modification
 * method is here simply because spacing is correct for size2 text
 */
// prints text string on display
// 10*4 characters to display (40 chars)
// wraps entire string, ignores \n and \r
void drawText_Medium(unsigned char *textString, int bufferLength)
{
    display.clearDisplay();
    int linenumber = 0;
    int characternumber = 0;
    for (int i = 0; i < bufferLength - 1; i++) {
        if (textString[i] != 10) {
            display.drawChar((characternumber * 11), (linenumber * 16), textString[i], 1, 0, 2);
        }

        if (10 == characternumber) {
            linenumber++;
            characternumber = 0;
        } else {
            characternumber++;
        }
    }
    display.display();
}
