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

const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

const unsigned char *WELCOME_MESSAGE = reinterpret_cast<const unsigned char *>("Boot Complete!\n");
unsigned int LINE_BUFFER[3][8];
int BUFFER_CURRENT_Y; // current line to write to in local buffer
int BUFFER_WRITTEN_Y; // count to 64 of written display buffer lines
int Y;

void setup()
{
    Serial.begin(9600); // begin serial streamer

    // Display I2C Address (default 0x3D) for 128x64
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        // Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // don't proceed, loop forever if allocation fails
    }

    randomSeed(analogRead(0));
    // Serial.println("Boot Sequence Complete");
    display.clearDisplay();
    drawTextLine_Small((unsigned char *)WELCOME_MESSAGE, (uint8_t)strlen((char *)WELCOME_MESSAGE), 0);
    display.display();

    // delay(2000);
    initGrid();
}

void loop()
{
    // Serial.println('start loop');
    BUFFER_WRITTEN_Y = 0;
    BUFFER_CURRENT_Y = 0;

    for (Y = 0; Y < SCREEN_HEIGHT; Y++)
    {
        // Serial.println('start y');
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            writePixelBuffer(x, checkCellStatus(x, Y));
            // display.drawPixel(x, y, checkCellStatus(x, y));
        }
        // Serial.println('finished writing x');
        updateDisplayBuffer();
    }

    Serial.println(SCREEN_HEIGHT - BUFFER_WRITTEN_Y);
    Serial.println(SCREEN_HEIGHT - BUFFER_WRITTEN_Y);
    flushDisplayBuffer();

    display.display();
}

const void initGrid()
{
    display.clearDisplay();
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            display.drawPixel(x, y, random(50) < 20);
        }
    }
    display.display();
}

bool checkCellStatus(const int x, const int y)
{
    bool result = false;
    bool currentLifeStatus = display.getPixel(x, y);
    const int neighboughAliveCount = getNeighbourAliveCount(x, y);

    if (currentLifeStatus)
    {
        if (neighboughAliveCount == 2 || neighboughAliveCount == 3)
        {
            result = true;
        }
    }
    else
    {
        if (neighboughAliveCount == 3)
        {
            result = true;
        }
    }
    return result;
}

int getNeighbourAliveCount(const int x, const int y)
{
    int count = 0;
    count += (int)display.getPixel(x - 1, y + 1);
    count += (int)display.getPixel(x, y + 1);
    count += (int)display.getPixel(x + 1, y + 1);
    count += (int)display.getPixel(x - 1, y);
    count += (int)display.getPixel(x + 1, y);
    count += (int)display.getPixel(x - 1, y - 1);
    count += (int)display.getPixel(x, y - 1);
    count += (int)display.getPixel(x + 1, y - 1);
    return count;
}

void updateDisplayBuffer()
{
    if (Y > 0)
    {
        if (BUFFER_CURRENT_Y == 0)
        {
            writeDisplayBufferLine(2);
            BUFFER_CURRENT_Y++;
        }
        else if (BUFFER_CURRENT_Y == 1)
        {
            writeDisplayBufferLine(0);
            BUFFER_CURRENT_Y++;
        }
        else if (BUFFER_CURRENT_Y == 2)
        {
            writeDisplayBufferLine(1);
            BUFFER_CURRENT_Y = 0;
        }
    }
}

void writeDisplayBufferLine(const int writableBufferIndex)
{
    int intChunks = SCREEN_WIDTH / 16;
    for (int intIndex = 0; intIndex < intChunks; intIndex++)
    {
        int xIndex = (intIndex * 16);
        unsigned int tmp = LINE_BUFFER[writableBufferIndex][intIndex];
        display.drawPixel(xIndex, BUFFER_WRITTEN_Y, (tmp >> 15) & 1);
        display.drawPixel(xIndex + 1, BUFFER_WRITTEN_Y, (tmp >> 14) & 1);
        display.drawPixel(xIndex + 2, BUFFER_WRITTEN_Y, (tmp >> 13) & 1);
        display.drawPixel(xIndex + 3, BUFFER_WRITTEN_Y, (tmp >> 12) & 1);
        display.drawPixel(xIndex + 4, BUFFER_WRITTEN_Y, (tmp >> 11) & 1);
        display.drawPixel(xIndex + 5, BUFFER_WRITTEN_Y, (tmp >> 10) & 1);
        display.drawPixel(xIndex + 6, BUFFER_WRITTEN_Y, (tmp >> 9) & 1);
        display.drawPixel(xIndex + 7, BUFFER_WRITTEN_Y, (tmp >> 8) & 1);
        display.drawPixel(xIndex + 8, BUFFER_WRITTEN_Y, (tmp >> 7) & 1);
        display.drawPixel(xIndex + 9, BUFFER_WRITTEN_Y, (tmp >> 6) & 1);
        display.drawPixel(xIndex + 10, BUFFER_WRITTEN_Y, (tmp >> 5) & 1);
        display.drawPixel(xIndex + 11, BUFFER_WRITTEN_Y, (tmp >> 4) & 1);
        display.drawPixel(xIndex + 12, BUFFER_WRITTEN_Y, (tmp >> 3) & 1);
        display.drawPixel(xIndex + 13, BUFFER_WRITTEN_Y, (tmp >> 2) & 1);
        display.drawPixel(xIndex + 14, BUFFER_WRITTEN_Y, (tmp >> 1) & 1);
        display.drawPixel(xIndex + 15, BUFFER_WRITTEN_Y, tmp & 1);
        LINE_BUFFER[writableBufferIndex][intIndex] = 0; // reset buffer
    }
    BUFFER_WRITTEN_Y++;
}

void flushDisplayBuffer()
{
    for (; BUFFER_WRITTEN_Y < SCREEN_HEIGHT;)
    {
        updateDisplayBuffer();
    }
}

void writePixelBuffer(const int x, const int val)
{
    int innerInt = (int)x / 16;
    LINE_BUFFER[BUFFER_CURRENT_Y][innerInt] =
        (LINE_BUFFER[BUFFER_CURRENT_Y][innerInt] << 1) | val;
}

/**
 * prints string to line on display
 * 18 characters per line (display allows 8 lines or total 144 characters)
 * returns bool:
 *  true - string end character not found.
 *  false - string end character found (\n, 10, 0x0A).
 */
bool drawTextLine_Small(unsigned char *charArray, uint8_t bufferLength, uint8_t lineNumber)
{
    if (lineNumber < 8)
    {
        for (int i = 0; i < bufferLength; i++)
        {
            if (0x0A != charArray[i])
            {
                display.drawChar(i * 7, lineNumber * 8, charArray[i], 1, 0, 1);
            }
        }
    }
    return 0x0A != charArray[bufferLength - 1];
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
    for (int i = 0; i < bufferLength - 1; i++)
    {
        if (textString[i] != 10)
        {
            display.drawChar((characternumber * 11), (linenumber * 16), textString[i], 1, 0, 2);
        }

        if (10 == characternumber)
        {
            linenumber++;
            characternumber = 0;
        }
        else
        {
            characternumber++;
        }
    }
    display.display();
}
