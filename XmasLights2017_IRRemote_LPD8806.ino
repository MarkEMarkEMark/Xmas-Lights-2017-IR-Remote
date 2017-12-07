#include <FastLED.h>
#include <IRremote2.h>

#define NUM_LEDS 169 //really 160, but nearest square root is 13
#define SQRT_LEDS 13
#define CHIPSET    LPD8806
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
CRGB fakeLeds[NUM_LEDS]; //unused LEDs for FastLED calculations
CRGB fakeLEDsFB[2][NUM_LEDS]; //unused LEDs for FastLED calculations Front/Back

#define NUM_PATTERNS 29 //including Off (0) and Solid Colour (Last)
#define NUM_CROSSFADES 1
//#define stayFrames 3600 //number of frames to show a program - if multiple of half num pixels, then some programs won't run into each other (eg random strobe fade)
#define MAX_LEVEL 256 //max R,G,B or S, V levels (note: need to subtract one to make 0 based)
#define PI 3.14159265
#define DAMPING 0.92

#define SETUP_VAR_NOTRANDOM true

//IR-Remote - air conditioner via module
#define IRA01 0x004FB40BF //ON/OFF
#define IRA02 0x004FB906F //Mode
#define IRA03 0x004FB58A7 //Fan Speed
#define IRA04 0x004FB50AF //Up
#define IRA05 0x004FB9867 //Timer
#define IRA06 0x004FB8877 //Down
//IR-Remote - air conditioner via direct component
#define IRB01 0x00BBF4F9F //ON/OFF
#define IRB02 0x0DF6E70A3 //Mode
#define IRB03 0x0D5A04AFC //Fan Speed
#define IRB04 0x02427CC83 //Up
#define IRB05 0x090E6EF1F //Timer
#define IRB06 0x098934743 //Down
// IR Raw Key Codes for eTopxizu 44Key IR Remote Controller for 5050 3528 RGB LED Light Strip
#define IRCODE_ETOPXIZU_HELD            0x7FFFFFFF // 4294967295
#define IRCODE_ETOPXIZU_POWER           16712445
#define IRCODE_ETOPXIZU_PLAY_PAUSE      16745085
#define IRCODE_ETOPXIZU_BRIGHTNESS_UP   16726725
#define IRCODE_ETOPXIZU_BRIGHTNESS_DOWN 16759365
#define IRCODE_ETOPXIZU_DIY1            16724175
#define IRCODE_ETOPXIZU_DIY2            16756815
#define IRCODE_ETOPXIZU_DIY3            16740495
#define IRCODE_ETOPXIZU_DIY4            16716015
#define IRCODE_ETOPXIZU_DIY5            16748655
#define IRCODE_ETOPXIZU_DIY6            16732335
#define IRCODE_ETOPXIZU_JUMP3           16720095
#define IRCODE_ETOPXIZU_JUMP7           16752735
#define IRCODE_ETOPXIZU_FADE3           16736415
#define IRCODE_ETOPXIZU_FADE7           16769055
#define IRCODE_ETOPXIZU_FLASH           16764975
#define IRCODE_ETOPXIZU_AUTO            16773135
#define IRCODE_ETOPXIZU_QUICK           16771095
#define IRCODE_ETOPXIZU_SLOW            16762935
#define IRCODE_ETOPXIZU_RED_UP          16722135
#define IRCODE_ETOPXIZU_RED_DOWN        16713975
#define IRCODE_ETOPXIZU_GREEN_UP        16754775
#define IRCODE_ETOPXIZU_GREEN_DOWN      16746615
#define IRCODE_ETOPXIZU_BLUE_UP         16738455
#define IRCODE_ETOPXIZU_BLUE_DOWN       16730295
#define IRCODE_ETOPXIZU_RED             16718565
#define IRCODE_ETOPXIZU_RED_ORANGE      16722645
#define IRCODE_ETOPXIZU_ORANGE          16714485
#define IRCODE_ETOPXIZU_YELLOW_ORANGE   16726215
#define IRCODE_ETOPXIZU_YELLOW          16718055
#define IRCODE_ETOPXIZU_GREEN           16751205
#define IRCODE_ETOPXIZU_LIME            16755285
#define IRCODE_ETOPXIZU_AQUA            16747125
#define IRCODE_ETOPXIZU_TEAL            16758855
#define IRCODE_ETOPXIZU_NAVY            16750695
#define IRCODE_ETOPXIZU_BLUE            16753245
#define IRCODE_ETOPXIZU_ROYAL_BLUE      16749165
#define IRCODE_ETOPXIZU_PURPLE          16757325
#define IRCODE_ETOPXIZU_INDIGO          16742535
#define IRCODE_ETOPXIZU_MAGENTA         16734375
#define IRCODE_ETOPXIZU_WHITE           16720605
#define IRCODE_ETOPXIZU_PINK            16716525
#define IRCODE_ETOPXIZU_LIGHT_PINK      16724685
#define IRCODE_ETOPXIZU_BABY_BLUE       16775175
#define IRCODE_ETOPXIZU_LIGHT_BLUE      16767015

//easier to read command chars
#define CMD_POWER        'A'
#define CMD_BRIGHT_UP    'B'
#define CMD_BRIGHT_DOWN  'C'
#define CMD_SPEED_UP     'D'
#define CMD_SPEED_DOWN   'E'
#define CMD_PATTERN_UP   'F'
#define CMD_PATTERN_DOWN 'G'
#define CMD_CUSTOM1_UP    'H'
#define CMD_CUSTOM1_DOWN  'I'
#define CMD_CUSTOM2_UP    'J'
#define CMD_CUSTOM2_DOWN  'K'
#define CMD_CUSTOM3_UP    'L'
#define CMD_CUSTOM3_DOWN  'M'
#define CMD_RED           'N'
#define CMD_RED_ORANGE    'O'
#define CMD_ORANGE        'P'
#define CMD_YELLOW_ORANGE 'Q'
#define CMD_YELLOW        'R'
#define CMD_GREEN         'S'
#define CMD_LIME          'T'
#define CMD_AQUA          'U'
#define CMD_TEAL          'V'
#define CMD_NAVY          'W'
#define CMD_BLUE          'X'
#define CMD_ROYAL_BLUE    'Y'
#define CMD_PURPLE        'Z'
#define CMD_INDIGO        '0'
#define CMD_MAGENTA       '1'
#define CMD_WHITE         '2'
#define CMD_PINK          '3'
#define CMD_LIGHT_PINK    '4'
#define CMD_BABY_BLUE     '5'
#define CMD_LIGHT_BLUE    '6'
#define CMD_AUTO          '7'


const TProgmemRGBPalette16 RGB_p FL_PROGMEM =
{ CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Red };

const TProgmemRGBPalette16 RGBY_p FL_PROGMEM =
        {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow }; 

// A mostly red palette with green accents and white trim.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM =
{ CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray,
CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green };

// A mostly (dark) green palette with red berries.
#define Holly_Green 0x00580c
#define Holly_Red   0xB00402
const TProgmemRGBPalette16 Holly_p FL_PROGMEM =
{ Holly_Green, Holly_Green, Holly_Green, Holly_Green,
Holly_Green, Holly_Green, Holly_Green, Holly_Green,
Holly_Green, Holly_Green, Holly_Green, Holly_Green,
Holly_Green, Holly_Green, Holly_Green, Holly_Red
};

// A red and white striped palette
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedWhite_p FL_PROGMEM =
{ CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray,
CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray };

// A mostly blue palette with white accents.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM =
{ CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray };

// A pure "fairy light" palette with some brightness variations
#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM =
{ CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight,
HALFFAIRY, HALFFAIRY, CRGB::FairyLight, CRGB::FairyLight,
QUARTERFAIRY, QUARTERFAIRY, CRGB::FairyLight, CRGB::FairyLight,
CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight };

// A palette of soft snowflakes with the occasional bright one
const TProgmemRGBPalette16 Snow_p FL_PROGMEM =
{ 0x304048, 0x304048, 0x304048, 0x304048,
0x304048, 0x304048, 0x304048, 0x304048,
0x304048, 0x304048, 0x304048, 0x304048,
0x304048, 0x304048, 0x304048, 0xE0F0FF };

// A palette reminiscent of large 'old-school' C9-size tree lights
// in the five classic colors: red, orange, green, blue, and white.
#define C9_Red    0xB80400
#define C9_Orange 0x902C02
#define C9_Green  0x046002
#define C9_Blue   0x070758
#define C9_Pink  0x982430
const TProgmemRGBPalette16 RetroC9_p FL_PROGMEM =
{ C9_Red, C9_Orange, C9_Red, C9_Orange,
C9_Orange, C9_Red, C9_Orange, C9_Red,
C9_Green, C9_Green, C9_Green, C9_Green,
C9_Blue, C9_Blue, C9_Blue,
C9_Pink
};

// A cold, icy pale blue palette
#define Ice_Blue1 0x0C1040
#define Ice_Blue2 0x182080
#define Ice_Blue3 0x5080C0
const TProgmemRGBPalette16 Ice_p FL_PROGMEM =
{
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue2, Ice_Blue2, Ice_Blue2, Ice_Blue3
};

// Lucy Palette
#define Lucy_Blue1 0x00CED1
#define Lucy_Blue2 0x48D1CC
#define Lucy_Blue3 0x00E5EE
#define Lucy_Blue4 0x40E0D0
#define Lucy_Blue5 0xAFEEEE
const TProgmemRGBPalette16 LucyBlue FL_PROGMEM =
{
  Lucy_Blue1, Lucy_Blue1, Lucy_Blue1, Lucy_Blue1,
  Lucy_Blue4, Lucy_Blue4, Lucy_Blue4, Lucy_Blue5,
  Lucy_Blue1, Lucy_Blue1, Lucy_Blue1, Lucy_Blue1,
  Lucy_Blue2, Lucy_Blue2, Lucy_Blue2, Lucy_Blue3
};

extern const TProgmemRGBPalette16 HulkGreenPurple_p FL_PROGMEM =
{
  0x00FF00, 0x5500AB, 0x7F0081, 0xAB0055,
  0x5500AB, 0x7F0081, 0xAB0055, 0xD5002B,
  0x2A00D5, 0x7F0081, 0xAB0055, 0xD5002B,
  0x00FF00, 0x7F0081, 0xAB0055, 0xD5002B
};

// Add or remove palette names from this list to control which color
// palettes are used, and in what order.
const TProgmemRGBPalette16* ActivePaletteList[] = {
  &FairyLight_p,  
  &HulkGreenPurple_p, 
  &BlueWhite_p, 
  &RetroC9_p, 
  &RainbowColors_p, 
  &RedGreenWhite_p,
  &LucyBlue,  
  &PartyColors_p,   
  &RedWhite_p, 
  &RGBY_p, 
  &Snow_p,   
  &Holly_p,  
  &Ice_p,
  &RGB_p
};

// Simplex Noise
//(inspired by happyinmotion: http://happyinmotion.livejournal.com/278357.html)
// Simplex noise for whole strip of LEDs.
// (Well, it's simplex noise for 6 nodes and cubic interpolation between those nodes.)
#define ONE_THIRD 0.333333333
#define ONE_SIXTH 0.166666667
//#define numSpacing 10 //was 4
#define FULL_ONE_MINUS 255 //level range
int ii, jj, kk, AA[] = { 0, 0, 0 };
float uu, vv, ww, ss;
int TT[] = { 0x15, 0x38, 0x32, 0x2c, 0x0d, 0x13, 0x07, 0x2a };

//Interpretted Buttons Commands
int nextSelectedPattern = 27;
int lastSelectedPattern = 27;
int beforeOffSelectedPattern = 27;
bool patternSelected = false;
int brightness = 256; //remmeber to subtract 1 - 256 for easy maths
int myParam1 = 5; //light program specific parameter - e.g. density of soft twinkles
int myParam2 = 4; //light program specific parameter - e.g. density of soft twinkles
int speedDelay = 3; //

//states
bool desireOff;
bool desireRandom = true;
bool useCustomColour = false;
int customColour1_R = 0;
int customColour1_G = 0;
int customColour1_B = 255;
int customColour2_R = 0;
int customColour2_G = 255;
int customColour2_B = 0;

int longFrame = 2147483647;
int shortFrame = 6000;
int stayFrames = shortFrame;
int longFade = 240;
int shortFade = 30;
int fadeFrames = longFade;

CRGB twinkleLightColour(8, 7, 1); //softTwinkle note GRB
CRGBPalette16 gCurrentPalette;
CRGBPalette16 gTargetPalette;
// How often to change color palettes.
#define SECONDS_PER_PALETTE  30
// If AUTO_SELECT_BACKGROUND_COLOR is set to 1,
// then for any palette where the first two entries
// are the same, a dimmed version of that color will
// automatically be used as the background color.
#define AUTO_SELECT_BACKGROUND_COLOR 0
// Background color for 'unlit' pixels
// Can be set to CRGB::Black if desired.
CRGB gBackgroundColor = CRGB::Black;
// Example of dim incandescent fairy light background color
// CRGB gBackgroundColor = CRGB(CRGB::FairyLight).nscale8_video(16);
// If COOL_LIKE_INCANDESCENT is set to 1, colors will
// fade out slighted 'reddened', similar to how
// incandescent bulbs change color as they get dim down.
#define COOL_LIKE_INCANDESCENT 0

bool varChangeUp = true;

//table of powers for damping used in ProgramRandomSplash [pow(damping, frame) - damping^frame]
//#define damping 0.90
float dampingTable[100];
int ditheredTable[NUM_LEDS];
int randomTable[2][NUM_LEDS];

// for sparks/fire
int fireTable[2][2][NUM_LEDS];

// for slow fill - stationary bulbs (idx, pixel, color, current/next)
int fxPixelBuffer[2][NUM_LEDS][3][2];


// Principle of operation: at any given time, the LEDs depict an image or
// animation effect (referred to as the "back" image throughout this code).
// Periodically, a transition to a new image or animation effect (referred
// to as the "front" image) occurs.  During this transition, a third buffer
// (the "alpha channel") determines how the front and back images are
// combined; it represents the opacity of the front image.  When the
// transition completes, the "front" then becomes the "back," a new front
// is chosen, and the process repeats.
//byte pixelState[2][NUM_LEDS * 3]; // Data for 2 pixelStrings worth of imagery

byte backImgIdx = 0, // Index of 'back' image (always 0 or 1)
fxIdx[3]; // Effect # for back & front images + alpha
bool fxInitialised[3]; // Whether to set initialisation variables, or already fxInitialised
int fxIntVars[3][13], // Effect instance variables (explained later)
fxArrVars[3][2][NUM_LEDS], // MEO: 2 x Array
fxFrameCount[3], // MEO: current overall frame count of single effect
fxFrameDelay[3], // MEO: if too fast - can set number of frames to pause
fxFrameDelayCount[3], // MEO: counter for fxFrameDelay
tCounter = -2, // Countdown to next transition
transitionTime; // Duration (in frames) of current transition
float fxFltVars[3][7]; // MEO: float variables
bool fxBlnVars[3][3]; // MEO: bool variables
int fxCustomColour[2][8]; //r, g, b, hue, secondary r,g,b,hue
int fxNextVariationNo[NUM_PATTERNS]; // - pattern Note if add more patterns, then need to increase this value
int fxCurrentVariationNo[2]; // 2 - idx front/back
int fxDefaultSliders[3][3]; // 0 will be slider 2, 1 = 3, 2 = slider 4
int fxActualSliders[3][3]; // 0 will be slider 2, 1 = 3, 2 = slider 4

//so sliders don't affect anything until turned
//bool isNewPattern = true;
int rememberSlider2; //speed/delay
int rememberSlider3;
int remmeberSlider4;
int delayCounterFront, delayCounterBack;

//bool randProg = true;
//bool lightsOn = true;

// Light Program number for back & front images
int patternNo[2];
// Crossfade no
int xFadeNo;
bool isInitialised[2]; // note crossfades not using this yet
uint8_t fadeMask[NUM_LEDS];

float fast_sin(float x)
{
  if (x < 0.5f)
  {
    return -16.0f*x*x + 8.0f*x;
  }
  else
  {
    return 16.0f*x*x - 24.0f*x + 8.0f;
  }
};


//prototypes:
volatile void LightFrame(void);
void HSVtoRGB(int h, int s, int v, int wheelLine, int color[3]);
void RGBtoHSV(int r, int g, int b, int hsv[3]);


//Infra-red recieve
int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  for (int table = 0; table < 100; table++) {
    dampingTable[table] = (float)pow(DAMPING, table);
  }

  delay(3000); // power-up safety delay
  FastLED.addLeds<CHIPSET, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 0, COLOR_ORDER>(fakeLeds, NUM_LEDS); //Fake LEDs
  FastLED.addLeds<CHIPSET, 0, COLOR_ORDER>(fakeLEDsFB[0], NUM_LEDS); //Fake LEDs
  FastLED.addLeds<CHIPSET, 0, COLOR_ORDER>(fakeLEDsFB[1], NUM_LEDS); //Fake LEDs

  fxActualSliders[0][0] = 0; fxActualSliders[1][0] = 0;
  delayCounterBack = 0; delayCounterFront = 0;

  irrecv.enableIRIn(); // Start the receiver

  patternNo[0] = 27; patternNo[1] = 27; //soft warm twinkles
  brightness = 256;

  chooseNextColorPalette(gTargetPalette); //TWINKLE ONLY

  //Serial.begin(9600);
}

void loop() {
  CheckIRRemote();

  FastLED.setBrightness(brightness - 1);

  int actualDelay = 1 << speedDelay; // << is to n to the power of 2
  FastLED.delay(actualDelay);

  //TWINKLE ONLY
  EVERY_N_SECONDS(SECONDS_PER_PALETTE) {
    chooseNextColorPalette(gTargetPalette);
  }
  EVERY_N_MILLISECONDS(10) {
    nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
  }
  LightFrame();
}

volatile void CheckIRRemote(void) {
  if (irrecv.decode(&results)) {
    char keyCommand = ' ';

    switch (results.value) {
    case IRA01:
    case IRB01:
    case IRCODE_ETOPXIZU_POWER:
      keyCommand = CMD_POWER;
      //Serial.print("Power: ");
      break;
    case IRA02:
    case IRB02:
    case IRCODE_ETOPXIZU_BRIGHTNESS_DOWN:
      keyCommand = CMD_BRIGHT_DOWN;
      //Serial.print("Brightmess Down: ");
      break;
    case IRCODE_ETOPXIZU_BRIGHTNESS_UP:
      keyCommand = CMD_BRIGHT_UP;
      //Serial.print("Brightness Up: ");
      break;
    case IRA03:
    case IRB03:
    case IRCODE_ETOPXIZU_SLOW:
      keyCommand = CMD_SPEED_DOWN;
      //Serial.print("Speed Down: ");
      break;
    case IRCODE_ETOPXIZU_QUICK:
      keyCommand = CMD_SPEED_UP;
      //Serial.print("Speed Up: ");
      break;
    case IRA04:
    case IRB04:
    case IRCODE_ETOPXIZU_PLAY_PAUSE:
      keyCommand = CMD_PATTERN_UP;
      //Serial.print("Pattern Up: ");
      break;
    case IRA05:
    case IRB05:
    case IRCODE_ETOPXIZU_DIY1:
      keyCommand = CMD_CUSTOM1_UP;
      //Serial.print("Custom 1 Up: ");
      break;
    case IRCODE_ETOPXIZU_DIY4:
      keyCommand = CMD_CUSTOM1_DOWN;
      //Serial.print("Custom 1 Down: ");
      break;
    case IRCODE_ETOPXIZU_DIY2:
      keyCommand = CMD_CUSTOM2_UP;
      //Serial.print("Custom 2 Up: ");
      break;
    case IRCODE_ETOPXIZU_DIY5:
      keyCommand = CMD_CUSTOM2_DOWN;
      //Serial.print("Custom 2 Down: ");
      break;
    case IRCODE_ETOPXIZU_DIY3:
      keyCommand = CMD_CUSTOM3_UP;
      //Serial.print("Custom 3 Up: ");
      break;
    case IRCODE_ETOPXIZU_DIY6:
      keyCommand = CMD_CUSTOM3_DOWN;
      //Serial.print("Custom 3 Down: ");
      break;
    case IRA06:
    case IRB06:
    case IRCODE_ETOPXIZU_FLASH:
      keyCommand = CMD_PATTERN_DOWN;
      //Serial.print("Pattern Down: ");
      break;
    case IRCODE_ETOPXIZU_RED:
      customColour1_R = 255;
      customColour1_G = 0;
      customColour1_B = 0;
      customColour2_R = 0;
      customColour2_G = 255;
      customColour2_B = 255;
      keyCommand = CMD_RED;
      break;
    case IRCODE_ETOPXIZU_RED_ORANGE:
      customColour1_R = 255;
      customColour1_G = 69;
      customColour1_B = 0;
      customColour2_R = 0;
      customColour2_G = 186;
      customColour2_B = 255;
      keyCommand = CMD_RED_ORANGE;
      break;
    case IRCODE_ETOPXIZU_ORANGE:
      customColour1_R = 255;
      customColour1_G = 165;
      customColour1_B = 0;
      customColour2_R = 0;
      customColour2_G = 90;
      customColour2_B = 255;
      keyCommand = CMD_ORANGE;
      break;
    case IRCODE_ETOPXIZU_YELLOW_ORANGE:
      customColour1_R = 255;
      customColour1_G = 204;
      customColour1_B = 0;
      customColour2_R = 0;
      customColour2_G = 51;
      customColour2_B = 255;
      keyCommand = CMD_YELLOW_ORANGE;
      break;
    case IRCODE_ETOPXIZU_YELLOW:
      customColour1_R = 255;
      customColour1_G = 255;
      customColour1_B = 0;
      customColour2_R = 0;
      customColour2_G = 0;
      customColour2_B = 255;
      keyCommand = CMD_YELLOW;
      break;
    case IRCODE_ETOPXIZU_GREEN:
      customColour1_R = 0;
      customColour1_G = 165;
      customColour1_B = 0;
      customColour2_R = 255;
      customColour2_G = 0;
      customColour2_B = 255;
      keyCommand = CMD_GREEN;
      break;
    case IRCODE_ETOPXIZU_LIME:
      customColour1_R = 50;
      customColour1_G = 205;
      customColour1_B = 50;
      customColour2_R = 205;
      customColour2_G = 50;
      customColour2_B = 205;
      keyCommand = CMD_LIME;
      break;
    case IRCODE_ETOPXIZU_AQUA:
      customColour1_R = 0;
      customColour1_G = 255;
      customColour1_B = 255;
      customColour2_R = 255;
      customColour2_G = 0;
      customColour2_B = 0;
      keyCommand = CMD_AQUA;
      break;
    case IRCODE_ETOPXIZU_TEAL:
      customColour1_R = 0;
      customColour1_G = 128;
      customColour1_B = 128;
      customColour2_R = 255;
      customColour2_G = 127;
      customColour2_B = 127;
      keyCommand = CMD_TEAL;
      break;
    case IRCODE_ETOPXIZU_NAVY:
      customColour1_R = 0;
      customColour1_G = 0;
      customColour1_B = 128;
      customColour2_R = 255;
      customColour2_G = 255;
      customColour2_B = 127;
      keyCommand = CMD_NAVY;
      break;
    case IRCODE_ETOPXIZU_BLUE:
      customColour1_R = 0;
      customColour1_G = 0;
      customColour1_B = 255;
      customColour2_R = 255;
      customColour2_G = 255;
      customColour2_B = 0;
      keyCommand = CMD_BLUE;
      break;
    case IRCODE_ETOPXIZU_ROYAL_BLUE:
      customColour1_R = 65;
      customColour1_G = 105;
      customColour1_B = 225;
      customColour2_R = 190;
      customColour2_G = 155;
      customColour2_B = 30;
      keyCommand = CMD_ROYAL_BLUE;
      break;
    case IRCODE_ETOPXIZU_PURPLE:
      customColour1_R = 238;
      customColour1_G = 130;
      customColour1_B = 238;
      customColour2_R = 17;
      customColour2_G = 125;
      customColour2_B = 17;
      keyCommand = CMD_PURPLE;
      break;
    case IRCODE_ETOPXIZU_INDIGO:
      customColour1_R = 75;
      customColour1_G = 0;
      customColour1_B = 130;
      customColour2_R = 180;
      customColour2_G = 255;
      customColour2_B = 125;
      keyCommand = CMD_INDIGO;
      break;
    case IRCODE_ETOPXIZU_MAGENTA:
      customColour1_R = 255;
      customColour1_G = 0;
      customColour1_B = 255;
      customColour2_R = 0;
      customColour2_G = 255;
      customColour2_B = 0;
      keyCommand = CMD_MAGENTA;
      break;
    case IRCODE_ETOPXIZU_WHITE:
      customColour1_R = 255;
      customColour1_G = 255;
      customColour1_B = 255;
      customColour2_R = 0;
      customColour2_G = 0;
      customColour2_B = 0;
      keyCommand = CMD_WHITE;
      break;
    case IRCODE_ETOPXIZU_PINK:
      customColour1_R = 255;
      customColour1_G = 192;
      customColour1_B = 203;
      customColour2_R = 0;
      customColour2_G = 63;
      customColour2_B = 52;
      keyCommand = CMD_PINK;
      break;
    case IRCODE_ETOPXIZU_LIGHT_PINK:
      customColour1_R = 255;
      customColour1_G = 182;
      customColour1_B = 193;
      customColour2_R = 0;
      customColour2_G = 73;
      customColour2_B = 62;
      keyCommand = CMD_LIGHT_PINK;
      break;
    case IRCODE_ETOPXIZU_BABY_BLUE:
      customColour1_R = 137;
      customColour1_G = 207;
      customColour1_B = 240;
      customColour2_R = 118;
      customColour2_G = 48;
      customColour2_B = 15;
      keyCommand = CMD_BABY_BLUE;
      break;
    case IRCODE_ETOPXIZU_LIGHT_BLUE:
      customColour1_R = 173;
      customColour1_G = 216;
      customColour1_B = 230;
      customColour2_R = 82;
      customColour2_G = 39;
      customColour2_B = 25;
      keyCommand = CMD_LIGHT_BLUE;
      break;
    case IRCODE_ETOPXIZU_AUTO:
      keyCommand = CMD_AUTO;
      break;
    default:
      keyCommand = ' ';
      break;
    }

    switch (keyCommand) {
    case CMD_POWER:
      desireOff = !desireOff;

      if (desireOff) {
        beforeOffSelectedPattern = lastSelectedPattern;
        nextSelectedPattern = 0;
      }
      else {
        nextSelectedPattern = beforeOffSelectedPattern;
      }

      tCounter = -5;
      fadeFrames = shortFade;
      patternSelected = true;
      //stayFrames = shortFrame;
      //fadeFrames = longFade;
      break;
    case CMD_BRIGHT_DOWN:
      if (!desireOff) {
        brightness = brightness / 2;
        if (brightness < 1) {
          brightness = 256;
        }
      }
      break;
    case CMD_BRIGHT_UP:
      if (!desireOff) {
        brightness = brightness * 2;
        if (brightness > 256) {
          brightness = 1;
        }
      }
      break;
    case CMD_SPEED_DOWN:
      if (!desireOff) {
        speedDelay = speedDelay + 1;
        if (speedDelay > 8) {
          speedDelay = 0;
        }
      }
      break;
    case CMD_SPEED_UP:
      if (!desireOff) {
        speedDelay = speedDelay - 1;
        if (speedDelay < 0) {
          speedDelay = 8;
        }
      }
      break;
    case CMD_PATTERN_UP: //Up
      if (!desireOff) {
        nextSelectedPattern = lastSelectedPattern + 1;
        if (nextSelectedPattern >= (NUM_PATTERNS - 1)) { //skip PatternSolidColour - which is last prrogram
          nextSelectedPattern = 1; //Skip 0 which is 'Off' special program
        }
        tCounter = -5;
        fadeFrames = shortFade;
        patternSelected = true;
      }
      break;
    case CMD_CUSTOM1_UP: //Variation up
      if (!desireOff) {
        varChangeUp = true;
        tCounter = -5;
        fadeFrames = shortFade;
        patternSelected = true;
      }
      break;
    case CMD_CUSTOM1_DOWN: //Variation Down
      if (!desireOff) {
        varChangeUp = false;
        tCounter = -5;
        fadeFrames = shortFade;
        patternSelected = true;
      }
      break;
    case CMD_CUSTOM2_UP: //Timer - my parameter - num twinkles and other stuff
      if (!desireOff) {
        myParam1 = myParam1 + 1;
        if (myParam1 > 8) {
          myParam1 = 1;
        }
      }
      break;
    case CMD_CUSTOM2_DOWN: //Timer - my parameter
      if (!desireOff) {
        myParam1 = myParam1 - 1;
        if (myParam1 < 1) {
          myParam1 = 8;
        }
      }
      break;
    case CMD_CUSTOM3_UP: //my parameter - speed twinkles and other stuff
      if (!desireOff) {
        myParam2 = myParam2 + 1;
        if (myParam2 > 8) {
          myParam2 = 1;
        }
      }
      break;
    case CMD_CUSTOM3_DOWN: //Timer - my parameter
      if (!desireOff) {
        myParam2 = myParam2 - 1;
        if (myParam2 < 1) {
          myParam2 = 8;
        }
      }
      break;
    case CMD_PATTERN_DOWN: //Down
      if (!desireOff) {
        nextSelectedPattern = lastSelectedPattern - 1;
        if (nextSelectedPattern <= 0) { //Skip 0 which is 'Off' special program
          nextSelectedPattern = NUM_PATTERNS - 2; //skip PatternSolidColour - which is last prrogram
        }
        tCounter = -5;
        fadeFrames = shortFade;
        patternSelected = true;
      }
      break;
    case CMD_RED:
    case CMD_RED_ORANGE:
    case CMD_ORANGE:
    case CMD_YELLOW_ORANGE:
    case CMD_YELLOW:
    case CMD_GREEN:
    case CMD_LIME:
    case CMD_AQUA:
    case CMD_TEAL:
    case CMD_NAVY:
    case CMD_BLUE:
    case CMD_ROYAL_BLUE:
    case CMD_PURPLE:
    case CMD_INDIGO:
    case CMD_MAGENTA:
    case CMD_WHITE:
    case CMD_PINK:
    case CMD_LIGHT_PINK:
    case CMD_BABY_BLUE:
    case CMD_LIGHT_BLUE:
      useCustomColour = true;
      tCounter = -5;
      fadeFrames = shortFade;
      patternSelected = true;
      break;
    case CMD_AUTO:
      useCustomColour = false;
      tCounter = -5;
      fadeFrames = shortFade;
      patternSelected = true;
      break;
    }

    //Serial.print("Code: "); Serial.print(results.value); Serial.print("("); Serial.print(results.value, HEX); Serial.print(", ");Serial.print(keyCommand);Serial.print(") - ");Serial.print(nextSelectedPattern);
    //Serial.print("  Delay: "); Serial.print(speedDelay);Serial.print("  My Param: "); Serial.print(myParam1);Serial.print("  Brightness: "); Serial.println(brightness);
    irrecv.resume(); // Receive the next value
  }
}

volatile void LightFrame(void) {
  // Very first thing here is to issue the pixelString data generated from the
  // *previous* LightFrame.  It's done this way on purpose because show() is
  // roughly constant-time, so the refresh will always occur on a uniform
  // beat with respect to the Timer1 interrupt.  The various effects
  // rendering and compositing code is not constant-time, and that
  // unevenness would be apparent if show() were called at the end.
  FastLED.show();

  byte frontImgIdx = 1 - backImgIdx;
  byte r, g, b;
  int i;
  float alpha, invAlpha;

  // Render front image and alpha mask based on current effect
  // indices...
  // run a frame of the Pattern for the front
  delayCounterBack++;
  if (delayCounterBack >= fxActualSliders[backImgIdx][0]) {
    delayCounterBack = 0;
    runSelectedPattern(backImgIdx);
  }

  delayCounterFront++;
  if (delayCounterFront >= fxActualSliders[frontImgIdx][0]) {
    delayCounterFront = 0;
    runSelectedPattern(frontImgIdx);
  }

  runSelectedCrossfade();

  // Front render and composite only happen during transitions...
  if (tCounter > 0) {// Transition in progress
    for (i = 0; i < NUM_LEDS; i++) {
      //alpha = fadeMask[i];
      //invAlpha = 1.0 - alpha;
      int alpha256 = (int)(256.0 * invAlpha); //TODO: replace alpha/fademask with fract8/blend suitable logic)

      leds[i] = blend(fakeLEDsFB[frontImgIdx][i], fakeLEDsFB[backImgIdx][i], fadeMask[i]);

    }
  }
  else {
    // No transition in progress; just show back image
    for (i = 0; i < NUM_LEDS; i++) {
      // See note above re: r, g, b vars.
      //alpha = 0.0;
      //invAlpha = 1.0;

      leds[i] = blend(fakeLEDsFB[frontImgIdx][i], fakeLEDsFB[backImgIdx][i], 255);
    }
  }

  // Count up to next transition (or end of current one):
  tCounter++;

  if (tCounter == 0) { // Transition start
    SelectPattern(frontImgIdx);
    // skip any patterns not ticked in prefs
    skipPattern(frontImgIdx);
    // use next crossfade, and skip if not in list
    xFadeNo++;
    if (xFadeNo == NUM_CROSSFADES) {
      xFadeNo = 0;
    }
    skipCrossfade();

    transitionTime = fadeFrames;

    // Effect not yet initialized
    isInitialised[frontImgIdx] = false;
    // Transition not yet initialized //
    fxIntVars[2][0] = 0;

  }
  else if (tCounter >= transitionTime) { // End transition
    // Move front effect index to back
    patternNo[backImgIdx] = patternNo[frontImgIdx];
    backImgIdx = 1 - backImgIdx; // Invert back index

    int tempCounter = delayCounterBack; // Invert delay counters
    delayCounterBack = delayCounterFront;
    delayCounterFront = tempCounter;

    // normal scenaro: put counter back to setting time
    tCounter = -stayFrames;
  }
}

// ********** LIGHT PROGRAMS **************************

// Simulate being off, by turn all bulbs to black
void ProgramOff(byte idx) {
  // Only needs to be rendered once, when effect is initialized:
  if (isInitialised[idx] == false) {
    for (int i = 0; i < NUM_LEDS; i++) {
      fakeLEDsFB[idx][i] = CRGB(0, 0, 0);
    }
    isInitialised[idx] = true; // Effect initialized
  }
}

//parameter ideas - which hue
void ProgramSolidColour(byte idx) {
  // Only needs to be rendered once, when effect is initialized:
  if (isInitialised[idx] == false) {
    //TurnExcessPixelsOff(idx, NUM_LEDS);
    // choose random color
    int color[3];

    DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]

    if (useCustomColour) {
      color[0] = fxCustomColour[idx][0];
      color[1] = fxCustomColour[idx][1];
      color[2] = fxCustomColour[idx][2];
    }
    else {
      HSVtoRGB(random(1536), 255, 255, 0, color);
    }

    for (int i = 0; i < NUM_LEDS; i++) {
      fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
    }
    isInitialised[idx] = true; // Effect initialized
  }
}

//parameter ideas - density, speed
/*void Program00SoftWarmTwinkles(int idx) {
  if (isInitialised[idx] == false) {
    isInitialised[idx] = true; // Effect initialized
    for (int i = 0; i < NUM_LEDS; i++) {
      fakeLEDsFB[idx][i] = CRGB(0, 0, 0);

      fakeLeds[i].r = 0;
      fakeLeds[i].g = 0;
      fakeLeds[i].b = 0;
    }
    fxDefaultSliders[idx][0] = 0; //delay
    fxDefaultSliders[idx][1] = 512; //density (note these are sliders before divisor, so goes up to 1024)
    fxDefaultSliders[idx][2] = 0; //unused     
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    if (!fakeLeds[i]) { // skip black pixels
    }
    else {
      if (fakeLeds[i].r & 1) { // is red odd?
        fakeLeds[i] -= twinkleLightColour; // darken if red is odd
      }
      else {
        fakeLeds[i] += twinkleLightColour; // brighten if red is even
      }
    }
    fakeLEDsFB[idx][i] = fakeLeds[i];
  }

  int myInterpretedParam;
  myInterpretedParam = 1 << myParam1; // << is same as '2 to the power of'

  // Randomly choose a pixel, and if it's black, 'bump' it up a little.
  // Since it will now have an EVEN red component, it will start getting
  // brighter over time.
  if (random8() < (myInterpretedParam)) {
    int j = random16(NUM_LEDS);
    if (!fakeLeds[j]) {
      fakeLeds[j] = twinkleLightColour;
      fakeLEDsFB[idx][j] = fakeLeds[j];
    }
  }
}*/

//parameter ideas - density, speed
void Program00TwinkleFox2015(int idx) {
  if (isInitialised[idx] == false) {
    // Variations
    uint8_t numPalettes = sizeof(ActivePaletteList) / sizeof(ActivePaletteList[0]);    
    setVariation(0, numPalettes, idx);
        
    for (int i = 0; i < NUM_LEDS; i++) {
      fakeLEDsFB[idx][i] = CRGB(0, 0, 0);

      fakeLeds[i].r = 0;
      fakeLeds[i].g = 0;
      fakeLeds[i].b = 0;
    }
    fxDefaultSliders[idx][0] = 0; //delay
    fxDefaultSliders[idx][1] = 512; //density (note these are sliders before divisor, so goes up to 1024)
    fxDefaultSliders[idx][2] = 0; //unused

    DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]

    isInitialised[idx] = true; // Effect initialized    
  }

  // "PRNG16" is the pseudorandom number generator
  // It MUST be reset to the same starting value each time
  // this function is called, so that the sequence of 'random'
  // numbers that it generates is (paradoxically) stable.
  uint16_t PRNG16 = 11337;
  uint32_t clock32 = millis();

  // Set up the background color, "bg".
  // if AUTO_SELECT_BACKGROUND_COLOR == 1, and the first two colors of
  // the current palette are identical, then a deeply faded version of
  // that color is used for the background color
  CRGB bg;
  if ((AUTO_SELECT_BACKGROUND_COLOR == 1) &&
    (gCurrentPalette[0] == gCurrentPalette[1])) {
    bg = gCurrentPalette[0];
    uint8_t bglight = bg.getAverageLight();
    if (bglight > 64) {
      bg.nscale8_video(16); // very bright, so scale to 1/16th
    }
    else if (bglight > 16) {
      bg.nscale8_video(64); // not that bright, so scale to 1/4th
    }
    else {
      bg.nscale8_video(86); // dim, scale to 1/3rd.
    }
  }
  else {
    bg = gBackgroundColor; // just use the explicitly defined background color
  }

  uint8_t backgroundBrightness = bg.getAverageLight();

  for (int i = 0; i < NUM_LEDS; i++) {
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    uint16_t myclockoffset16 = PRNG16; // use that number as clock offset
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
    uint8_t myspeedmultiplierQ5_3 = ((((PRNG16 & 0xFF) >> 4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
    uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
    uint8_t  myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

    // We now have the adjusted 'clock' for this pixel, now we call
    // the function that computes what color the pixel should be based
    // on the "brightness = f( time )" idea.
    CRGB c = computeOneTwinkle(myclock30, myunique8, idx, fxCurrentVariationNo[idx]);
    CRGB pixel;

    uint8_t cbright = c.getAverageLight();
    int16_t deltabright = cbright - backgroundBrightness;
    if (deltabright >= 32 || (!bg)) {
      // If the new pixel is significantly brighter than the background color,
      // use the new color.
      pixel = c;
    }
    else if (deltabright > 0) {
      // If the new pixel is just slightly brighter than the background color,
      // mix a blend of the new color and the background color
      pixel = blend(bg, c, deltabright * 8);
    }
    else {
      // if the new pixel is not at all brighter than the background color,
      // just use the background color.
      pixel = bg;
    }

    fakeLEDsFB[idx][i] = pixel;
  }
}

// Simplex Noise (inspired by happyinmotion: http://happyinmotion.livejournal.com/278357.html)
// Simplex noise for whole strip of LEDs. (Well, it's simplex noise for 6 nodes and cubic interpolation between those nodes.)
//parameter ideas - yoffset/space inc/time inc
void Program01SimplexNoise(int idx) {
  if (isInitialised[idx] == false) {
    // Variations
    setVariation(1, 8, idx);
    // Log.i(TAG_SETTINGS, "Simplex Noise Var: " +
    // fxCurrentVariationNo[idx]);
    String varName = "";

    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 8; //an extra on the end
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    if (fxCurrentVariationNo[idx] == 7) {
      // normal or complimentary version
      fxIntVars[idx][2] = 0;
    }
    else {
      fxIntVars[idx][2] = 1;
    }

    fxFltVars[idx][0] = 0.0; // yOffset
    // Useable values for time increment range from 0.005 (barely
    // perceptible) to 0.2 (irritatingly flickery)
    // 0.02 seems ideal for relaxed screensaver
    fxFltVars[idx][1] = 0.02; // time inc
    // Useable values for space increment range from 0.8 (LEDS doing
    // different things to their neighbours), to 0.02 (roughly one
    // feature present in 15 LEDs).
    fxFltVars[idx][2] = 0.1; // space inc

    fxFltVars[idx][3] = -1000.0; // debug - max value
    fxFltVars[idx][4] = 1000.0; // debug - min value

    // need to remember locally for bulb count changes, otherwise
    // crashes
    // fxIntVars[idx][7] = NUM_LEDS;

    // complementary main color
    fxIntVars[idx][3] = random(1536);
    // complementary increment colour steps at a time
    fxIntVars[idx][4] = 1;
    // complementary increment total
    fxIntVars[idx][5] = 0;

    isInitialised[idx] = true; // Effect initialized

    fxDefaultSliders[idx][0] = 2; //delay
    fxDefaultSliders[idx][1] = 512; //timeinc (note these are sliders BEFORE divisor, so does go up to 1024)
    fxDefaultSliders[idx][2] = 512; //spaceinc
  }

  //*POTENTIOMETERS*
  //fxFltVars[idx][1] = 0.01 + ((float)slider3 / 51200.0);
  //fxFltVars[idx][2] = 0.05 + ((float)slider4 / 10240.0);

  float bulbArray_red[NUM_LEDS + 1];
  float bulbArray_green[NUM_LEDS + 1];
  float bulbArray_blue[NUM_LEDS + 1];

  int numSpacing = NUM_LEDS / SQRT_LEDS; // / 1 = every pixel - max overhead
  int node_spacing = NUM_LEDS / numSpacing;

  float rMult_ = 0;
  float gMult_ = 0;
  float bMult_ = 0;
  float spaceinc = fxFltVars[idx][2]; //*POTENTIOMETER THIS*
  float timeinc = fxFltVars[idx][1]; //*POTENTIOMETER THIS*
  float yoffset = fxFltVars[idx][0]; //*POTENTIOMETER THIS*

  // sub pattern variation
  switch (fxCurrentVariationNo[idx]) {
  case 0: // RGB
    rMult_ = 1.0;
    gMult_ = 1.0;
    bMult_ = 1.0;
    break;
  case 1: // RB
    rMult_ = 1.0;
    gMult_ = 0.0;
    bMult_ = 1.0;
    break;
  case 2: // GB
    rMult_ = 0.0;
    gMult_ = 1.0;
    bMult_ = 1.0;
    break;
  case 3: // RG
    rMult_ = 1.0;
    gMult_ = 1.0;
    bMult_ = 0.0;
    break;
  case 4: // R
    rMult_ = 1.0;
    gMult_ = 0.0;
    bMult_ = 0.0;
    break;
  case 5: // G
    rMult_ = 0.0;
    gMult_ = 1.0;
    bMult_ = 0.0;
    break;
  case 6: // B
    rMult_ = 0.0;
    gMult_ = 0.0;
    bMult_ = 1.0;
    break;
  case 7: // rotating complimentary
    break;
  case 8: // custom color
    break;
  }

  float xoffset = 0.0;
  for (int i = 0; i <= NUM_LEDS; i = i + node_spacing) {
    xoffset += spaceinc;
    bulbArray_red[i] = SNSimplexNoise(xoffset, yoffset, 0.0);
    bulbArray_green[i] = SNSimplexNoise(xoffset, yoffset, 1.0);
    bulbArray_blue[i] = SNSimplexNoise(xoffset, yoffset, 2.0);
  }

  int last_node = 0;
  int next_node = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    int position_between_nodes = i % node_spacing;

    // If at node, skip
    if (position_between_nodes == 0) {
      // At node for simplex noise, do nothing but update which nodes we are between
      last_node = i;
      next_node = last_node + node_spacing;
    }
    else {
      // Else between two nodes, so identify those nodes
      // And interpolate between the values at those nodes for red,
      // green, and blue
      float t = (float)position_between_nodes / (float)node_spacing;
      float t_squaredx3 = 3.0 * t * t;
      float t_cubedx2 = 2.0 * t * t * t;
      bulbArray_red[i] = (bulbArray_red[last_node] * (t_cubedx2 - t_squaredx3 + 1.0)) + (bulbArray_red[next_node] * (t_squaredx3 - t_cubedx2));
      bulbArray_green[i] = (bulbArray_green[last_node] * (t_cubedx2 - t_squaredx3 + 1.0)) + (bulbArray_green[next_node] * (t_squaredx3 - t_cubedx2));
      bulbArray_blue[i] = (bulbArray_blue[last_node] * (t_cubedx2 - t_squaredx3 + 1.0)) + (bulbArray_blue[next_node] * (t_squaredx3 - t_cubedx2));
    }
  }

  // Convert values from raw noise to scaled r,g,b and feed to strip
  for (int i = 0; i < NUM_LEDS; i++) {
    if (bulbArray_red[i] > fxFltVars[idx][3]) {
      fxFltVars[idx][3] = bulbArray_red[i]; // debug - max value
    }
    if (bulbArray_red[i] < fxFltVars[idx][4]) {
      fxFltVars[idx][4] = bulbArray_red[i]; // debug - min value
    }

    // values come in between -0.2910708 and 0.29872513 so the multiplier to get to a max of 255 is 853.62754717
    int r, g, b;

    if (fxCurrentVariationNo[idx] == 7) {
      // two color complementary version
      int saturation = 0, level = 0, col = 0;
      if (bulbArray_red[i] > 0.0) {
        col = (fxIntVars[idx][3] + fxIntVars[idx][5]) % 1536;
        saturation = 255;
        level = (int)(bulbArray_red[i] * 853.62754717);
      }
      else {
        col = (fxIntVars[idx][3] + 768 + fxIntVars[idx][5]) % 1536;
        saturation = 255;
        level = (int)(bulbArray_red[i] * -876.07551552); // multiplier for 255max
      }
      int color[3];
      HSVtoRGB(col, saturation, level, 0, color);//saturation, level, 0, color);
      fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);

    }
    else if (fxCurrentVariationNo[idx] == 8) {
      // custom color version
      int saturation = 0, level = 0, col = 0;
      if (bulbArray_red[i] > 0.0) {
        col = fxCustomColour[idx][3];
        saturation = 255;
        level = (int)(bulbArray_red[i] * 853.62754717);
      }
      int color[3];
      HSVtoRGB(col, saturation, level, 0, color);
      fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
    }
    else {
      // normal version
      r = (int)(bulbArray_red[i] * 853.62754717);
      g = (int)(bulbArray_green[i] * 853.62754717);
      b = (int)(bulbArray_blue[i] * 853.62754717);
      if (r > 255) {
        r = 255;
      }
      else if (r < 0) {
        r = 0; // Adds no time at all. Conclusion: constrain()
        // sucks.
      }

      if (g > 255) {
        g = 255;
      }
      else if (g < 0) {
        g = 0;
      }

      if (b > 255) {
        b = 255;
      }
      else if (b < 0) {
        b = 0;
      }

      if (rMult_ > 1.0) {
        rMult_ = 1.0;
      }
      else if (rMult_ < 0.0) {
        rMult_ = 0.0;
      }

      if (gMult_ > 1.0) {
        gMult_ = 1.0;
      }
      else if (gMult_ < 0.0) {
        gMult_ = 0.0;
      }

      if (bMult_ > 1.0) {
        bMult_ = 1.0;
      }
      else if (bMult_ < 0.0) {
        bMult_ = 0.0;
      }
      fakeLEDsFB[idx][i] = CRGB((int)(r * rMult_), (int)(g * gMult_), (int)(b * bMult_));
    }
  }


  // change over time, and copy back to FltVar
  yoffset += timeinc;

  fxFltVars[idx][0] = yoffset;

  // increment colour steps counter, and trim back to 1535 max
  fxIntVars[idx][5] += fxIntVars[idx][4];
  fxIntVars[idx][5] = fxIntVars[idx][5] % 1536;
}

// based on complementary fade, but just one colour
//parameter ideas - fade speed
void Program02LineFade(int idx) {
  if (isInitialised[idx] == false) {
    fxIntVars[idx][0] = random(1536); // Random hue //*POTENTIOMETER THIS*
    fxIntVars[idx][2] = 0; // frame count

    // Variations
    setVariation(2, 4, idx);

    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 4; //an extra on the end
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    isInitialised[idx] = true; // Effect initialized
    // Log.i(TAG, "Initialised Program: Line Fade");
  }

  //*POTENTIOMETERS*
  //fxIntVars[idx][0] = (analogRead(A2) * 1536) / 4096;
  //fxFltVars[idx][2] = (float)analogRead(A0) / 20000.0;

  int color[3];
  int fade = 0, hue = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    fade = GetSmoothFade27((i + fxIntVars[idx][2]) % SQRT_LEDS);

    switch (fxCurrentVariationNo[idx]) {
    case 0: // Fixed Colour, RGB
      hue = fxIntVars[idx][0]; //*POTENTIOMETER THIS*
      HSVtoRGB(hue, 255, fade, 0, color);
      break;
    case 1: // RG
      hue = (fxIntVars[idx][0] + (fxIntVars[idx][2] * 8)) % 1024;
      HSVtoRGB(hue, 255, fade, 1, color);
      break;
    case 2: // GB
      hue = (fxIntVars[idx][0] + (fxIntVars[idx][2] * 8)) % 1024;
      HSVtoRGB(hue, 255, fade, 2, color);
      break;
    case 3: // BR
      hue = (fxIntVars[idx][0] + (fxIntVars[idx][2] * 8)) % 1024;
      HSVtoRGB(hue, 255, fade, 3, color);
      break;
    case 4: // custom
      hue = fxCustomColour[idx][3];
      HSVtoRGB(hue, 255, fade, 3, color);
      break;
    }

    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }
  fxIntVars[idx][2]++;
  if (fxIntVars[idx][2] >= 1536) {
    fxIntVars[idx][2] = 0;
  }
}

// Chaser
// based on G35 Xmas lights code by Paul Martis
// (http://www.digitalmisery.com)
// (more interesting patterns by MEO)
//parameter ideas - LEDs in a row
void Program03Chaser(int idx) {
  if (isInitialised[idx] == false) {

    fxIntVars[idx][0] = 0; // sequence step
    // specific color
    fxIntVars[idx][3] = random(1536); // color starting point
    fxIntVars[idx][4] = 1; // count step

    for (int i = 0; i < NUM_LEDS; i++) {
      fakeLEDsFB[idx][i] = CRGB(0, 0, 0);
    }

    // Variations
    setVariation(3, 6, idx);

    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 6; //an extra on the end
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    fxFrameDelay[idx] = 2; //delay frame count
    fxFrameDelayCount[idx] = 0; //delay frame

    isInitialised[idx] = true; // Effect initialized

    //Serial_printf("Variation: %d", fxCurrentVariationNo[idx]);
  }

  if (fxFrameDelayCount[idx] == fxFrameDelay[idx]) { //only do once every delay frames
    int count, sequence;
    count = fxIntVars[idx][4];
    sequence = fxIntVars[idx][0];

    int bulbsInARow;
    bulbsInARow = myParam1; //((slider3 * 10) / 1024) + 1;

    while (count-- > 0) {
      int color[3];

      // The x in 'sequence++ / x' decides how many of the bulbs in a //*POTENTIOMETER THIS*
      // row are the same (stops looking too flickery when
      // appropriate)
      switch (fxCurrentVariationNo[idx]) {
      case 0:
        ChaseSeqRotateAnalogic45(sequence++ / bulbsInARow, fxIntVars[idx][3], color);
        break;
      case 1:
        ChaseSeqRotateAccentedAnalogic30(sequence++ / bulbsInARow, fxIntVars[idx][3], color);
        break;
      case 2:
        ChaseSeqRotateCompliment(sequence++ / bulbsInARow, fxIntVars[idx][3], color);
        break;
      case 3:
        ChaseSeqRGB(sequence++ / bulbsInARow, fxIntVars[idx][3], color);
        break;
      case 4:
        ChaseSeqRWB(sequence++ / bulbsInARow, fxIntVars[idx][3], color);
        break;
      case 5:
        ChaseSeqGWR(sequence++ / bulbsInARow, fxIntVars[idx][3], color);
        break;
      case 6:
        ChaseSeqCustom(sequence++ / bulbsInARow, fxCustomColour[idx][3], color);
        break;
      }

      for (int i = 0; i < NUM_LEDS; i++) {
        if (i == count) {
          fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
        }
      }
    }

    if (fxIntVars[idx][4] < NUM_LEDS) {
      ++fxIntVars[idx][4];
    }
    else {
      ++fxIntVars[idx][0];
    }
    fxFrameDelayCount[idx] = 0;
  }
  else {
    fxFrameDelayCount[idx]++;
  }
}

// Rainbow effect (1 or more full loops of color wheel at 100% saturation).
// Not a big fan of this pattern (it's way overused with LED stuff), but
// it's
// practically part of the Geneva Convention by now.
//parameter ideas - density / direction
void Program04Rainbow(int idx) {
  if (isInitialised[idx] == false) { // Initialize effect?
    // Number of repetitions (complete loops around color wheel); any
    // more than 4 per meter just looks too chaotic and un-rainbow-like.
    // Store as hue 'distance' around complete belt:
    // 0 = one colour at a time, 1536 (or 1024) has all colours spread
    // across string once
    // 6144 (4096) = 4 / 3072 (2048) = 2
    // fxIntVars[idx][0] = 0;

    // Frame-to-frame hue increment (speed) -- may be positive or
    // negative,
    // but magnitude shouldn't be so small as to be boring. It's
    // generally
    // still less than a full pixel per frame, making motion very
    // smooth.
    fxIntVars[idx][1] = 16;

    // Reverse speed and hue shift direction half the time.
    if (random(2) == 0)
      fxIntVars[idx][0] = -fxIntVars[idx][0];
    if (random(2) == 0)
      fxIntVars[idx][1] = -fxIntVars[idx][1];
    fxIntVars[idx][2] = 0; // Current position
    fxIntVars[idx][3] = 1; // increase step 1 / decrease step 0

    // fxIntVars[idx][4] = random(4); // full rainbow / RG only
    // / GB / BR

    // Variations
    setVariation(4, 8, idx);

    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 8; //an extra on the end
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    // fxInt1 = span of colour - increases, so 0 will become higher etc
    // - 0 is single color across string
    // - 1536 is all colours across string (or 1024 for RG/GB/BR)
    // - half and double also used
    //String varName = "";
    switch (fxCurrentVariationNo[idx]) {
    case 0: // RGB single
      fxIntVars[idx][0] = 0;
      fxIntVars[idx][4] = 0;
      //varName = "Red to green to blue Single";
      break;
    case 1: // RG single
      fxIntVars[idx][0] = 0;
      fxIntVars[idx][4] = 1;
      //varName = "R to G Single";
      break;
    case 2: // GB single
      fxIntVars[idx][0] = 0;
      fxIntVars[idx][4] = 2;
      //varName = "G to B Single";
      break;
    case 3: // BR single
      fxIntVars[idx][0] = 0;
      fxIntVars[idx][4] = 3;
      //varName = "B to R Single";
      break;
    case 4: // RGB span
      fxIntVars[idx][0] = 1536;
      fxIntVars[idx][4] = 0;
      //varName = "R to G to B Span";
      break;
    case 5: // RG span
      fxIntVars[idx][0] = 1024;
      fxIntVars[idx][4] = 1;
      //varName = "R to G Span";
      break;
    case 6: // GB span
      fxIntVars[idx][0] = 1024;
      fxIntVars[idx][4] = 2;
      //varName = "G to B Span";
      break;
    case 7: // BR span
      fxIntVars[idx][0] = 1024;
      fxIntVars[idx][4] = 3;
      //varName = "B to R Span";
      break;
    case 8: // custom color (spread a several degrees either side of hue)
      fxIntVars[idx][0] = fxCustomColour[idx][3];
      fxIntVars[idx][4] = 0;
      //varName = "Custom Color";
      break;
    }
    isInitialised[idx] = true; // Effect initialized
  }

  int color[3];
  int i;
  for (i = 0; i < NUM_LEDS; i++) {

    if (fxCurrentVariationNo[idx] == 8) {
      HSVtoRGB((((fxIntVars[idx][2] + fxIntVars[idx][0] * i / NUM_LEDS) - 120) % 240 + fxCustomColour[idx][3]) % 1536, 255, 255, fxIntVars[idx][4], color);
    }
    else {
      HSVtoRGB((fxIntVars[idx][2] + fxIntVars[idx][0] * i / NUM_LEDS), 255, 255, fxIntVars[idx][4], color);
    }

    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }
  fxIntVars[idx][2] += fxIntVars[idx][1];

  // step in direction
  if (fxIntVars[idx][3] == 1) {
    fxIntVars[idx][0]++;
  }
  else {
    fxIntVars[idx][0]--;
  }
  // change direction
  if (fxIntVars[idx][0] == 2500) {
    fxIntVars[idx][3] = 0;
  }
  if (fxIntVars[idx][0] == -1) {
    fxIntVars[idx][3] = 1;
  }
}

// Sine wave chase effect
//parameter ideas - density / direction
void Program05Wave(int idx) {
  if (isInitialised[idx] == false) { // Initialize effect?
    fxIntVars[idx][0] = random(3) * 512; // random(1536); //
    // Random hue
    // Number of repetitions (complete loops around color wheel);
    // any more than 4 per meter just looks too chaotic.
    // Store as distance around complete belt in half-degree units:
    // fxIntVars[idx][1] = (1 + random(4 * ((_NUM_LEDS + 31) /
    // 32))) * 720;
    // Frame-to-frame increment (speed) -- may be positive or negative,
    // but magnitude shouldn't be so small as to be boring. It's
    // generally
    // still less than a full pixel per frame, making motion very
    // smooth.

    // Log.i(TAG, "Rand 0: " + fxIntVars[idx][0]); // ##DEBUG##

    fxIntVars[idx][3] = 0; // Current position
    // ToDo: Rainbow changing sine
    // ToDo: Rainbown spread across string sine
    // fxIntVars[idx][4] = random(2) + 1; // number of half
    // waves per string
    // fxIntVars[idx][5] = random(2); // still colour or rainbow
    // fxIntVars[idx][6] = random(4); // full rainbow or lines
    fxIntVars[idx][7] = 0; // start of rainbow

    // Variations
    setVariation(5, 3, idx);

    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 3; //an extra on the end
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    // Log.i(TAG_SETTINGS, "Wave Var: " + fxCurrentVariationNo[idx]);
    //String varName = "";
    switch (fxCurrentVariationNo[idx]) {
    case 0: // candy stripe
      fxFltVars[idx][4] = 4.0 * (float)NUM_LEDS / 255.0; //*POTENTIOMETER THIS*
      fxIntVars[idx][5] = 0;
      fxIntVars[idx][6] = 0;
      //varName = "Candy stripe";
      break;
    case 1: // gradually changing through rainbow
      fxFltVars[idx][4] = 1.0 * (float)NUM_LEDS / 255.0;
      fxIntVars[idx][5] = 1;
      fxIntVars[idx][6] = random(4); // RGB/RG/GB/BR
      //varName = "Color shift";
      break;
    case 2: // rainbow across string
      fxFltVars[idx][4] = 2.0 * (float)NUM_LEDS / 255.0;
      fxIntVars[idx][5] = 1;
      fxIntVars[idx][6] = 0;
      //varName = "Rainbow";
      break;
    case 3: // custom color
      fxFltVars[idx][4] = 4.0 * (float)NUM_LEDS / 255.0;
      fxIntVars[idx][5] = 0;
      fxIntVars[idx][6] = 0;
      //varName = "Custom Color";
      break;
    }

    fxIntVars[idx][2] = 4 + random(fxIntVars[idx][0] + 1) / NUM_LEDS;
    // Reverse direction half the time.
    if (random(2) == 0)
      fxIntVars[idx][2] = -fxIntVars[idx][2];
    //currentMessage = "Wave\n" + varName + "\n";
    isInitialised[idx] = true; // Effect initialized
  }

  int color[3];
  int i;
  float y;

  for (i = 0; i < NUM_LEDS; i++) {
    // Peaks of sine wave are white, troughs are black, mid-range
    // values are pure hue (100% saturated).

    y = (float)(sin(PI * fxFltVars[idx][4] * 0.5 * (fxIntVars[idx][3] + (float)i) / NUM_LEDS) * 255.0);

    int hue = 0;
    switch (fxCurrentVariationNo[idx]) {
    case 0: // candy stripe
      hue = 0; // red
      break;
    case 1: // gradually changing through rainbow
      hue = fxIntVars[idx][0] + (fxIntVars[idx][7] * fxIntVars[idx][5]);
      break;
    case 2: // rainbow across string
      hue = (i * 1536 / NUM_LEDS) + fxIntVars[idx][7];
      break;
    case 3: // custom color
      hue = fxCustomColour[idx][3];
      break;
    }

    if (y >= 0.0) {
      // Peaks of sine wave are white (saturation = 0)
      HSVtoRGB(hue, 255 - (int)y, 255, fxIntVars[idx][6], color);
    }
    else {
      // troughs are black (level = 0)
      HSVtoRGB(hue, 255, 255 + (int)y, fxIntVars[idx][6], color);
    }

    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }
  fxIntVars[idx][3] += fxIntVars[idx][2];
  fxIntVars[idx][7]++;
}

void Program06Phasing(int idx) { //THIS IS BROKEN FOR SOME UNKNOWN REASON...
  if (isInitialised[idx] == false) {
    fxIntVars[idx][0] = 0; // start step (fStep in 2012 version) - freq modifier
    fxIntVars[idx][2] = 0; // pStep in 2012 version - phase modifier
    fxIntVars[idx][3] = 1; // direction (1 forward / 0 backwards)
    fxIntVars[idx][4] = 0; // turn - sub-sub-variation

    fxIntVars[idx][5] = 250; // was switchHalf (switchFull was double this)

    // rate of change, or how much moves per frame (larger takes longer)
    fxFltVars[idx][6] = 1000.0;

    // private final float rate = 1000.0f; // 2000.0 //size or rate of change, or
    // how much moves at a time, or summat (larger takes longer)
    // private final int switchHalf = 250; // 2000 //should be related to rate
    // (e.g. rate=100.0, this 100) private final int switchFull = 500; // 4000

    fxBlnVars[idx][0] = false; // these may get over rided in
    // following switch.
    fxBlnVars[idx][1] = false;
    fxBlnVars[idx][2] = false;

    // Variations
    setVariation(6, 17, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 2; //same as 'White', will do custom color check elsewhere
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    switch (fxCurrentVariationNo[idx]) {
    case 0: // Wavey pastels (Green 'peak')
      fxFltVars[idx][3] = 0.0;
      fxFltVars[idx][4] = 2.0 * PI / 3.0;
      fxFltVars[idx][5] = 4.0 * PI / 3.0;
      //varName = "Rainbow";
      break;
    case 1: // subtly changing pastel
      fxFltVars[idx][3] = 0.0;
      fxFltVars[idx][0] = 1.666;
      fxFltVars[idx][1] = 2.666;
      fxFltVars[idx][2] = 3.666;
      //varName = "Subtle pastels";
      break;
    case 2: // White
      fxFltVars[idx][3] = 1.0;
      fxFltVars[idx][4] = 1.0;
      fxFltVars[idx][5] = 1.0;
      //varName = "Black and white";
      break;
    case 3: // Cyan/Red/White (Cyan 'peak')
      fxFltVars[idx][3] = 0.0;
      fxFltVars[idx][4] = 1.0;
      fxFltVars[idx][5] = 1.0;
      //varName = "Cyan, red and white";
      break;
    case 4: // Magenta/Green/White (Magenta 'peak')
      fxFltVars[idx][3] = 1.0;
      fxFltVars[idx][4] = 0.0;
      fxFltVars[idx][5] = 1.0;
      //varName = "Magenta, green and white";
      break;
    case 5: // Yellow/Blue/White (Yellow 'peak')
      fxFltVars[idx][3] = 1.0;
      fxFltVars[idx][4] = 1.0;
      fxFltVars[idx][5] = 0.0;
      //varName = "Yellow, blue and white";
      break;
    case 6: // nothing - a still pastel rainbow
      fxFltVars[idx][3] = 2.0 * PI / 3.0;
      fxFltVars[idx][4] = 4.0 * PI / 3.0;
      fxFltVars[idx][5] = 0.0;
      fxFltVars[idx][0] = 0.06;
      fxFltVars[idx][1] = 0.06;
      fxFltVars[idx][2] = 0.06;
      //varName = "Still";
      break;
    case 7: // Evolving pastel wave - 6 sub variations
      //varName = "Full sweep";
      break;
    case 8: // Red/Blue/Magenta (Red 'peak')
      fxFltVars[idx][3] = 0.0;
      fxFltVars[idx][4] = 2.0 * PI / 3.0;
      fxFltVars[idx][5] = 4.0 * PI / 3.0;
      fxBlnVars[idx][1] = true; // switch off green channel
      //varName = "Blue and red - dark";
      break;
    case 9: // Green/Red/Yellow (Green 'peak')
      fxFltVars[idx][3] = 0.0;
      fxFltVars[idx][4] = 2.0 * PI / 3.0;
      fxFltVars[idx][5] = 4.0 * PI / 3.0;
      fxBlnVars[idx][2] = true; // switch off blue channel
      //varName = "Red and green - dark";
      break;
    case 10: // Blue/Green/Cyan (Blue 'peak')
      fxFltVars[idx][3] = 0.0;
      fxFltVars[idx][4] = 4.0 * PI / 3.0;
      fxFltVars[idx][5] = 2.0 * PI / 3.0;
      fxBlnVars[idx][0] = true; // switch off red channel
      //varName = "Green and blue - dark";
      break;
    case 11: // Cyan/Green/Red/Magenta slightly askew (Cyan 'peak')
      fxFltVars[idx][3] = 0.0;
      fxFltVars[idx][4] = 2.0 * PI / 3.0;
      fxFltVars[idx][5] = 1.0;
      //varName = "Red and green - bright";
      break;
    case 12: // Magenta/Green/Blue/Cyan slightly askew (Magenta 'peak')
      fxFltVars[idx][3] = 1.0;
      fxFltVars[idx][4] = 0.0;
      fxFltVars[idx][5] = 2.0 * PI / 3.0;
      //varName = "Green and blue - bright";
      break;
    case 13: // Yellow/Blue/Red/Cyan slightly askew (Yellow 'peak')
      fxFltVars[idx][3] = 2.0 * PI / 3.0;
      fxFltVars[idx][4] = 1.0;
      fxFltVars[idx][5] = 0.0;
      //varName = "Blue and red - bright";
      break;
    case 14:
      fxFltVars[idx][3] = 1.0;
      fxFltVars[idx][4] = 1.0;
      fxFltVars[idx][5] = 0.0;
      fxFltVars[idx][2] = 0.0;
      //varName = "Pastel green and yellow";
      break;
    case 15:
      fxFltVars[idx][3] = 0.0;
      fxFltVars[idx][4] = 1.0;
      fxFltVars[idx][5] = 1.0;
      fxFltVars[idx][0] = 0.0;
      //varName = "pastel purple and indigo";
      break;
    case 16:
      fxFltVars[idx][3] = 1.0;
      fxFltVars[idx][4] = 0.0;
      fxFltVars[idx][5] = 1.0;
      fxFltVars[idx][1] = 0.0;
      //varName = "Pastel blue and pink";
      break;
    case 17: // White
      fxFltVars[idx][3] = 1.0;
      fxFltVars[idx][4] = 1.0;
      fxFltVars[idx][5] = 1.0;
      //varName = "Black and white";
      break;
    }

    //currentMessage = "Phasing\n" + varName + "\n";
    isInitialised[idx] = true; // Effect initialized
    // Log.i(TAG, "Initialised Program: Phasing");
    //Serial.println("SWAP");
  }

  // this switch sets the *variable* variables! (again, not necessarily
  // the same ones)
  switch (fxCurrentVariationNo[idx]) {
  case 0: // Wavey pastels (Green 'peak')
  case 2: // White
  case 3: // Cyan/Red/White (Cyan 'peak')
  case 4: // Magenta/Green/White (Magenta 'peak')
  case 5: // Yellow/Blue/White (Yellow 'peak')
  case 8: // Red/Blue/Magenta (Red 'peak')
  case 9: // Green/Red/Yellow (Green 'peak')
  case 10: // Blue/Green/Cyan (Blue 'peak')
  case 11: // Cyan/Green/Red/Magenta slightly askew (Cyan 'peak')
  case 12: // Magenta/Green/Blue/Cyan slightly askew (Magenta 'peak')
  case 13: // Yellow/Blue/Red/Cyan slightly askew (Yellow 'peak')
    fxFltVars[idx][0] = (float)fxIntVars[idx][0] / fxFltVars[idx][6];
    fxFltVars[idx][1] = (float)fxIntVars[idx][0] / fxFltVars[idx][6];
    fxFltVars[idx][2] = (float)fxIntVars[idx][0] / fxFltVars[idx][6];

    break;
  case 1: // subtly changing pastel
    fxFltVars[idx][4] = (2.0 * PI / 360.0) * (float)fxIntVars[idx][0];
    fxFltVars[idx][5] = (4.0 * PI / 360.0) * (float)fxIntVars[idx][0];
    break;
  case 6: // Single primary (White 'peak') - 'White' is a touch
    // complementary color
    break;
  case 7: // Evolving pastel wave - 6 sub variations
    fxFltVars[idx][0] = (float)fxIntVars[idx][0] / fxFltVars[idx][6];
    fxFltVars[idx][1] = (float)fxIntVars[idx][0] / fxFltVars[idx][6];
    fxFltVars[idx][2] = (float)fxIntVars[idx][0] / fxFltVars[idx][6];
    float temp;
    temp = (PI / 360.0) * (float)fxIntVars[idx][0];
    switch (fxIntVars[idx][4] % 6) {
    case 0: // these sub variations are all pretty similar
      fxFltVars[idx][3] = 0.0 * temp;
      fxFltVars[idx][4] = 2.0 * temp;
      fxFltVars[idx][5] = 4.0 * temp;
      break;
    case 2:
      fxFltVars[idx][3] = 4.0 * temp;
      fxFltVars[idx][4] = 0.0 * temp;
      fxFltVars[idx][5] = 2.0 * temp;
      break;
    case 4:
      fxFltVars[idx][3] = 2.0 * temp;
      fxFltVars[idx][4] = 4.0 * temp;
      fxFltVars[idx][5] = 0.0 * temp;
      break;
    case 1:
      fxFltVars[idx][3] = 0.0 * temp;
      fxFltVars[idx][4] = 4.0 * temp;
      fxFltVars[idx][5] = 2.0 * temp;
      break;
    case 3:
      fxFltVars[idx][3] = 4.0 * temp;
      fxFltVars[idx][4] = 2.0 * temp;
      fxFltVars[idx][5] = 0.0 * temp;
      break;
    case 5:
      fxFltVars[idx][3] = 2.0 * temp;
      fxFltVars[idx][4] = 0.0 * temp;
      fxFltVars[idx][5] = 4.0 * temp;
      break;
    }
    break;
  case 14: // Cyan & White (White 'peak')
    fxFltVars[idx][0] = (float)fxIntVars[idx][0] / fxFltVars[idx][6];
    fxFltVars[idx][1] = 0; // in case 14: 0 makes green/yellow
    break;
  case 15: // Cyan/Magenta (Cyan 'peak')
    fxFltVars[idx][1] = (float)fxIntVars[idx][0] / fxFltVars[idx][6];
    fxFltVars[idx][2] = 0; // in case 17: 0 would make white/weak
    // mageneta
    break;
  case 16: // Cyan/White (White 'peak') variation
    fxFltVars[idx][0] = (float)fxIntVars[idx][0] / fxFltVars[idx][6];
    fxFltVars[idx][2] = 0;
    break;
  case 17: //custom color
    break;
  }

  int color[3];

  for (int i = 0; i < NUM_LEDS; i++) {
    if (fxBlnVars[idx][0]) {
      color[0] = 0;
    }
    else {
      color[0] = (int)(sin(fxFltVars[idx][0] * i + fxFltVars[idx][3]) * (((float)MAX_LEVEL / 2.0) - 1.0) + ((float)MAX_LEVEL / 2.0));
    }
    if (fxBlnVars[idx][1]) {
      color[1] = 0;
    }
    else {
      color[1] = (int)(sin(fxFltVars[idx][1] * i + fxFltVars[idx][4]) * (((float)MAX_LEVEL / 2.0) - 1.0) + ((float)MAX_LEVEL / 2.0));
    }
    if (fxBlnVars[idx][2]) {
      color[2] = 0;
    }
    else {
      color[2] = (int)(sin(fxFltVars[idx][2] * i + fxFltVars[idx][5]) * (((float)MAX_LEVEL / 2.0) - 1.0) + ((float)MAX_LEVEL / 2.0));
    }

    if (useCustomColour) {
      int HSVColor[3];
      RGBtoHSV(color[0], color[1], color[2], HSVColor);
      //just use the level applied to custom color
      HSVtoRGB(fxCustomColour[idx][3], 255, HSVColor[2], 0, color);
    }

    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }

  // step up in general
  fxIntVars[idx][2]++;

  // step in direction
  if (fxIntVars[idx][3] == 1) {
    fxIntVars[idx][0]++;
  }
  else {
    fxIntVars[idx][0]--;
  }

  // set reverse direction: 0 1 2 .. 98 99 .. 399 400 399 .. 99 98 .. 2 1
  if (fxIntVars[idx][2] == fxIntVars[idx][5]) { // switchHalf
    fxIntVars[idx][3] = 0;
  }
  // repeat when finished 0 1 2 .. 799 800 0 1 2 .. 799 800
  if (fxIntVars[idx][2] == fxIntVars[idx][5] * 2) { // switchfull
    fxIntVars[idx][2] = 0;
  }

  // set forward direction: 0 1 2 .. 98 99 .. 399 400 399 .. 99 98 .. 2 1
  // and increment sub-sub-variation counter
  if (fxIntVars[idx][0] == 0) {
    fxIntVars[idx][3] = 1;
    fxIntVars[idx][4]++;
  }
}

// tried and tested plasma algorithm
// this is too boring and also to CPU intensive so slows down cross fades
void Program07Plasma(int idx) {
  if (isInitialised[idx] == false) {
    fxIntVars[idx][3] = random(2) + 3; // speed
    fxIntVars[idx][3] = fxIntVars[idx][3] * fxIntVars[idx][3]; // square
    // speed
    fxIntVars[idx][4] = 0; // frame

    fxFltVars[idx][0] = 0.0; // time

    setVariation(7, 10, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 0; //same as 'Black and White', will check elsewhere
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    isInitialised[idx] = true; // Effect initialized
    // Log.i(TAG, "Initialised Program: Plasma");
  }
  int color[3];
  int hsv[3];

  fxFltVars[idx][0] = fxFltVars[idx][0] + (0.0083333333 * fxIntVars[idx][3]); // 1/2 of 60th second
  if (fxFltVars[idx][0] > (12.0 * PI)) {
    fxFltVars[idx][0] = 0.0;
  }

  // Plasma algorithm
  for (int x = 0; x < SQRT_LEDS; x++) {
    for (int y = 0; y < SQRT_LEDS; y++) {
      float v = 0.0;
      float cx = (x - 8.0) / 16.0;
      float cy = (y - 8.0) / 16.0;
      v += sin((cx + fxFltVars[idx][0]));
      v += sin((cy + fxFltVars[idx][0]) / 2.0);
      v += sin((cx + cy + fxFltVars[idx][0]) / 2.0);

      cx += sin(fxFltVars[idx][0] / 3.0);
      cy += cos(fxFltVars[idx][0] / 2.0);
      v += sin(sqrt(cx * cx + cy * cy + 1.0) + fxFltVars[idx][0]);
      v = v / 2.0;

      switch (fxCurrentVariationNo[idx]) {
      case 0: // b&w
        color[0] = (int)((sin(PI * v) * .5 + .5) * 256.0);
        color[1] = color[0];
        color[2] = color[0];
        break;
      case 1: // rgb
        color[0] = (int)((sin(PI * v) * .5 + .5) * 256.0);
        color[1] = (int)((sin((PI * v) + (2.0 * PI / 3.0)) * .5 + .5) * 256.0);
        color[2] = (int)((sin((PI * v) + (4.0 * PI / 3.0)) * .5 + .5) * 256.0);
        break;
      case 2: // red pinky yellow
        color[0] = 255;
        color[1] = (int)((sin(PI * v) * .5 + .5) * 256.0);
        color[2] = (int)((cos(PI * v) * .5 + .5) * 256.0);
        break;
      case 3: // blue green
        color[0] = 0;
        color[1] = (int)((sin(PI * v) * .5 + .5) * 256.0);
        color[2] = (int)((cos(PI * v) * .5 + .5) * 256.0);
        break;
      case 4:
        color[2] = 255;
        color[0] = (int)((sin(PI * v) * .5 + .5) * 256.0);
        color[1] = (int)((cos(PI * v) * .5 + .5) * 256.0);
        break;
      case 5:
        color[2] = 0;
        color[0] = (int)((sin(PI * v) * .5 + .5) * 256.0);
        color[1] = (int)((cos(PI * v) * .5 + .5) * 256.0);
        break;
      case 6:
        color[1] = 255;
        color[2] = (int)((sin(PI * v) * .5 + .5) * 256.0);
        color[0] = (int)((cos(PI * v) * .5 + .5) * 256.0);
        break;
      case 7:
        color[1] = 0;
        color[2] = (int)((sin(PI * v) * .5 + .5) * 256.0);
        color[0] = (int)((cos(PI * v) * .5 + .5) * 256.0);
        break;
      case 8:
        color[1] = 255;
        color[2] = (int)((sin(PI * v) * .5 + .5) * 256.0);
        color[0] = (int)((cos(PI * v) * .5 + .5) * 256.0);

        // hue shift by frame count
        RGBtoHSV(color[0], color[1], color[2], hsv);
        HSVtoRGB((hsv[0] + fxIntVars[idx][4]) % 1536, hsv[1], hsv[2], 0, color);

        break;
      case 9:
        color[1] = 0;
        color[2] = (int)((sin(PI * v) * .5 + .5) * 256.0);
        color[0] = (int)((cos(PI * v) * .5 + .5) * 256.0);

        // hue shift by frame count
        RGBtoHSV(color[0], color[1], color[2], hsv);
        HSVtoRGB((hsv[0] + fxIntVars[idx][4]) % 1536, hsv[1], hsv[2], 0, color);
        break;
      }

      if (useCustomColour) {
        int HSVColor[3];
        RGBtoHSV(color[0], color[1], color[2], HSVColor);
        //just use the level applied to custom color
        HSVtoRGB(fxCustomColour[idx][3], 255, HSVColor[2], 0, color);
      }

      fakeLEDsFB[idx][(x + y * SQRT_LEDS)] = CRGB(color[0], color[1], color[2]);
    }
  }

  fxIntVars[idx][4] = fxIntVars[idx][4] + 8;
  if (fxIntVars[idx][4] >= 1536) {
    fxIntVars[idx][4] = 0;
  }
}

// Stobe with fade
// Programmed by MEO from scratch - based on my Random Strobe, but with the
// fade from my Larson Scanner
// ToDo: merge with Random Strobe, with Fade as a flag
// ToDo: as with Larson - fade also moves through rainbow - not too
// noticable, as only 28 steps - but would be nice to offset
//parameter ideas   - colour bend while fade param - colour change across string param - fade time param
void Program08StrobeFade(int idx) {
  if (isInitialised[idx] == false) { // Initialize effect?
    // create non-repeating randomised table
    // Start off ordered
    for (int range = 0; range < NUM_LEDS; range++) {
      randomTable[idx][range] = range;
    }
    // Random shuffle, so, randomTable[0]=38, randomTable[1]=13.. etc..
    // Random rand = new Random();
    for (int shuffle = 0; shuffle < NUM_LEDS - 1; shuffle++) {
      int myrand = shuffle + random(NUM_LEDS - shuffle);
      int save = randomTable[idx][shuffle];
      randomTable[idx][shuffle] = randomTable[idx][myrand];
      randomTable[idx][myrand] = save;
    }

    fxIntVars[idx][0] = 0; // eye position
    fxIntVars[idx][1] = random(3) * 512; // Colour on color wheel
    // Frame-to-frame hue increment (speed) -- may be positive or negative,
    // but magnitude shouldn't be so small as to be boring. It's generally
    // still less than a full pixel per frame, making motion very smooth.
    fxIntVars[idx][2] = 5; // 4 + random(fxIntVars[idx][1]) / _NUM_LEDS;
    // //1 was 4
    // Reverse speed and hue shift direction half the time.
    if (random(2) == 0)
      fxIntVars[idx][1] = -fxIntVars[idx][1];
    if (random(2) == 0)
      fxIntVars[idx][2] = -fxIntVars[idx][2];
    fxIntVars[idx][3] = 0; // Current position
    // fxIntVars[idx][4] = random(4); // full rainbow or one of the lines
    // fxIntVars[idx][5] = random(3); // whether fixed colour,
    // rainbow, or rainbow
    // change color fade

    // Variations
    setVariation(8, 9, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 0; //same as 'Fixed Color', will check elsewhere
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }


    // Log.i(TAG_SETTINGS, "Strobe Fade Var: " +
    // fxCurrentVariationNo[idx]);

    // fxInt1 = span of colour - increases, so 0 will become higher etc
    // - 0 is single color across string
    // - 1536 is all colours across string (or 1024 for RG/GB/BR)
    // - half and double also used
    //String varName = "";
    switch (fxCurrentVariationNo[idx]) {
    case 0: // Fixed Colour
      fxIntVars[idx][5] = 0;
      fxIntVars[idx][4] = 0;
      //varName = "Fixed color";
      break;
    case 1: // Rainbow
      fxIntVars[idx][5] = 1;
      fxIntVars[idx][4] = 0;
      //varName = "Rainbow";
      break;
    case 2: // RG
      fxIntVars[idx][5] = 1;
      fxIntVars[idx][4] = 1;
      //varName = "Red to green";
      break;
    case 3: // GB
      fxIntVars[idx][5] = 1;
      fxIntVars[idx][4] = 2;
      //varName = "Green to blue";
      break;
    case 4: // BR
      fxIntVars[idx][5] = 1;
      fxIntVars[idx][4] = 3;
      //varName = "Blue to red";
      break;
    case 5: // Rainbow change
      fxIntVars[idx][5] = 2;
      fxIntVars[idx][4] = 0;
      //varName = "Rainbow fade";
    case 6: // RG change
      fxIntVars[idx][5] = 2;
      fxIntVars[idx][4] = 1;
      //varName = "Red to green fade";
      break;
    case 7: // GB change
      fxIntVars[idx][5] = 2;
      fxIntVars[idx][4] = 2;
      //varName = "Green to blue fade";
      break;
    case 8: // BR change
      fxIntVars[idx][5] = 2;
      fxIntVars[idx][4] = 3;
      //varName = "Blue to red fade";
      break;
    }
    //currentMessage = "Strobe Fade\n" + varName + "\n";
    isInitialised[idx] = true; // end initialise
  }

  int offset;
  int color[3];

  for (int i = 0; i < NUM_LEDS; i++) {
    offset = (NUM_LEDS + fxIntVars[idx][0] - randomTable[idx][i]) % NUM_LEDS;

    if (fxIntVars[idx][5] == 0) {
      if (useCustomColour) {
        HSVtoRGB(fxCustomColour[idx][3], 255, GetSmoothFade27(offset), 0, color);
      }
      else {
        HSVtoRGB(fxIntVars[idx][1], 255, GetSmoothFade27(offset), 0, color);
      }
    }
    else if (fxIntVars[idx][5] == 1) {// rainbow - the complex
      // color term is due to
      // needing to fade in the
      // same colour - otherwise
      // the fade carry on
      // rotating through colours
      HSVtoRGB(((fxIntVars[idx][3] + fxIntVars[idx][1] * i / NUM_LEDS) - (offset * fxIntVars[idx][2])) % 1536, 255,
        GetSmoothFade27(offset), fxIntVars[idx][4], color);
    }
    else { // fade changes colour
      HSVtoRGB(((fxIntVars[idx][3] + fxIntVars[idx][1] * i / NUM_LEDS) + (offset * fxIntVars[idx][2]) * 10) % 1536,
        255, GetSmoothFade27(offset), fxIntVars[idx][4], color);
    }
    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }

  // increase/decrease eye position
  fxIntVars[idx][0]++;

  // for rainbow version - move through rainbow
  fxIntVars[idx][3] += fxIntVars[idx][2];
}

// Random splash - ToDo: add ripple effect
// Programmed by MEO from scratch
//parameter ideas - colour bend while fade param - colour change across string param - fade time param
void Program09SplashFade(int idx) {
  int bulbArray;
  if (isInitialised[idx] == false) { // Initialize effect?
    // create non-repeating randomised table
    // Start off ordered
    for (int range = 0; range < NUM_LEDS; range++) {
      randomTable[idx][range] = range;
    }
    // Random shuffle, so, randomTable[0]=38, randomTable[1]=13.. etc..
    // Random rand = new Random();
    for (int shuffle = 0; shuffle < NUM_LEDS - 1; shuffle++) {
      int myrand = shuffle + random(NUM_LEDS - shuffle);
      int save = randomTable[idx][shuffle];
      randomTable[idx][shuffle] = randomTable[idx][myrand];
      randomTable[idx][myrand] = save;
    }

    fxIntVars[idx][0] = random(6) * 512; // Colour on color wheel
    fxIntVars[idx][1] = 0; // add bulb to splash (not yet randomised)
    fxIntVars[idx][2] = 5;
    // Reverse speed and hue shift direction half the time.
    if (random(2) == 0)
      fxIntVars[idx][1] = -fxIntVars[idx][1];
    if (random(2) == 0)
      fxIntVars[idx][2] = -fxIntVars[idx][2];
    fxIntVars[idx][3] = 0; // Current position
    fxIntVars[idx][4] = random(4); // full rainbow or one of the
    // lines
    fxIntVars[idx][5] = random(3); // whether fixed colour,
    // rainbow, or rainbow
    // change color fade

    fxIntVars[idx][6] = 0; // add frame for the new bulb to splash
    fxIntVars[idx][7] = 0; // add bulb to splash (random version)
    fxIntVars[idx][8] = 10; // a new splash every x frames

    // 50 can be lower for higher 'new splash every x frames' - e.g. 10
    for (bulbArray = 0; bulbArray < fxIntVars[idx][8]; bulbArray++) {
      fxArrVars[idx][0][bulbArray] = -1; // reset cache bulbs
      fxArrVars[idx][1][bulbArray] = -1; // reset cache frames
    }
    fxFrameCount[idx] = 0; // overall frame/time

    // Variations
    setVariation(9, 9, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 0; //same as 'fixed color', will check elsewhere
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    // Log.i(TAG_SETTINGS, "Splash Var: " + fxCurrentVariationNo[idx]);
    //String varName = "";
    switch (fxCurrentVariationNo[idx]) {
    case 0: // fixed colour
      fxIntVars[idx][4] = 0;
      fxIntVars[idx][5] = 0;
      fxIntVars[idx][8] = 10; // a new splash every 10 frames
      //varName = "Fixed color";
      break;
    case 1:
      fxIntVars[idx][4] = 0;
      fxIntVars[idx][5] = 1;
      fxIntVars[idx][8] = 10;
      //varName = "Rainbow";
      break;
    case 2:
      fxIntVars[idx][4] = 0;
      fxIntVars[idx][5] = 2;
      fxIntVars[idx][8] = 10;
      //varName = "Rainbow fade";
      break;
    case 3: // RG
      fxIntVars[idx][4] = 1;
      fxIntVars[idx][5] = 1;
      fxIntVars[idx][8] = 20; // a new splash every 20 frames
      //varName = "Red to green";
      break;
    case 4:
      fxIntVars[idx][4] = 1;
      fxIntVars[idx][5] = 2;
      fxIntVars[idx][8] = 5; // a new splash every 5 frames
      //varName = "Red to green fade";
      break;
    case 5:
      fxIntVars[idx][4] = 2;
      fxIntVars[idx][5] = 1;
      fxIntVars[idx][8] = 20;
      //varName = "Green to blue";
      break;
    case 6:
      fxIntVars[idx][4] = 2;
      fxIntVars[idx][5] = 2;
      fxIntVars[idx][8] = 5;
      //varName = "Green to blue fade";
      break;
    case 7:
      fxIntVars[idx][4] = 3;
      fxIntVars[idx][5] = 1;
      fxIntVars[idx][8] = 20;
      //varName = "Blue to red";
      break;
    case 8:
      fxIntVars[idx][4] = 3;
      fxIntVars[idx][5] = 2;
      fxIntVars[idx][8] = 5;
      //varName = "Blue to red fade";
      break;
    }
    //currentMessage = "Splash fade\n" + varName + "\n";
    isInitialised[idx] = true; // end initialise
    // Log.i(TAG, "Initialised Program: Splash Fade");
  }

  //fxIntVars[idx][8] = 8 + (slider3 / 32);

  fxIntVars[idx][7] = randomTable[idx][fxIntVars[idx][1]];
  int color[3];
  int splash;

  // new splash every fxI32Vars[idx][4] frames
  if ((fxFrameCount[idx] % fxIntVars[idx][8]) == 0) {
    fxIntVars[idx][6] = fxFrameCount[idx];
    fxIntVars[idx][1]++;

    // insert newest flash at start, and move rest along
    // 48 can be lower for higher 'new splash every x frames' - e.g.
    // 8
    for (bulbArray = fxIntVars[idx][8]; bulbArray >= 0; bulbArray--) {
      // shift bulbs in cache
      fxArrVars[idx][0][bulbArray + 1] = fxArrVars[idx][0][bulbArray];
      // shift frames in cache
      fxArrVars[idx][1][bulbArray + 1] = fxArrVars[idx][1][bulbArray];
    }
    // insert new bulb into cache
    fxArrVars[idx][0][0] = fxIntVars[idx][7];
    // insert new frame into cache
    fxArrVars[idx][1][0] = fxIntVars[idx][6];
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    splash = 0;

    // 50 can be lower for higher 'new splash every x frames' - e.g.
    // 10
    for (bulbArray = 0; bulbArray < fxIntVars[idx][8]; bulbArray++) { // 0; <10; ++
      // ignore cache items not yet set
      if (fxArrVars[idx][0][bulbArray] >= 0) {
        splash = splash + GetSplash(i, fxFrameCount[idx], fxArrVars[idx][0][bulbArray], fxArrVars[idx][1][bulbArray], 0.5);
      }
    }
    if (splash > 255) {
      splash = 255;
    }

    if (fxIntVars[idx][5] == 0) {
      if (useCustomColour) {
        HSVtoRGB(fxCustomColour[idx][3], 255, splash, 0, color);
      }
      else {
        HSVtoRGB(fxIntVars[idx][0], 255, splash, 0, color);
      }
    }
    else if (fxIntVars[idx][5] == 1) {// rainbow - the complex
      // color term is due to
      // needing to fade in the
      // same colour - otherwise
      // the fade carry on
      // rotating through colours

      HSVtoRGB(((fxIntVars[idx][3] + fxIntVars[idx][0] * i / NUM_LEDS)) % 1536, 255, splash,
        fxIntVars[idx][4], color);

    }
    else { // quicker change colour

      HSVtoRGB(((fxIntVars[idx][3] + fxIntVars[idx][0] * i / NUM_LEDS) * 10) % 1536, 255, splash,
        fxIntVars[idx][4], color);
    }

    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }

  // increase overall frame counter
  fxFrameCount[idx]++;

  // reset at end of bulbs;
  if (fxIntVars[idx][1] == NUM_LEDS) {
    fxIntVars[idx][1] = 0;
  }
}

void Program10SlowFill(int idx) {
  int color[3];
  int count, sequence;
  if (isInitialised[idx] == false) {
    fxIntVars[idx][0] = 0; // sequence step
    // fxIntVars[idx][1] = random(3); // chaser pattern
    fxIntVars[idx][2] = 1; // number of pixels in a row with //*POTENTIOMETER THIS*
    // specific color
    fxIntVars[idx][3] = random(1536); // color starting point
    fxIntVars[idx][4] = 1; // count step
    fxIntVars[idx][5] = fxIntVars[idx][3]; // remember starting color
    // for reset
    fxFrameCount[idx] = 1;

    // Variations
    setVariation(10, 3, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 3;
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    // Log.i(TAG_SETTINGS, "Slow Fill Var: " +
    // fxCurrentVariationNo[idx]);
    //String varName = "";
    for (int i = 0; i < NUM_LEDS; i++) {
      // set to black

      fakeLEDsFB[idx][i] = CRGB(0, 0, 0);

      // set still colors
      // ToDo: there should be no need for the following fixed bulbs,
      // if can only chase up to an ever decreaing point whilst
      // leaving the last bulb lit

      switch (fxCurrentVariationNo[idx]) {
      case 0: // rainbow fill
        HSVtoRGB(// 9 is for fade9
          (fxIntVars[idx][3] + (9 * i) % 1536), 255, 255, 0, color);
        fxPixelBuffer[idx][NUM_LEDS - 1 - i][0][0] = color[0];
        fxPixelBuffer[idx][NUM_LEDS - 1 - i][1][0] = color[1];
        fxPixelBuffer[idx][NUM_LEDS - 1 - i][2][0] = color[2];
        //varName = "Rainbow";
        break;
      case 1: // RB fill
        HSVtoRGB(0, 255, 255, 0, color); // red
        if (i % 2 == 0) {
          HSVtoRGB(1024, 255, 255, 0, color); // blue
        }
        fxPixelBuffer[idx][NUM_LEDS - 1 - i][0][0] = color[0];
        fxPixelBuffer[idx][NUM_LEDS - 1 - i][1][0] = color[1];
        fxPixelBuffer[idx][NUM_LEDS - 1 - i][2][0] = color[2];
        //varName = "Red and blue";
        break;
      case 2: // RG fill
        HSVtoRGB(0, 255, 255, 0, color); // Red
        if (i % 2 == 0) {
          HSVtoRGB(512, 255, 255, 0, color); // green
        }
        fxPixelBuffer[idx][NUM_LEDS - 1 - i][0][0] = color[0];
        fxPixelBuffer[idx][NUM_LEDS - 1 - i][1][0] = color[1];
        fxPixelBuffer[idx][NUM_LEDS - 1 - i][2][0] = color[2];
        //varName = "Red and green";
        break;
      case 3: // Custom color
        HSVtoRGB(
          fxCustomColour[idx][3], 255, 255, 0, color);
        fxPixelBuffer[idx][NUM_LEDS - 1 - i][0][0] = color[0];
        fxPixelBuffer[idx][NUM_LEDS - 1 - i][1][0] = color[1];
        fxPixelBuffer[idx][NUM_LEDS - 1 - i][2][0] = color[2];
        //varName = "Custom Color";
        break;
      }
    }
    isInitialised[idx] = true; // Effect initialized
  }

  count = fxIntVars[idx][4];
  sequence = fxIntVars[idx][0];

  while (count-- > 0) {
    // The x in 'sequence++ / x' decides how many of the bulbs in a
    // row are the same (stops looking too flickery when
    // appropriate)
    switch (fxCurrentVariationNo[idx]) {
    case 0:
      ChaseSeqFadeTrail9Bright(sequence++, fxIntVars[idx][3], color);
      break;
    case 1:
      ChaseSeqFadeTrail9RBBright(sequence++, fxIntVars[idx][3], color);
      break;
    case 2:
      ChaseSeqFadeTrail9RGBright(sequence++, fxIntVars[idx][3], color);
      break;
    case 3:
      ChaseSeqFadeTrail9Custom(sequence++, fxCustomColour[idx][3], color);
      break;
    }

    // work out stationary bulbs at the end...
    // endLength = ((frame - stringlength) DIV (spacing - 1)) + 1
    int endLength1 = fxFrameCount[idx] - NUM_LEDS;
    int endLength2 = (9 - 1); // 9 = fade9
    int endLength3 = endLength1 / endLength2;
    int endLength4 = endLength1 % endLength2;
    int endLength = 0;
    if (endLength4 > 0) { // if a remainder
      endLength = endLength3 + 1;
    }
    else {
      endLength = endLength3;
    }

    // chasing pixels
    for (int i = 0; i < NUM_LEDS - endLength; i++) {
      if (i == count) {
        fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
      }
    }
    // still pixels
    for (int i = NUM_LEDS - endLength; i < NUM_LEDS; i++) {
      fakeLEDsFB[idx][i] = CRGB(fxPixelBuffer[idx][i][0][0], fxPixelBuffer[idx][i][1][0], fxPixelBuffer[idx][i][2][0]);
    }

    if (endLength == NUM_LEDS) {// reset at end
      isInitialised[idx] = false; // Effect not initialized, so goes
      // to start again
    }
  }

  if (fxIntVars[idx][4] < NUM_LEDS) {
    ++fxIntVars[idx][4];
  }
  else {
    ++fxIntVars[idx][0];
  }

  fxFrameCount[idx]++;
}

void Program11LarsonScanners(int idx) {
  if (isInitialised[idx] == false) { // Initialize effect?
    //Serial.println("Start Init Larson");
    fxIntVars[idx][0] = 0; // white position
    // Number of repetitions (complete loops around color wheel); any
    // more than 4 per meter just looks too chaotic and un-rainbow-like.
    // Store as hue 'distance' around complete belt:
    fxIntVars[idx][1] = 0; // 1536; //(4 + random(1 * ((_NUM_LEDS
    // + 31) / 32))) * 1536; //1 was 4
    // Frame-to-frame hue increment (speed) -- may be positive or
    // negative,
    // but magnitude shouldn't be so small as to be boring. It's
    // generally
    // still less than a full pixel per frame, making motion very
    // smooth.
    fxIntVars[idx][2] = 20; // 4 + random(fxIntVars[idx][1]) / //*POTENTIOMETER THIS*
    // _NUM_LEDS; //1 was 4
    // Reverse speed and hue shift direction half the time.
    if (random(2) == 0)
      fxIntVars[idx][1] = -fxIntVars[idx][1];
    if (random(2) == 0)
      fxIntVars[idx][2] = -fxIntVars[idx][2];
    fxIntVars[idx][3] = 0; // Current position
    fxIntVars[idx][4] = 1; // increase step 1 / decrease step 0
    // fxIntVars[idx][5] = random(4); // full rainbow / RG only
    // / GB / BR
    // fxIntVars[idx][6] = random(3); // size 1 - 8, 16, 32
    // fxIntVars[idx][7] = random(3); // size 2 - 8, 16, 32
    // fxIntVars[idx][8] = random(2); // white or complimentary
    // color

    //String varName = "";
    setVariation(11, 5, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 5;
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }
    
    switch (fxCurrentVariationNo[idx]) {
    case 0:
      fxIntVars[idx][5] = 0; // Rainbow
      fxIntVars[idx][6] = SQRT_LEDS; // size
      fxIntVars[idx][8] = 0; // white or compilentary
      //varName = "White";
      break;
    case 1:
      fxIntVars[idx][5] = 1; // RG
      fxIntVars[idx][6] = SQRT_LEDS * 2;
      fxIntVars[idx][8] = 1;
      //varName = "Red to green";
      break;
    case 2:
      fxIntVars[idx][5] = 2; // GB
      fxIntVars[idx][6] = SQRT_LEDS * 2;
      fxIntVars[idx][8] = 0;
      //varName = "Green to blue";
      break;
    case 3:
      fxIntVars[idx][5] = 3; // BR
      fxIntVars[idx][6] = SQRT_LEDS * 2;
      fxIntVars[idx][8] = 1;
      //varName = "Blue to red";
      break;
    case 4:
      fxIntVars[idx][5] = 0; // Rainbow
      fxIntVars[idx][6] = SQRT_LEDS * 4;
      fxIntVars[idx][8] = 1;
      //varName = "Rainbow";
      break;
    case 5: // Custom, but set up same as 0
      fxIntVars[idx][5] = 0;
      fxIntVars[idx][6] = SQRT_LEDS; // size
      fxIntVars[idx][8] = 0; // white or compilentary
      //varName = "Custom Color";
      break;
    }

    isInitialised[idx] = true; // Effect initialized
    //Serial.println("End Init Larson");
  }

  bool white = false;

  switch (fxIntVars[idx][8]) {
  case 0:
    white = true;
    break;
  case 1:
    white = false;
    break;
  }

  int color[3];
  int i;

  int backCol = 0;
  for (i = 0; i < NUM_LEDS; i++) {
    if (white) {
      backCol = (fxIntVars[idx][3] + fxIntVars[idx][1] * i / NUM_LEDS);
    }
    else {
      backCol = ((fxIntVars[idx][3] + fxIntVars[idx][1] * i / NUM_LEDS) + 768);
    }

    if (useCustomColour) {
      HSVtoRGB(fxCustomColour[idx][3], 255, 63, // was 31
        fxIntVars[idx][5], color);
    }
    else {
      HSVtoRGB(backCol, 255, 31, // was 31
        fxIntVars[idx][5], color);
    }

    // note: in reverse fade order, so that brightest 'wins' when can be
    // either
    int saturation = 255;
    
    if (i % fxIntVars[idx][6] == GetSimpleOscillatePos(fxIntVars[idx][0] - 3, fxIntVars[idx][6], fxIntVars[idx][6], NUM_LEDS)) {
      if (white == true) {
        saturation = 127;
      }
      HSVtoRGB(fxIntVars[idx][3] + fxIntVars[idx][1] * i / NUM_LEDS, saturation, 63, fxIntVars[idx][5], color);
    }
    if (i % fxIntVars[idx][6] == GetSimpleOscillatePos(fxIntVars[idx][0] - 2, fxIntVars[idx][6], fxIntVars[idx][6], NUM_LEDS)) {
      if (white == true) {
        saturation = 63;
      }
      HSVtoRGB(fxIntVars[idx][3] + fxIntVars[idx][1] * i / NUM_LEDS, saturation, 95, fxIntVars[idx][5], color);
    }

    if (i % fxIntVars[idx][6] == GetSimpleOscillatePos(fxIntVars[idx][0] - 1, fxIntVars[idx][6], fxIntVars[idx][6], NUM_LEDS)) {
      if (white == true) {
        saturation = 31;
      }
      HSVtoRGB(fxIntVars[idx][3] + fxIntVars[idx][1] * i / NUM_LEDS, saturation, 127, fxIntVars[idx][5], color);
    }

    if (i % fxIntVars[idx][6] == GetSimpleOscillatePos(fxIntVars[idx][0], fxIntVars[idx][6], fxIntVars[idx][6], NUM_LEDS)) {
      if (white == true) {
        saturation = 15;
      }
      HSVtoRGB(fxIntVars[idx][3] + fxIntVars[idx][1] * i / NUM_LEDS, saturation, 255, fxIntVars[idx][5], color);
    }

    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }
  
  fxIntVars[idx][3] += fxIntVars[idx][2];

  // //Make a bit more interesing by gradually tightening rainbow span
  // size
  // step in direction
  if (fxIntVars[idx][4] == 1) {
    fxIntVars[idx][1]++;
  }
  else {
    fxIntVars[idx][1]--;
  }
  // change direction
  if (fxIntVars[idx][1] == 2500) {
    fxIntVars[idx][4] = 0;
  }
  if (fxIntVars[idx][1] == -1) {
    fxIntVars[idx][4] = 1;
  }

  fxIntVars[idx][0]++; // white (or complimentary) position
}

// Pulse by elmerfud
// (http://forums.adafruit.com/viewtopic.php?f=47&t=29844&p=150244&hilit=advanced+belt#p150244)
// "I added a ... simple one that picks a color and pulses the entire strip.
// Pulse entire image with solid color

void Program12SolidColorPulse(int idx) {
  if (isInitialised[idx] == false) {
    // lcd.setCursor(0, 0);
    // lcd.print("Pulse           ");
    fxIntVars[idx][0] = 50; // Pulse ammount min (v)
    fxIntVars[idx][1] = 250; // Pulse ammount max (v)

    if (useCustomColour) {
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
      fxIntVars[idx][2] = fxCustomColour[idx][3];
    }
    else {
      fxIntVars[idx][2] = random(1536); // Random hue //*POTENTIOMETER THIS*
    }
    fxIntVars[idx][3] = fxIntVars[idx][0]; // pulse position
    fxIntVars[idx][4] = 1; // 0 = negative, 1 = positive
    fxIntVars[idx][5] = 2 + random(10); // step value

    isInitialised[idx] = true; // Effect initialized
    // Log.i(TAG, "Initialised Program: Solid Color Pulse");
    //currentMessage = "Solid Color Pulse\n";
  }

  // byte *ptr = &imgData[idx][0];
  int i;
  int color[3];
  for (i = 0; i < NUM_LEDS; i++) {
    HSVtoRGB(fxIntVars[idx][2], 255, fxIntVars[idx][3], 0, color);
    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }

  if (fxIntVars[idx][4] == 0) {
    fxIntVars[idx][3] = fxIntVars[idx][3] - fxIntVars[idx][5];
    if (fxIntVars[idx][3] <= fxIntVars[idx][0]) {
      fxIntVars[idx][4] = 1;
      fxIntVars[idx][3] = fxIntVars[idx][0];
    }
  }
  else if (fxIntVars[idx][4] == 1) {
    fxIntVars[idx][3] = fxIntVars[idx][3] + fxIntVars[idx][5];
    if (fxIntVars[idx][3] >= fxIntVars[idx][1]) {
      fxIntVars[idx][4] = 0;
      fxIntVars[idx][3] = fxIntVars[idx][1];
    }
  }
}

// Random strobe effect
// (inspired by the Eiffel Tower! :
// Note: currently only works with 50, 100 or 160 pixels (easy to add more)
// To Do - see if can use my code to generate non repeating random nos on
// the fly

void Program13RandomStrobe(int idx) {
  if (isInitialised[idx] == false) {
    // create non-repeating randomised table
    // Start off ordered
    for (int range = 0; range < NUM_LEDS; range++) {
      randomTable[idx][range] = range;
    }
    // Random shuffle, so, randomTable[0]=38, randomTable[1]=13.. etc..
    // Random rand = new Random();
    for (int shuffle = 0; shuffle < NUM_LEDS - 1; shuffle++) {
      int myrand = shuffle + random(NUM_LEDS - shuffle);
      int save = randomTable[idx][shuffle];
      randomTable[idx][shuffle] = randomTable[idx][myrand];
      randomTable[idx][myrand] = save;
    }

    // lcd.setCursor(0, 0);
    // lcd.print("Random Strobe   ");
    fxIntVars[idx][0] = 0; // Current position
    // fxIntVars[idx][1] = random(9); // sub pattern / variation
    fxIntVars[idx][2] = 0; // step through effect
    // Number of repetitions (complete loops around color wheel); any
    // more than 4 per meter just looks too chaotic and un-rainbow-like.
    // Store as hue 'distance' around complete belt:
    fxIntVars[idx][3] = 512; // 1536; //(4 + random(1 * ((_NUM_LEDS +
    // 31) / 32))) * 1536; //1 was 4
    // Frame-to-frame hue increment (speed) -- may be positive or
    // negative,
    // but magnitude shouldn't be so small as to be boring. It's
    // generally
    // still less than a full pixel per frame, making motion very
    // smooth.
    fxIntVars[idx][4] = 5; // 4 + random(fxIntVars[idx][1]) / _NUM_LEDS;
    // //1 was 4
    // Reverse speed and hue shift direction half the time.
    if (random(2) == 0)
      fxIntVars[idx][3] = -fxIntVars[idx][3];
    if (random(2) == 0)
      fxIntVars[idx][4] = -fxIntVars[idx][4];

    // ToDo: make number of bulbs at a time work
    // ToDo: slow down, by having bulbs stay same for a number of frames

    // Variations
    setVariation(13, 10, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 10;
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    // Log.i(TAG_SETTINGS, "Strobe Var: " + fxCurrentVariationNo[idx]);
    //String varName = "";
    switch (fxCurrentVariationNo[idx]) {
    case 0: // Black - white flash
      //varName = "Black and white";
      break;
    case 1: // Blue - white flash
      //varName = "Blue and white";
      break;
    case 2: // Red - white flash
      //varName = "Red and white";
      break;
    case 3: // Green - white flash
      //varName = "Green and white";
      break;
    case 4: // Black - rainbow flash - all bulbs of single colour before
      //varName = "Black and rainbow";
      break;
    case 5: // Rainbow backround - white flash
      //varName = "Rainbow and white";
      break;
    case 6: // Rainbow background - Rainbow flash
      //varName = "Rainbow and compliment";
      break;
    case 7: // red backround - blue flash
      //varName = "Red and blue";
      break;
    case 8: // blue backround - red flash
      //varName = "Blue and red";
      break;
    case 9: // purple backround green flash
      //varName = "Purple and green";
      break;
    case 10: // purple backround green flash
      //varName = "Custom color and white";
      break;
    }
    //currentMessage = "Random strobe\n" + varName + "\n";
    isInitialised[idx] = true; // Effect initialized
    // Log.i(TAG, "Initialised Program: Random Strobe");
  }

  // byte *ptr = &imgData[idx][0];

  bool rainbowFlash_ = false; // instead of single strobe color, will
  // cycle through colour wheel
  bool rainbowMain_ = false; // instead of single background color,
  // will cycle through colour wheel
  // (complimentary colour to flash)
  int rMain = 0; // backround colour
  int gMain = 0;
  int bMain = 0;
  int rFlash = 0; // flash colour
  int gFlash = 0;
  int bFlash = 0;

  switch (fxCurrentVariationNo[idx]) {
  case 0: // Black - white flash
    rainbowMain_ = false;
    rainbowFlash_ = false;
    rMain = 0;
    gMain = 0;
    bMain = 0;
    rFlash = 255;
    gFlash = 255;
    bFlash = 255;
    break;
  case 1: // Blue - white flash
    rainbowMain_ = false;
    rainbowFlash_ = false;
    rMain = 0;
    gMain = 0;
    bMain = 51;
    rFlash = 255;
    gFlash = 255;
    bFlash = 255;
    break;
  case 2: // Red - white flash
    rainbowMain_ = false;
    rainbowFlash_ = false;
    rMain = 51;
    gMain = 0;
    bMain = 0;
    rFlash = 255;
    gFlash = 255;
    bFlash = 255;
    break;
  case 3: // Green - white flash
    rainbowMain_ = false;
    rainbowFlash_ = false;
    rMain = 0;
    gMain = 51;
    bMain = 0;
    rFlash = 255;
    gFlash = 255;
    bFlash = 255;
    break;
  case 4: // Black - rainbow flash - all bulbs of single colour before
    // change
    rainbowMain_ = false;
    rainbowFlash_ = true;
    rMain = 0;
    gMain = 0;
    bMain = 0;
    break;
  case 5: // Rainbow backround - white flash
    rainbowMain_ = true;
    rainbowFlash_ = false;
    rFlash = 255;
    gFlash = 255;
    bFlash = 255;
    break;
  case 6: // Rainbow background - Rainbow flash
    rainbowMain_ = true;
    rainbowFlash_ = true;
    rFlash = 255;
    gFlash = 255;
    bFlash = 255;
    break;
  case 7: // red backround - blue flash
    rainbowMain_ = false;
    rainbowFlash_ = false;
    rMain = 51;
    gMain = 0;
    bMain = 0;
    rFlash = 0;
    gFlash = 0;
    bFlash = 255;
    break;
  case 8: // blue backround - red flash
    rainbowMain_ = false;
    rainbowFlash_ = false;
    rMain = 0;
    gMain = 0;
    bMain = 51;
    rFlash = 255;
    gFlash = 0;
    bFlash = 0;
    break;
  case 9: // purplish backround - greenish flash
    rainbowMain_ = false;
    rainbowFlash_ = false;
    rMain = 51;
    gMain = 0;
    bMain = 36;
    rFlash = 0;
    gFlash = 255;
    bFlash = 21;
    break;
  case 10: // custom color and white flash
    rainbowMain_ = false;
    rainbowFlash_ = false;
    rFlash = 255;
    gFlash = 255;
    bFlash = 255;
    break;
  }

  //*POTENTIOMETER THIS* - any way to have this to how many bulbs at a time? instead of always one.

  int color[3];
  for (int i = 0; i < NUM_LEDS; i++) {
    if (randomTable[idx][i] == fxIntVars[idx][2]) {
      if (rainbowFlash_) {
        // int color;
        HSVtoRGB(fxIntVars[idx][0] + fxIntVars[idx][3] * i / NUM_LEDS, 255, 255, 0, color);

        fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
      }
      else {
        fakeLEDsFB[idx][i] = CRGB(rFlash, gFlash, bFlash);

      }
    }
    else {
      if (rainbowMain_) {
        HSVtoRGB((fxIntVars[idx][0] + fxIntVars[idx][3] * i / NUM_LEDS) + 768, 255, 100, 0, color); // 768 so main hue is 180 deg from flash colour
        fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
      }
      else {
        if (useCustomColour) {
          HSVtoRGB(fxCustomColour[idx][3], 255, 63, 0, color);
          fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
        }
        else {
          fakeLEDsFB[idx][i] = CRGB(rMain, gMain, bMain);
        }
      }
    }
  }

  fxIntVars[idx][2]++;

  // reset
  if (fxIntVars[idx][2] == NUM_LEDS) {
    fxIntVars[idx][2] = 0;
  }

  fxIntVars[idx][0] += fxIntVars[idx][4];
}

// Larson Scanner
// Programmed by MEO from scratch
// ToDo: When hits either end, the fade stops dead.
// Can't really tell at 60Hz, but still it's bugging me, and I'd like to fix
// it
// ToDo: as with strobe fade - fade also moves through rainbow - not too
// noticable, as only 28 steps - but would be nice to offset

void Program14LarsonHeartbeat(int idx) {
  if (isInitialised[idx] == false) { // Initialize effect?
    // lcd.setCursor(0, 0);
    // lcd.print("Larson Scanner  ");
    fxIntVars[idx][0] = NUM_LEDS; // position - start one loop in, so
    // can count backwards
    fxIntVars[idx][1] = random(6) * 256; // Colour on color wheel

    // Frame-to-frame hue increment (speed) -- may be positive or
    // negative,
    // but magnitude shouldn't be so small as to be boring. It's
    // generally
    // still less than a full pixel per frame, making motion very
    // smooth.
    fxIntVars[idx][2] = 5; // 4 + random(fxIntVars[idx][1]) / _NUM_LEDS;
    // //1 was 4
    // Reverse speed and hue shift direction half the time.
    if (random(2) == 0)
      fxIntVars[idx][1] = -fxIntVars[idx][1];
    if (random(2) == 0)
      fxIntVars[idx][2] = -fxIntVars[idx][2];
    fxIntVars[idx][3] = 0; // Current position
    // fxIntVars[idx][4] = random(4); // full rainbow or one of
    // the
    // lines
    // fxIntVars[idx][5] = random(2); // whether to rainbow, or
    // fixed
    // colour
    setVariation(14, 5, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 1;
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    //String varName = "";
    switch (fxCurrentVariationNo[idx]) {
    case 0:
      fxIntVars[idx][4] = 0;
      fxIntVars[idx][5] = 0;
      fxIntVars[idx][6] = SQRT_LEDS * 4; // translates to
      // oscillations
      // per string (when div into num
      // pixels)
      fxIntVars[idx][7] = (SQRT_LEDS * 4) + 4; // length of
      // oscillate Forward
      fxIntVars[idx][8] = (SQRT_LEDS * 4) + 4; // length of
      // oscillate Back
      //varName = "Fixed random color";
      break;
    case 1:
      fxIntVars[idx][4] = 0;
      fxIntVars[idx][5] = 1;
      fxIntVars[idx][6] = SQRT_LEDS * 4;
      fxIntVars[idx][7] = (SQRT_LEDS * 4) + 4;
      fxIntVars[idx][8] = (SQRT_LEDS * 4) + 4;
      //varName = "Rainbow";
      break;
    case 2:
      //varName = "Red to green";
      fxIntVars[idx][4] = 1;
      fxIntVars[idx][5] = 1;
      fxIntVars[idx][6] = SQRT_LEDS * 4;
      fxIntVars[idx][7] = (SQRT_LEDS * 4) + 4;
      fxIntVars[idx][8] = (SQRT_LEDS * 4) + 4;
      break;
    case 3:
      //varName = "Green to blue";
      fxIntVars[idx][4] = 2;
      fxIntVars[idx][5] = 1;
      fxIntVars[idx][6] = SQRT_LEDS * 4;
      fxIntVars[idx][7] = (SQRT_LEDS * 4) + 4;
      fxIntVars[idx][8] = (SQRT_LEDS * 4) + 4;
      break;
    case 4:
      //varName = "Blue to red";
      fxIntVars[idx][4] = 3;
      fxIntVars[idx][5] = 1;
      fxIntVars[idx][6] = SQRT_LEDS * 4;
      fxIntVars[idx][7] = (SQRT_LEDS * 4) + 4;
      fxIntVars[idx][8] = (SQRT_LEDS * 4) + 4;
      break;
    }
    //currentMessage = "Larson Heartbeat\n" + varName + "\n";
    isInitialised[idx] = true; // end initialise
    // Log.i(TAG, "Initialised Program: Larson Heartbeat");
  }

  int offset;
  int color[3];
  for (int i = 0; i < NUM_LEDS; i++) {
    // do backwards trail offset, so brighter overrides dimmer when
    // overlap
    HSVtoRGB(0, 0, 0, 0, color); // background
    for (offset = 27; offset >= 0; offset--) { // works with
      // GetSmoothFade9,
      // but overkill
      if (i % fxIntVars[idx][6] == GetSimpleOscillatePos(fxIntVars[idx][0] - offset, fxIntVars[idx][7], fxIntVars[idx][8],
        NUM_LEDS)) {
        if (fxIntVars[idx][5] == 1) { // rainbow
          if (useCustomColour) { //custom color - a few degrees either side of hue
            HSVtoRGB(((((fxIntVars[idx][3] + fxIntVars[idx][1] * i / NUM_LEDS) - 80) % 160 + fxCustomColour[idx][3]) % 1536) - offset, 255,
              GetSmoothFade27(offset), fxIntVars[idx][4], color);
          }
          else {
            HSVtoRGB((fxIntVars[idx][3] + fxIntVars[idx][1] * i / NUM_LEDS) - offset, 255,
              GetSmoothFade27(offset), fxIntVars[idx][4], color);
          }
        }
        else { // fixed color
          HSVtoRGB(fxIntVars[idx][1], 255, GetSmoothFade27(offset), 0, color);
        }
      }

    }
    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }

  // for rainbow version - move through rainbow
  fxIntVars[idx][3] += fxIntVars[idx][2];

  // increase oscillate step
  fxIntVars[idx][0]++;
}

// MEO

void Program15Flames(int idx) {
  int transition, intensity;
  if (isInitialised[idx] == false) { // Initialize effect?
    setVariation(15, 3, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 3;
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    // Log.i(TAG_SETTINGS, "Flames Var: " + fxCurrentVariationNo[idx]);
    //String varName = "";
    switch (fxCurrentVariationNo[idx]) {
    case 0: // fire
      fxIntVars[idx][0] = 101; // intensity Hi
      fxIntVars[idx][1] = 0; // intensity Lo
      fxIntVars[idx][2] = 101; // transition Hi
      fxIntVars[idx][3] = 0; // transition Lo

      fxIntVars[idx][5] = 255; // Colour 1 R
      fxIntVars[idx][6] = 0; // Colour 1 G
      fxIntVars[idx][7] = 0; // Colour 1 B
      fxIntVars[idx][8] = 255; // Colour 2 R
      fxIntVars[idx][9] = 145; // Colour 2 G
      fxIntVars[idx][10] = 0; // Colour 2 B

      fxIntVars[idx][12] = 8; // how many frames between change
      //varName = "Hot";
      break;
    case 1: // ice
      fxIntVars[idx][0] = 101; // intensity Hi
      fxIntVars[idx][1] = 0; // intensity Lo
      fxIntVars[idx][2] = 101; // transition Hi
      fxIntVars[idx][3] = 0; // transition Lo

      fxIntVars[idx][5] = 0; // Colour 1 R
      fxIntVars[idx][6] = 0; // Colour 1 G
      fxIntVars[idx][7] = 255; // Colour 1 B
      fxIntVars[idx][8] = 0; // Colour 2 R
      fxIntVars[idx][9] = 145; // Colour 2 G
      fxIntVars[idx][10] = 255; // Colour 2 B

      fxIntVars[idx][12] = 15; // how many frames between change
      //varName = "Ice";
      break;
    case 2: //
      fxIntVars[idx][0] = 101; // intensity Hi
      fxIntVars[idx][1] = 0; // intensity Lo
      fxIntVars[idx][2] = 101; // transition Hi
      fxIntVars[idx][3] = 0; // transition Lo

      fxIntVars[idx][5] = 0; // Colour 1 R
      fxIntVars[idx][6] = 0; // Colour 1 G
      fxIntVars[idx][7] = 255; // Colour 1 B
      fxIntVars[idx][8] = 255; // Colour 2 R
      fxIntVars[idx][9] = 255; // Colour 2 G
      fxIntVars[idx][10] = 0; // Colour 2 B

      fxIntVars[idx][12] = 15; // how many frames between change
      //varName = "Mystical";
      break;
    case 3: // custom color
      fxIntVars[idx][0] = 101; // intensity Hi
      fxIntVars[idx][1] = 0; // intensity Lo
      fxIntVars[idx][2] = 101; // transition Hi
      fxIntVars[idx][3] = 0; // transition Lo

      fxIntVars[idx][5] = fxCustomColour[idx][0]; // Colour 1 R
      fxIntVars[idx][6] = fxCustomColour[idx][1]; // Colour 1 G
      fxIntVars[idx][7] = fxCustomColour[idx][2]; // Colour 1 B

      int RGBColor[3];
      //hue rotated a tiny bit, and lower level
      HSVtoRGB((fxCustomColour[idx][3] + 45) % 1536, 255, 127, 0, RGBColor);

      fxIntVars[idx][8] = RGBColor[0]; // Colour 2 R
      fxIntVars[idx][9] = RGBColor[1]; // Colour 2 G
      fxIntVars[idx][10] = RGBColor[2]; // Colour 2 B

      fxIntVars[idx][12] = 45; // how many frames between change
      //varName = "Custom Color";
      break;
    }

    fxIntVars[idx][11] = 0; // frame count for smooth fade

    // store first frame's worth of pixels
    for (int i = 0; i < NUM_LEDS; i++) {
      transition = random(fxIntVars[idx][2] - fxIntVars[idx][3]) + fxIntVars[idx][3];
      intensity = random(fxIntVars[idx][0] - fxIntVars[idx][1]) + fxIntVars[idx][1];

      fxPixelBuffer[idx][i][0][1] = ((fxIntVars[idx][8] - fxIntVars[idx][5]) * transition / 100 + fxIntVars[idx][5]) * intensity / 100;
      fxPixelBuffer[idx][i][1][1] = ((fxIntVars[idx][9] - fxIntVars[idx][6]) * transition / 100 + fxIntVars[idx][6]) * intensity / 100;
      fxPixelBuffer[idx][i][2][1] = ((fxIntVars[idx][10] - fxIntVars[idx][7]) * transition / 100 + fxIntVars[idx][7]) * intensity / 100;
    }
    //currentMessage = "Flames\n" + varName + "\n";
    fxFrameCount[idx] = 0;
    isInitialised[idx] = true; // end initialise
    // Log.i(TAG, "Initialised Program: Flames");
  }

  if (fxIntVars[idx][11] == 0) {
    // get next 10th frame's worth of pixels
    for (int i = 0; i < NUM_LEDS; i++) {
      transition = random(fxIntVars[idx][2] - fxIntVars[idx][3]) + fxIntVars[idx][3];
      intensity = random(fxIntVars[idx][0] - fxIntVars[idx][1]) + fxIntVars[idx][1];
      // swap
      fxPixelBuffer[idx][i][0][0] = fxPixelBuffer[idx][i][0][1];
      // new
      fxPixelBuffer[idx][i][0][1] = ((fxIntVars[idx][8] - fxIntVars[idx][5]) * transition / 100 + fxIntVars[idx][5]) * intensity / 100;
      fxPixelBuffer[idx][i][1][0] = fxPixelBuffer[idx][i][1][1];
      fxPixelBuffer[idx][i][1][1] = ((fxIntVars[idx][9] - fxIntVars[idx][6]) * transition / 100 + fxIntVars[idx][6]) * intensity / 100;
      fxPixelBuffer[idx][i][2][0] = fxPixelBuffer[idx][i][2][1];
      fxPixelBuffer[idx][i][2][1] = ((fxIntVars[idx][10] - fxIntVars[idx][7]) * transition / 100 + fxIntVars[idx][7]) * intensity / 100;
    }
  }

  // show merged results for smooth fade between super-frames
  // byte *ptr = &imgData[idx][0];
  int color[3] = { 0, 0, 0 };
  int hsv[3];
  for (int i = 0; i < NUM_LEDS; i++) {
    // gradual crossfaded mixure of 1st and 2nd set of frames
    color[0] = (fxPixelBuffer[idx][i][0][1] * fxIntVars[idx][11] / fxIntVars[idx][12])
      + (fxPixelBuffer[idx][i][0][0] * (fxIntVars[idx][12] - fxIntVars[idx][11]) / fxIntVars[idx][12]);
    color[1] = (fxPixelBuffer[idx][i][1][1] * fxIntVars[idx][11] / fxIntVars[idx][12])
      + (fxPixelBuffer[idx][i][1][0] * (fxIntVars[idx][12] - fxIntVars[idx][11]) / fxIntVars[idx][12]);
    color[2] = (fxPixelBuffer[idx][i][2][1] * fxIntVars[idx][11] / fxIntVars[idx][12])
      + (fxPixelBuffer[idx][i][2][0] * (fxIntVars[idx][12] - fxIntVars[idx][11]) / fxIntVars[idx][12]);
    // *ptr++ = r; *ptr++ = g; *ptr++ = b;

    if (fxCurrentVariationNo[idx] == 2) {
      // color rotate variation 2 - mystical
      RGBtoHSV(color[0], color[1], color[2], hsv);
      HSVtoRGB((hsv[0] + fxFrameCount[idx]) % 1536, hsv[1], hsv[2], 0, color);
    }
    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }

  // smooth fade counter
  fxIntVars[idx][11]++;
  if (fxIntVars[idx][11] == fxIntVars[idx][12]) {
    fxIntVars[idx][11] = 0;
  }

  // colour shift counter
  fxFrameCount[idx]++;
  if (fxFrameCount[idx] > 1536) {
    fxFrameCount[idx] = 0;
  }
}

// - All one colour, but level set by fadeTable 0 1 2 3 2 1 0 1 2 3 2 1 0 -
// around all bulbs
// - variations: rotate bulbs / slowly invert fade / rotate & invert
// gap between each level for another colour 0 D 1 C 2 B 3 A 3 B 2 C 1 D 0
// rotate in opps directions
// no gap so colour add up at peak

void Program16ComplimentaryFade(int idx) {
  if (isInitialised[idx] == false) {
    fxIntVars[idx][2] = 0; // frame count

    // Variations
    setVariation(16, 5, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 5;
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }
    // Log.i(TAG_SETTINGS, "Comp Fade Var: " +
    // fxCurrentVariationNo[idx]);
    //String varName = "";
    switch (fxCurrentVariationNo[idx]) {
    case 0: // Red/Blue
      fxIntVars[idx][0] = 0;
      fxIntVars[idx][1] = 1024;
      //varName = "Red and blue";
      break;
    case 1: // Purples
      fxIntVars[idx][0] = 1400;
      fxIntVars[idx][1] = 1160;
      //varName = "Purples";
      break;
    case 2: // Random Hue, and compliment
      fxIntVars[idx][0] = random(1536); // Random hue
      fxIntVars[idx][1] = (fxIntVars[idx][0] + 768) % 1536; // complementary
      //varName = "Random color and compliment";
      break;
    case 3: // Random Hue, and compliment rotation
      fxIntVars[idx][0] = random(1536); // Random hue
      fxIntVars[idx][1] = (fxIntVars[idx][0] + 768) % 1536; // complementary
      //varName = "Color and compliment shift";
      break;
    case 4: // Random Hue, and compliment opposite rotation
      fxIntVars[idx][0] = random(1536); // Random hue
      fxIntVars[idx][1] = (fxIntVars[idx][0] + 768) % 1536; // complementary
      //varName = "Color shift and reverse shift";
      break;
    case 5: // Custom Color
      fxIntVars[idx][0] = (fxCustomColour[idx][3] - 64) % 1536; // 'left' rotation
      fxIntVars[idx][1] = (fxCustomColour[idx][3] + 64) % 1536; // 'right' rotation
      //varName = "Custom Color";
      break;
    }
    //currentMessage = "Complimentary Fade\n" + varName + "\n";
    isInitialised[idx] = true; // Effect initialized
    // Log.i(TAG, "Initialised Program: Complementary Fade");
  }

  int color[3];
  int fade1, fade2;
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i % 2 == 1) {
      fade1 = (i + fxIntVars[idx][2]) % SQRT_LEDS;
      HSVtoRGB(fxIntVars[idx][0] % 1536, 255, GetSmoothFade27(fade1), 0, color);
    }
    else {
      fade2 = (32 - i + fxIntVars[idx][2]) % SQRT_LEDS;
      if (fade2 < 0) {
        fade2 += 16;
      }
      // + 1536 is for variation 4
      HSVtoRGB((fxIntVars[idx][1] + 1536) % 1536, 255, GetSmoothFade27(fade2), 0, color);
    }
    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }

  if (fxCurrentVariationNo[idx] == 3) {
    // rotate colors
    fxIntVars[idx][0]++;
    fxIntVars[idx][1]++;
  }
  else if (fxCurrentVariationNo[idx] == 4) {
    // rotate colors
    fxIntVars[idx][0]++;
    fxIntVars[idx][1]--;
  }

  fxIntVars[idx][2]++;
  if (fxIntVars[idx][2] > 1536) {
    fxIntVars[idx][2] = 0;
  }
}

void Program17RandomLevels(int idx) {
  if (isInitialised[idx] == false) {
    // create non-repeating randomised table
    // Start off ordered
    for (int range = 0; range < NUM_LEDS; range++) {
      randomTable[idx][range] = (255 * range) / NUM_LEDS;
    }
    // Random shuffle, so, randomTable[0]=38, randomTable[1]=13.. etc..
    // Random rand = new Random();
    for (int shuffle = 0; shuffle < NUM_LEDS - 1; shuffle++) {
      int myrand = shuffle + random(NUM_LEDS - shuffle);
      int save = randomTable[idx][shuffle];
      randomTable[idx][shuffle] = randomTable[idx][myrand];
      randomTable[idx][myrand] = save;
    }

    fxIntVars[idx][0] = random(1536); // Random hue
    fxIntVars[idx][1] = 0; // local crossfade level
    fxIntVars[idx][2] = 0; // random level
    fxIntVars[idx][3] = 0; // frame count / hue shift

    // Variations
    setVariation(17, 3, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 3;
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    //String varName = "";
    switch (fxCurrentVariationNo[idx]) {
    case 0: // Fixed Colour
      //varName = "Fixed color";
      break;
    case 1: // Slow Rainbow shift
      //varName = "Rainbow Spread";
      break;
    case 2: // Quick rainbow shift
      //varName = "Quick shift";
      break;
    case 3: // Custom color
      //varName = "Custom Color";
      break;
    }
    //currentMessage = "Random Levels\n" + varName + "\n";
    isInitialised[idx] = true; // Effect initialized
    // Log.i(TAG, "Initialised Program: Random Levels");
  }

  int color[3];
  int level1, level2, level;
  for (int i = 0; i < NUM_LEDS; i++) {
    // random level, and random shift of position each frame
    level1 = randomTable[idx][(i + fxIntVars[idx][2]) % NUM_LEDS];
    level2 = randomTable[idx][(i + fxIntVars[idx][2] + 1) % NUM_LEDS];
    level = (level1 * (8 - fxIntVars[idx][1]) / 8) + level2 * fxIntVars[idx][1] / 8;

    int hue = 0;
    switch (fxCurrentVariationNo[idx]) {
    case 0: // Fixed Colour
      hue = fxIntVars[idx][0];
      break;
    case 1: // Rainbow spread
      hue = (fxIntVars[idx][0] + (int)(((float)i / (float)NUM_LEDS) * 1536.0) + (fxIntVars[idx][3] * 8)) % 1536;
      break;
    case 2: // Quick rainbow shift
      hue = (fxIntVars[idx][0] + (fxIntVars[idx][3] * 3)) % 1536;
      break;
    case 3: // Custom Colour
      hue = fxCustomColour[idx][3];
      break;
    }

    HSVtoRGB(hue, 255, level, 0, color);
    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }

  // crossfade level (not main crossfade, just a local crossfade between
  // two random levels)
  fxIntVars[idx][1]++;
  if (fxIntVars[idx][1] == 8) {
    fxIntVars[idx][1] = 0;
  }
  if (fxIntVars[idx][1] == 0) {
    // random level
    fxIntVars[idx][2]++;
  }
  if (fxIntVars[idx][2] == NUM_LEDS) {
    fxIntVars[idx][2] = 0;
  }

  // framecount increase
  fxIntVars[idx][3]++;
  if (fxIntVars[idx][3] >= 1536) {
    fxIntVars[idx][3] = 0;
  }
}

// Fade in/out a random 20% of bulbs

void Program18GentleRandomFade(int idx) {
  if (isInitialised[idx] == false) {
    // create non-repeating randomised table
    // Start off ordered
    for (int range = 0; range < NUM_LEDS; range++) {
      randomTable[idx][range] = range;
    }
    // Random shuffle, so, randomTable[0]=38, randomTable[1]=13.. etc..
    // Random rand = new Random();
    for (int shuffle = 0; shuffle < NUM_LEDS - 1; shuffle++) {
      int myrand = shuffle + random(NUM_LEDS - shuffle);
      int save = randomTable[idx][shuffle];
      randomTable[idx][shuffle] = randomTable[idx][myrand];
      randomTable[idx][myrand] = save;
    }

    // lcd.setCursor(0, 0);
    // lcd.print("Random FadeInOut");
    fxIntVars[idx][0] = 0;
    fxIntVars[idx][1] = 1; // Frames to stay at full level
    fxIntVars[idx][2] = 0; // hue - old colour
    fxIntVars[idx][3] = random(1536); // Random hue (new colour)
    fxIntVars[idx][4] = 0; // fade position
    fxIntVars[idx][5] = 0; // frame count
    fxIntVars[idx][6] = 0; // which bulbs at a time count
    fxIntVars[idx][7] = 0; // full rainbow or one of the
    // lines
    fxIntVars[idx][9] = 0; // overall frame count
    // fxIntVars[idx][8] = random(3); // whether fixed colour,
    // rainbow, or rainbow
    // change color fade

    // ToDo: add color level, so can start off from black

    setVariation(18, 3, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 3;
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    //String varName = "";
    switch (fxCurrentVariationNo[idx]) {
    case 0:
      // Number of bulbs at a time
      fxIntVars[idx][0] = NUM_LEDS / (SQRT_LEDS / 2);
      fxIntVars[idx][7] = 0; // full rainbow
      fxIntVars[idx][8] = 0; // fixed color
      //varName = "Random colors";
      break;
    case 1:
      fxIntVars[idx][0] = NUM_LEDS / SQRT_LEDS;
      fxIntVars[idx][7] = random(3) + 1; // RG, GB or BR
      fxIntVars[idx][8] = 1; // rainbow at once
      //varName = "Rainbow";
      break;
    case 2:
      fxIntVars[idx][0] = NUM_LEDS / SQRT_LEDS;
      fxIntVars[idx][7] = 0;
      fxIntVars[idx][8] = 2; // rainbow change color fade
      //varName = "Color shift";
      break;
    case 3:
      fxIntVars[idx][0] = NUM_LEDS / SQRT_LEDS;
      fxIntVars[idx][7] = 0;
      fxIntVars[idx][8] = 0; // rainbow change color fade
      //varName = "Custom Color";
      break;
    }
    isInitialised[idx] = true; // Effect initialized
  }

  // byte *ptr = &imgData[idx][0];
  int i, outLo, outHi, inLo, inHi;
  int color[3];
  int rBulb;

  // the bulbs fading out
  outLo = (NUM_LEDS + (fxIntVars[idx][6] * fxIntVars[idx][0]) - fxIntVars[idx][0]) % NUM_LEDS;
  outHi = (NUM_LEDS + (fxIntVars[idx][6] * fxIntVars[idx][0]) - 1) % NUM_LEDS;

  // the bulbs fading in
  inLo = (NUM_LEDS + (fxIntVars[idx][6] * fxIntVars[idx][0])) % NUM_LEDS;
  inHi = (NUM_LEDS + (fxIntVars[idx][6] * fxIntVars[idx][0]) + fxIntVars[idx][0] - 1) % NUM_LEDS;
  for (i = 0; i < NUM_LEDS; i++) {
    rBulb = randomTable[idx][i]; // for
    // non-repeating
    // random
    // rBulb = i; // for sequential

    HSVtoRGB(fxIntVars[idx][3], 255, 0, 0, color); // most
    // bulbs
    // are
    // off

    if ((rBulb >= inLo) && (rBulb <= inHi)) {
      switch (fxIntVars[idx][8]) {
      case 0:
        if (useCustomColour) {
          HSVtoRGB(fxCustomColour[idx][3], 255, fxIntVars[idx][4], fxIntVars[idx][7], color);
        }
        else {
          HSVtoRGB(fxIntVars[idx][3], 255, fxIntVars[idx][4], 0, color); // fade
        }
        // in
        // new
        // set
        break;
      case 1:
        // rainbow - the complex
        // color term is due to
        // needing to fade in
        // the
        // same colour -
        // otherwise
        // the fade carry on
        // rotating through
        // colours
        HSVtoRGB(((fxIntVars[idx][3] + (i * 1024) / NUM_LEDS)) % 1024, 255, fxIntVars[idx][4], fxIntVars[idx][7], color);
        break;
      case 2:
        HSVtoRGB(((fxIntVars[idx][3] + fxIntVars[idx][0] * i / NUM_LEDS) + fxIntVars[idx][9]) % 1536, 255,
          fxIntVars[idx][4], fxIntVars[idx][7], color);
        break;
      }
    }
    else if ((rBulb >= outLo) && (rBulb <= outHi)) {
      switch (fxIntVars[idx][8]) {
      case 0:
        if (useCustomColour) {
          HSVtoRGB(fxCustomColour[idx][3], 255, fxIntVars[idx][4], fxIntVars[idx][7], color);
        }
        else {
          HSVtoRGB(fxIntVars[idx][2], 255, 255 - fxIntVars[idx][4], 0, color); // fade
        }
        // out
        // last set
        break;
      case 1:
        HSVtoRGB(((fxIntVars[idx][2] + (i * 1024) / NUM_LEDS)) % 1024, 255, 255 - fxIntVars[idx][4],
          fxIntVars[idx][7], color);
        break;
      case 2:
        HSVtoRGB(((fxIntVars[idx][2] + fxIntVars[idx][0] * i / NUM_LEDS) + fxIntVars[idx][9]) % 1536, 255,
          255 - fxIntVars[idx][4], fxIntVars[idx][7], color);
        break;
      }
    }
    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }

  // fade counter - if brighness level < 255 AND frame count == 0
  if ((fxIntVars[idx][4] < 255) && (fxIntVars[idx][5] == 0)) {
    fxIntVars[idx][4]++; // increase brightness (decrease brightness
    // old)
    // Log.i(TAG, "x");
  }

  // increase frame count, only when at max brightess
  // frame counter - if brightness == 255
  if (fxIntVars[idx][4] == 255) {
    fxIntVars[idx][5]++; // increase frame count
    // Log.i(TAG, "Reached max brightness");
  }

  // start over, with new bulbs - if frame count == frames to stay at
  // full level
  if (fxIntVars[idx][5] == fxIntVars[idx][1]) {
    fxIntVars[idx][5] = 0; // reset frame count
    fxIntVars[idx][4] = 0; // reset brightness
    fxIntVars[idx][6]++; // go to next set of bulbs

    fxIntVars[idx][2] = fxIntVars[idx][3]; // save last colour
    if (useCustomColour) {
      fxIntVars[idx][3] = random(1536); // new colour
    }

    // Log.i(TAG, "Start next");
  }

  // start completely over
  if (fxIntVars[idx][6] >= NUM_LEDS) {
    fxIntVars[idx][6] = 0;
    // Log.i(TAG, "Start over");
  }

  fxIntVars[idx][9] = fxIntVars[idx][9] + 2;
  if (fxIntVars[idx][9] > 1536) {
    fxIntVars[idx][9] = 0;
  }
}

void Program19RandomCompLevels(int idx) {
  if (isInitialised[idx] == false) {
    // create non-repeating randomised table
    // Start off ordered
    for (int range = 0; range < NUM_LEDS; range++) {
      randomTable[idx][range] = (255 * range) / NUM_LEDS;
    }
    // Random shuffle, so, randomTable[0]=38, randomTable[1]=13.. etc..
    // Random rand = new Random();
    for (int shuffle = 0; shuffle < NUM_LEDS - 1; shuffle++) {
      int myrand = shuffle + random(NUM_LEDS - shuffle);
      int save = randomTable[idx][shuffle];
      randomTable[idx][shuffle] = randomTable[idx][myrand];
      randomTable[idx][myrand] = save;
    }

    fxIntVars[idx][0] = random(1536); // Random hue
    fxIntVars[idx][1] = 0; // local crossfade level
    fxIntVars[idx][2] = 0; // random level
    fxIntVars[idx][3] = 0; // frame count / hue shift

    // Variations
    setVariation(19, 3, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 3;
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    //String varName = "";
    switch (fxCurrentVariationNo[idx]) {
    case 0: // Fixed Colour
      //varName = "Fixed color";
      break;
    case 1: // Slow Rainbow shift
      //varName = "Slow shift";
      break;
    case 2: // Quick rainbow shift
      //varName = "Quick shift";
      break;
    case 3:
      //varName = "Custom Color";
      break;
    }
    //currentMessage = "Random Complimentary Levels\n" + varName + "\n";
    isInitialised[idx] = true; // Effect initialized
    // Log.i(TAG, "Initialised Program: Random Levels");
  }

  int color[3];
  int compColor[3];
  int level1, level2, level;
  for (int i = 0; i < NUM_LEDS; i++) {
    // random level, and random shift of position each frame
    level1 = randomTable[idx][(i + fxIntVars[idx][2]) % NUM_LEDS];
    level2 = randomTable[idx][(i + fxIntVars[idx][2] + 1) % NUM_LEDS];
    level = (level1 * (8 - fxIntVars[idx][1]) / 8) + level2 * fxIntVars[idx][1] / 8;

    int hue = 0;
    switch (fxCurrentVariationNo[idx]) {
    case 0: // Fixed Colour
      hue = fxIntVars[idx][0];
      break;
    case 1: // Slow Rainbow shift
      hue = (fxIntVars[idx][0] + fxIntVars[idx][3]) % 1536;
      break;
    case 2: // Quick rainbow shift
      hue = (fxIntVars[idx][0] + (fxIntVars[idx][3] * 8)) % 1536;
      break;
    case 3: // Custom
      hue = fxCustomColour[idx][3];
      break;
    }

    if (useCustomColour) {
      HSVtoRGB(hue, 255, level, 0, color);
      HSVtoRGB((hue + 768) % 1536, 255, (255 - level) / 3, 0, compColor);
    }
    else {
      HSVtoRGB(hue, 255, level, 0, color);
      HSVtoRGB((hue + 768) % 1536, 255, (255 - level), 0, compColor);
    }
    fakeLEDsFB[idx][i] = CRGB(color[0] + compColor[0], color[1] + compColor[1], color[2] + compColor[2]);
  }

  // crossfade level (not main crossfade, just a local crossfade between
  // two random levels)
  fxIntVars[idx][1]++;
  if (fxIntVars[idx][1] == 8) {
    fxIntVars[idx][1] = 0;
  }
  if (fxIntVars[idx][1] == 0) {
    // random level
    fxIntVars[idx][2]++;
  }
  if (fxIntVars[idx][2] == NUM_LEDS) {
    fxIntVars[idx][2] = 0;
  }

  // framecount increase
  fxIntVars[idx][3]++;
  if (fxIntVars[idx][3] >= 1536) {
    fxIntVars[idx][3] = 0;
  }
}

// Pulse entire image with solid color, from white
void Program20BrightPulse(int idx) {
  if (isInitialised[idx] == false) { // Initialize effect?
    fxIntVars[idx][0] = random(3) * 512; // random(1536); //
    fxIntVars[idx][3] = 0; // Current position
    fxIntVars[idx][7] = 0; // start of rainbow

    // Variations
    setVariation(20, 3, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 3;
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    //String varName = "";
    switch (fxCurrentVariationNo[idx]) {
    case 0: // red
      fxIntVars[idx][4] = 2;
      fxIntVars[idx][5] = 0;
      fxIntVars[idx][6] = 0;
      //varName = "Electric Red";
      break;
    case 1: // blue
      fxIntVars[idx][4] = 2;
      fxIntVars[idx][5] = 0;
      fxIntVars[idx][6] = 0;
      //varName = "Electric Blue";
      break;
    case 2: // green
      fxIntVars[idx][4] = 2;
      fxIntVars[idx][5] = 0;
      fxIntVars[idx][6] = 0;
      //varName = "Electric Green";
      break;
    case 3: // custom
      fxIntVars[idx][4] = 2;
      fxIntVars[idx][5] = 0;
      fxIntVars[idx][6] = 0;
      //varName = "Custom Color";
      break;
    }

    fxIntVars[idx][2] = 4 + random(fxIntVars[idx][0] + 1) / NUM_LEDS;
    // Reverse direction half the time.
    if (random(2) == 0)
      fxIntVars[idx][2] = -fxIntVars[idx][2];
    //currentMessage = "Bright Pulse\n" + varName + "\n";
    isInitialised[idx] = true; // Effect initialized
  }

  int color[3];
  int i;
  float y;

  y = (float)(sin(PI * fxIntVars[idx][4] * 0.5 * fxIntVars[idx][3] / NUM_LEDS) * 255.0);

  for (i = 0; i < NUM_LEDS; i++) {
    // Peaks of sine wave are white, troughs are black, mid-range
    // values are pure hue (100% saturated).
    // Bright pulse based on Sine Wave, but no variation across string
    // -following placed ouside loop, without ref to i
    // y = (float) (sin(PI * fxIntVars[idx][4] * 0.5 *
    // (fxIntVars[idx][3] + (float) i) / NUM_LEDS) * 255.0);

    int hue = 0;
    switch (fxCurrentVariationNo[idx]) {
    case 0: // red
      hue = 0;
      break;
    case 1: // blue
      hue = 1024;
      break;
    case 2: // green
      hue = 512;
      break;
    case 3: // custom color
      hue = fxCustomColour[idx][3];
      break;
    }

    if (y >= 0.0) {
      // Peaks of sine wave are white (saturation = 0)
      HSVtoRGB(hue, 255 - (int)y, 255, fxIntVars[idx][6], color);
    }
    else {
      // troughs are black (level = 0)
      HSVtoRGB(hue, 255, 255 + (int)y, fxIntVars[idx][6], color);
    }

    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }
  fxIntVars[idx][3] += fxIntVars[idx][2];
  fxIntVars[idx][7]++;
}

// Dither colour change

void Program21Stipple(int idx) {
  int x, y, b;
  if (isInitialised[idx] == false) {
    fxIntVars[idx][1] = 63; // random(1, NUM_LEDS); // run, in pixels
    fxIntVars[idx][2] = 255; // (random(2) == 1) ? 255 : -255; //
    // direction

    // generate dithered table
    // Determine highest bit needed to represent pixel index
    int hiBit = 0;
    int n = (256 - 1) - 1;
    for (int bit = 1; bit < 0x8000; bit <<= 1) {
      if (n & bit) hiBit = bit;
    }

    int pos = 0;
    int bit, reverse;

    for (int i = 0; i < (hiBit << 1); i++) {
      // Reverse the bits in i for ordered dither:
      reverse = 0;
      for (bit = 1; bit <= hiBit; bit <<= 1) {
        reverse <<= 1;
        if (i & bit) reverse |= 1;
      }
      // Log.i("MEOPos", "pos: " + pos + ",  reverse: " + reverse);
      if (pos < NUM_LEDS)
      {
        ditheredTable[pos] = reverse;
      }

      pos++;
    }

    // Variations
    setVariation(21, 3, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 3;
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    //String varName = "";
    switch (fxCurrentVariationNo[idx]) {
    case 0:
      fxIntVars[idx][3] = 0; // start colour
      fxIntVars[idx][4] = 512; // shift by
      fxIntVars[idx][5] = 35; // transition time
      //varName = "RGB";
      break;
    case 1:
      fxIntVars[idx][3] = 256;
      fxIntVars[idx][4] = 512;
      fxIntVars[idx][5] = 35;
      //varName = "CYM";
      break;
    case 2:
      fxIntVars[idx][3] = 0;
      fxIntVars[idx][4] = 721;
      fxIntVars[idx][5] = 35;
      //varName = "Rainbow";
      break;
    case 3:
      fxIntVars[idx][3] = fxCustomColour[idx][3];
      fxIntVars[idx][4] = 0;
      fxIntVars[idx][5] = 35;
      //varName = "Custom Color";
      break;
    }

    fxIntVars[idx][6] = 0; // counter

    //currentMessage = "Stipple\n" + varName + "\n";

    fxBlnVars[idx][0] = false;

    isInitialised[idx] = true; // Effect initialized
  }

  int color1[3];
  int color2[3];
  int colorSwap[3];
  HSVtoRGB(fxIntVars[idx][3], 255, 255, 0, color1);
  if (useCustomColour) {
    HSVtoRGB(fxIntVars[idx][3], 255, 0, 0, color2); //black
    if (fxBlnVars[idx][0]) {
      colorSwap[0] = color1[0];
      color1[0] = color2[0];
      color2[0] = colorSwap[0];
      colorSwap[1] = color1[1];
      color1[1] = color2[1];
      color2[1] = colorSwap[1];
      colorSwap[2] = color1[2];
      color1[2] = color2[2];
      color2[2] = colorSwap[2];
    }
  }
  else {
    HSVtoRGB((fxIntVars[idx][3] + fxIntVars[idx][4]) % 1536, 255, 255, 0, color2);
  }

  fxIntVars[idx][6]++;

  // Note: regardless of NUM_LEDS - Dither always uses 256
  // if (fxIntVars[idx][2] > 0) {
  b = (255 + (256 * fxIntVars[idx][2] / fxIntVars[idx][1])) * fxIntVars[idx][6] / fxIntVars[idx][5]
    - (256 * fxIntVars[idx][2] / fxIntVars[idx][1]);
  // } else {
  // b = (255 - (NUM_LEDS * fxIntVars[idx][2] / fxIntVars[idx][1])) *
  // fxIntVars[idx][6] / fxIntVars[idx][5];
  // }

  for (x = 0; x < NUM_LEDS; x++) {
    y = ditheredTable[x] * fxIntVars[idx][2] / fxIntVars[idx][1] + b;

    double mult1 = 0.0, mult2 = 1.0;
    int max = ditheredTable[x];
    if (max < 256) {
      if (y < 0) {
        mult1 = 0.0;
        mult2 = 1.0;
      }
      else if (y >= (256 - 1)) {
        mult1 = 1.0;
        mult2 = 0.0;
      }
      else {
        mult1 = y / 256.0;
        mult2 = 1.0 - mult1;
      }
    }
    fakeLEDsFB[idx][x] = CRGB((int)((double)color1[0] * mult2 + (double)color2[0] * mult1), (int)((double)color1[1] * mult2 + (double)color2[1] * mult1), (int)((double)color1[2] * mult2 + (double)color2[2] * mult1));
  }

  // reset for next change
  if (fxIntVars[idx][6] >= fxIntVars[idx][5]) {
    fxIntVars[idx][6] = 0;
    fxIntVars[idx][3] = (fxIntVars[idx][3] + fxIntVars[idx][4]) % 1536;
    fxBlnVars[idx][0] = !fxBlnVars[idx][0];
  }
}

// Rainbow effect (1 or more full loops of color wheel at 100% saturation).
// Not a big fan of this pattern (it's way overused with LED stuff), but
// it's
// practically part of the Geneva Convention by now.
void Program22DoubleRainbow(int idx) {
  if (isInitialised[idx] == false) { // Initialize effect?
    // Number of repetitions (complete loops around color wheel); any
    // more than 4 per meter just looks too chaotic and un-rainbow-like.
    // Store as hue 'distance' around complete belt:
    // 0 = one colour at a time, 1536 (or 1024) has all colours spread
    // across string once
    // 6144 (4096) = 4 / 3072 (2048) = 2
    // fxIntVars[idx][0] = 0;

    // Frame-to-frame hue increment (speed) -- may be positive or
    // negative,
    // but magnitude shouldn't be so small as to be boring. It's
    // generally
    // still less than a full pixel per frame, making motion very
    // smooth.
    fxIntVars[idx][1] = 16;

    // Reverse speed and hue shift direction half the time.
    if (random(2) == 0)
      fxIntVars[idx][0] = -fxIntVars[idx][0];
    if (random(2) == 0)
      fxIntVars[idx][1] = -fxIntVars[idx][1];
    fxIntVars[idx][2] = 0; // Current position
    fxIntVars[idx][3] = 1; // increase step 1 / decrease step 0

    // fxIntVars[idx][4] = random(4); // full rainbow / RG only
    // / GB / BR

    // Variations
    setVariation(22, 2, idx);
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 2;
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    // fxInt1 = span of colour - increases, so 0 will become higher etc
    // - 0 is single color across string
    // - 1536 is all colours across string (or 1024 for RG/GB/BR)
    // - half and double also used
    //String varName = "";
    switch (fxCurrentVariationNo[idx]) {
    case 0: // RGB single
      fxIntVars[idx][0] = 0; //*POTENTIOMETER THIS*
      fxIntVars[idx][4] = 0;
      //varName = "Red to green to blue Single";
      break;
    case 1: // RGB span
      fxIntVars[idx][0] = 1536;
      fxIntVars[idx][4] = 0;
      //varName = "R to G to B Span";
      break;
    case 2: // Custom Color
      fxIntVars[idx][0] = 0;
      fxIntVars[idx][4] = 0;
      //varName = "Custom Color";
      break;
    }
    
    isInitialised[idx] = true; // Effect initialized
  }

  int color[3];
  int i;
  for (i = 0; i < NUM_LEDS; i++) {
    int hue1, hue2;

    if (useCustomColour) {
      hue1 = (fxCustomColour[idx][3] + 32 + (fxIntVars[idx][0] % 64)) % 1536;
      hue2 = ((fxCustomColour[idx][3] - 32 - (fxIntVars[idx][0] % 64)) - 128) % 1536;
    }
    else {
      hue1 = fxIntVars[idx][2] + fxIntVars[idx][0] * i / NUM_LEDS;
      hue2 = (hue1 + 768) % 1536;
    }

    if (i % 2 == 0) {
      HSVtoRGB(hue1, 255, 255, fxIntVars[idx][4], color);
    }
    else {
      HSVtoRGB(hue2, 255, 255, fxIntVars[idx][4], color);
    }
    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }
  fxIntVars[idx][2] += fxIntVars[idx][1];

  // step in direction
  if (fxIntVars[idx][3] == 1) {
    fxIntVars[idx][0]++;
  }
  else {
    fxIntVars[idx][0]--;
  }

  // change direction
  if (fxIntVars[idx][0] == 2500) {
    fxIntVars[idx][3] = 0;
  }
  if (fxIntVars[idx][0] == -1) {
    fxIntVars[idx][3] = 1;
  }
}

// Data for American-flag-like colors (20 pixels representing
// blue field, stars and pixelStringes). This gets "stretched" as needed
// to the full LED pixelString length in the flag effect code, below.
// Can change this data to the colors of your own national flag,
// favorite sports team colors, etc. OK to change number of elements.
// MEO: ToDo: this doens't work properly because of the
// FixedSine/FixedCosine problem
// final int USA_RED[] = { 160, 0, 0 };
// final int USA_WHITE[] = { 255, 255, 255 };
// final int USA_BLUE[] = { 0, 0, 100 };

// #define WHITE_CANDLE 255, 147, 41 //-CANDLE - 1900
// #define WHITE_40W 255, 157, 50 //-
// #define BLACK 0, 0, 0

// #define FLAME_YELLOW 255, 127, 0
// #define FLAME_ORANGE 255, 47, 00
// #define FLAME_RED 255, 0, 0

// #define STD_RED 255,0,0
// #define STD_GREEN 0,255,0
// #define STD_BLUE 0,0,255

// cheesy RGB version
/*
* byte flagTable[] = { STD_RED, BLACK, STD_GREEN, BLACK, STD_BLUE, BLACK,
* STD_RED, BLACK, STD_GREEN, BLACK, STD_BLUE, BLACK, STD_RED, BLACK,
* STD_GREEN, BLACK, STD_BLUE, BLACK};
*/

// flame effect version - RED/ORANGE - ORANGE/YELLOW
// final int flagTable[][] = {
// FLAME_YELLOW, FLAME_ORANGE, FLAME_YELLOW , FLAME_ORANGE, FLAME_YELLOW,
// FLAME_ORANGE, FLAME_YELLOW,
// FLAME_ORANGE, FLAME_RED, FLAME_ORANGE, FLAME_RED, FLAME_ORANGE, FLAME_RED
// ,
// BLACK, FLAME_RED, BLACK, FLAME_RED, BLACK, FLAME_RED, BLACK };

// original USA flag version //size = 20
// final int flagTable[][] = { USA_BLUE, USA_WHITE, USA_BLUE, USA_WHITE,
// USA_BLUE, USA_WHITE, USA_BLUE, USA_RED, USA_WHITE, USA_RED,
// USA_WHITE, USA_RED, USA_WHITE, USA_RED, USA_WHITE, USA_RED,
// USA_WHITE, USA_RED, USA_WHITE, USA_RED };

int usFlagTable[] = { 0, 0, 100, 255, 255, 255, 0, 0, 100, 255, 255, 255, 0, 0, 100, 255, 255, 255, 0, 0, 100, 160, 0, 0, 255, 255, 255, 160, 0, 0,
255, 255, 255, 160, 0, 0, 255, 255, 255, 160, 0, 0, 255, 255, 255, 160, 0, 0, 255, 255, 255, 160, 0, 0, 255, 255, 255, 160, 0, 0
};

// Wavy flag effect - USA
void Program23FlagUS(int idx) {
  long sum, s, x;
  int i, idx1, idx2, a, b;
  if (isInitialised[idx] == false) { // Initialize effect?
    fxIntVars[idx][0] = 720 + random(720); // Wavyness
    fxIntVars[idx][1] = 100; //4 + random(10); // Wave speed
    fxIntVars[idx][2] = 200 + random(200); // Wave 'puckeryness'
    fxIntVars[idx][3] = 0; // Current position

    if (useCustomColour) {
      //varName = "Custom Color";
      //if (useColor2) {
      //varName = "Custom Colors";
      //}
      DecodeCustomColour(idx);
    }

    isInitialised[idx] = true; // Effect initialized
    // Log.i(TAG, "Initialised Program: Wavy Flag");
  }

  sum = 0;
  for (i = 0; i < (NUM_LEDS - 1); i++) {
    sum += fxIntVars[idx][2] + (int)(cos(fxIntVars[idx][3] + (float)fxIntVars[idx][0] * (float)i / NUM_LEDS) * 255.0);
  }

  s = 0;
  for (i = 0; i < NUM_LEDS; i++) {
    x = 256 * ((60 / 3) - 1) * s / sum; //flag length / 3
    idx1 = (int)((x >> 8) * 3);
    idx2 = (int)(((x >> 8) + 1) * 3);
    // wrap to end if under-runs
    if (idx1 < 0) {
      idx1 = 60 - 3;//flag length - 3
    }
    // wrap to 0 if overruns
    if (idx2 >= 60) { //flag length
      idx2 = 0;
    }

    b = (int)((x & 255) + 1);
    a = 257 - b;

    int color[3];

    color[0] = ((usFlagTable[idx1] * a) + (usFlagTable[idx2] * b)) >> 8;
    color[1] = ((usFlagTable[idx1 + 1] * a) + (usFlagTable[idx2 + 1] * b)) >> 8;
    color[2] = ((usFlagTable[idx1 + 2] * a) + (usFlagTable[idx2 + 2] * b)) >> 8;

    if (useCustomColour) {
      // Hue Shift
      //            if ((useColor2) && (i%2 == 0)) {
      //                color = HueOnly(color, fxCustomColour[idx][7]);
      //            } else {
      HueOnly(color, fxCustomColour[idx][3]); //colour comes back
      //            }
    }

    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);

    s += fxIntVars[idx][2] + (long)(cos(fxIntVars[idx][3] + (float)fxIntVars[idx][0] * (float)i / NUM_LEDS) * 255.0);
  }

  fxIntVars[idx][3] += fxIntVars[idx][1];
  if (fxIntVars[idx][3] >= 720)
    fxIntVars[idx][3] -= 720;
}

// Fade in/out a random 20% of bulbs, de-saturated
void Program24CreepyHalloween(int idx) {
  int saturation = 255;
  if (isInitialised[idx] == false) {
    // create non-repeating randomised table
    // Start off ordered
    for (int range = 0; range < NUM_LEDS; range++) {
      randomTable[idx][range] = range;
    }
    // Random shuffle, so, randomTable[0]=38, randomTable[1]=13.. etc..
    // Random rand = new Random();
    for (int shuffle = 0; shuffle < NUM_LEDS - 1; shuffle++) {
      int myrand = shuffle + random(NUM_LEDS - shuffle);
      int save = randomTable[idx][shuffle];
      randomTable[idx][shuffle] = randomTable[idx][myrand];
      randomTable[idx][myrand] = save;
    }

    // lcd.setCursor(0, 0);
    // lcd.print("Random FadeInOut");
    fxIntVars[idx][0] = 0;
    fxIntVars[idx][1] = 1; // Frames to stay at full level
    fxIntVars[idx][2] = 0; // hue - old colour
    fxIntVars[idx][3] = random(1024); // Random hue (new colour)
    fxIntVars[idx][4] = 0; // fade position
    fxIntVars[idx][5] = 0; // frame count
    fxIntVars[idx][6] = 0; // which bulbs at a time count
    fxIntVars[idx][7] = 0; // full rainbow or one of the
    // lines
    fxIntVars[idx][9] = 0; // overall frame count
    // fxIntVars[idx][8] = random(3); // whether fixed colour,
    // rainbow, or rainbow
    // change color fade

    // ToDo: add color level, so can start off from black

    setVariation(20, 3, idx);
    // Log.i(TAG_SETTINGS, "Gentle Fade Var: " +
    // fxCurrentVariationNo[idx]);

    String varName = "";
    switch (fxCurrentVariationNo[idx]) {
    case 0:
      // Number of bulbs at a time
      fxIntVars[idx][0] = NUM_LEDS / (SQRT_LEDS / 2);
      fxIntVars[idx][7] = 1; // RG
      fxIntVars[idx][8] = 0; // fixed color
      //varName = "Random colors";
      break;
    case 1:
      fxIntVars[idx][0] = NUM_LEDS / SQRT_LEDS;
      fxIntVars[idx][7] = 1; // RG
      fxIntVars[idx][8] = 1; // rainbow at once
      //varName = "Rainbow";
      break;
    case 2:
      fxIntVars[idx][0] = NUM_LEDS / SQRT_LEDS;
      fxIntVars[idx][7] = 1; // RG
      fxIntVars[idx][8] = 2; // rainbow change color fade
      //varName = "Color shift";
      break;
    }

    isInitialised[idx] = true; // Effect initialized
  }

  // byte *ptr = &imgData[idx][0];
  int i, outLo, outHi, inLo, inHi;
  int color[3];
  int rBulb;

  // the bulbs fading out
  outLo = (NUM_LEDS + (fxIntVars[idx][6] * fxIntVars[idx][0]) - fxIntVars[idx][0]) % NUM_LEDS;
  outHi = (NUM_LEDS + (fxIntVars[idx][6] * fxIntVars[idx][0]) - 1) % NUM_LEDS;

  // the bulbs fading in
  inLo = (NUM_LEDS + (fxIntVars[idx][6] * fxIntVars[idx][0])) % NUM_LEDS;
  inHi = (NUM_LEDS + (fxIntVars[idx][6] * fxIntVars[idx][0]) + fxIntVars[idx][0] - 1) % NUM_LEDS;
  for (i = 0; i < NUM_LEDS; i++) {
    rBulb = randomTable[idx][i]; // for
    // non-repeating
    // random
    // rBulb = i; // for sequential

    HSVtoRGB(fxIntVars[idx][3], saturation, 0, 0, color); // most
    // bulbs
    // are
    // off

    if ((rBulb >= inLo) && (rBulb <= inHi)) {
      switch (fxIntVars[idx][8]) {
      case 0:
        // Log.i(TAG, "In 0:" + fxIntVars[idx][8]);
        HSVtoRGB(fxIntVars[idx][3], saturation, fxIntVars[idx][4], 0, color); // fade
        // in
        // new
        // set
        break;
      case 1:
        // rainbow - the complex
        // color term is due to
        // needing to fade in
        // the
        // same colour -
        // otherwise
        // the fade carry on
        // rotating through
        // colours
        // Log.i(TAG, "In 1:" + fxIntVars[idx][8]);
        HSVtoRGB(((fxIntVars[idx][3] + (i * 1024) / NUM_LEDS)) % 1024, saturation, fxIntVars[idx][4],
          fxIntVars[idx][7], color);
        break;
      case 2:
        // Log.i(TAG, "In 2:" + fxIntVars[idx][8]);
        HSVtoRGB(((fxIntVars[idx][3] + fxIntVars[idx][0] * i / NUM_LEDS) + fxIntVars[idx][9]) % 1024, saturation,
          fxIntVars[idx][4], fxIntVars[idx][7], color);
        break;
      }
    }
    else if ((rBulb >= outLo) && (rBulb <= outHi)) {

      switch (fxIntVars[idx][8]) {
      case 0:
        // Log.i(TAG, "Out 0:" + fxIntVars[idx][8]);
        HSVtoRGB(fxIntVars[idx][2], saturation, 255 - fxIntVars[idx][4], 0, color); // fade
        // out
        // last set
        break;
      case 1:
        // Log.i(TAG, "Out 1:" + fxIntVars[idx][8]);
        HSVtoRGB(((fxIntVars[idx][2] + (i * 1024) / NUM_LEDS)) % 1024, saturation, 255 - fxIntVars[idx][4],
          fxIntVars[idx][7], color);
        break;
      case 2:
        // Log.i(TAG, "Out 2:" + fxIntVars[idx][8]);
        HSVtoRGB(((fxIntVars[idx][2] + fxIntVars[idx][0] * i / NUM_LEDS) + fxIntVars[idx][9]) % 1024, saturation,
          255 - fxIntVars[idx][4], fxIntVars[idx][7], color);
        break;
      }
    }
    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }

  // fade counter - if brighness level < 255 AND frame count == 0
  if ((fxIntVars[idx][4] < 255) && (fxIntVars[idx][5] == 0)) {
    fxIntVars[idx][4]++; // increase brightness (decrease brightness old)
  }

  // increase frame count, only when at max brightess
  // frame counter - if brightness == 255
  if (fxIntVars[idx][4] == 255) {
    fxIntVars[idx][5]++; // increase frame count
    // Log.i(TAG, "Reached max brightness");
  }

  // start over, with new bulbs - if frame count == frames to stay at
  // full level
  if (fxIntVars[idx][5] == fxIntVars[idx][1]) {
    fxIntVars[idx][5] = 0; // reset frame count
    fxIntVars[idx][4] = 0; // reset brightness
    fxIntVars[idx][6]++; // go to next set of bulbs

    fxIntVars[idx][2] = fxIntVars[idx][3]; // save last colour
    fxIntVars[idx][3] = random(1024); // new colour
  }

  // start completely over
  if (fxIntVars[idx][6] >= NUM_LEDS) {
    fxIntVars[idx][6] = 0;
  }

  fxIntVars[idx][9] = fxIntVars[idx][9] + 2;
  if (fxIntVars[idx][9] > 1024) {
    fxIntVars[idx][9] = 0;
  }
}

// Simplest rendering effect: fill entire image with solid color

void Program25SolidColor(byte idx) {
  // Only needs to be rendered once, when effect is initialized:
  if (isInitialised[idx] == false) {
    //TurnExcessPixelsOff(idx, NUM_LEDS);
    // choose random color
    int color[3];

    DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]

    if (useCustomColour) {
      color[0] = fxCustomColour[idx][0];
      color[1] = fxCustomColour[idx][1];
      color[2] = fxCustomColour[idx][2];
    }
    else {
      HSVtoRGB(random(1536), 255, 255, 0, color);
    }

    for (int i = 0; i < NUM_LEDS; i++) {
      fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
    }
    isInitialised[idx] = true; // Effect initialized
  }
}

void Program26Bonfire(int idx) {
  if (isInitialised[idx] == false) { // Initialize effect?

    fxIntVars[idx][1] = 55; //Cooling //220 /4 = 55
    fxIntVars[idx][2] = 120; //Sparking //308 / 4 = 77

    // Variations
    setVariation(26, 1, idx);
    //String varName = "Flames";

    if (useCustomColour) {
      //varName = "Custom Color";
      //if (useColor2) {
      //varName = "Custom Colors";
      //}
      DecodeCustomColour(idx);
    }

    //currentMessage = "Bonfire\n" + varName + "\n";
    isInitialised[idx] = true;
  }

  int color[3];
  int i;

  //fxIntVars[idx][1] = slider3 / 4; //cooling
  //fxIntVars[idx][2] = slider4 / 4; //Sparking

  for (int i = 0; i < NUM_LEDS; i++) {
    fireTable[idx][0][i] = fireTable[idx][0][i] -
      (random16(((fxIntVars[idx][1] * 10) / NUM_LEDS) + 2));
    if (fireTable[idx][0][i] < 0) {
      fireTable[idx][0][i] = 0;
    }
  }

  for (int k = NUM_LEDS - 1; k >= 2; k--) {
    fireTable[idx][0][k] = (fireTable[idx][0][k - 1] + fireTable[idx][0][k - 2] + fireTable[idx][0][k - 2]) / 3;
  }

  if (random16(255) < fxIntVars[idx][2]) {
    int y = random16(7);
    fireTable[idx][0][y] = fireTable[idx][0][y] + (random16(95) + 160);
    if (fireTable[idx][0][y] > 255) {
      fireTable[idx][0][y] = 255;
    }
  }

  for (i = 0; i < NUM_LEDS; i++) {
    HeatRamp(fireTable[idx][0][i - 1], color);
    if (useCustomColour) {
      // Hue Shift
      //            if ((useColor2) && (i%2 == 0)) {
      //                color = HueOnly(color, fxCustomColour[idx][7]);
      //            } else {
      HueOnly(color, fxCustomColour[idx][3]); //colour comes back
      //            }
    }
    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }
}

void Program27Sparks(int idx) {
  if (isInitialised[idx] == false) { // Initialize effect?

    // Variations
    setVariation(27, 4, idx);
    //String varName = "";
    if (useCustomColour) {
      fxCurrentVariationNo[idx] = 4;
      DecodeCustomColour(idx); //fill fxCustomColour[0 to 7]
    }

    switch (fxCurrentVariationNo[idx]) {
    case 0:
      fxIntVars[idx][1] = 30; //Cooling
      fxIntVars[idx][2] = 180; //Sparking
      fxIntVars[idx][4] = 0; //hue1 - red
      //varName = "Up";
      break;
    case 1:
      fxIntVars[idx][1] = 30; //Cooling
      fxIntVars[idx][2] = 180; //Sparking
      fxIntVars[idx][3] = 1;
      fxIntVars[idx][4] = 1024; //hue1 - blue
      //varName = "Down";
      break;
    case 2:
      fxIntVars[idx][1] = 60; //Cooling
      fxIntVars[idx][2] = 90; //Sparking
      fxIntVars[idx][4] = 512; //hue1 - green
      fxIntVars[idx][5] = 1280; //hue 2 - magenta
      //varName = "Two Down";
      break;
    case 3:
      fxIntVars[idx][1] = 60; //Cooling
      fxIntVars[idx][2] = 90; //Sparking
      fxIntVars[idx][4] = 0; //hue 1 - red
      fxIntVars[idx][5] = 1024; //hue 2 - blue
      //varName = "Up and Down";
      break;
    case 4:
      fxIntVars[idx][1] = 30; //Cooling
      fxIntVars[idx][2] = 180; //Sparking
      fxIntVars[idx][4] = fxCustomColour[idx][3]; //custom 1
      //varName = "Custom Color";
      break;
    case 5:
      fxIntVars[idx][1] = 60; //Cooling
      fxIntVars[idx][2] = 90; //Sparking
      fxIntVars[idx][4] = fxCustomColour[idx][3]; //custom 1
      fxIntVars[idx][5] = fxCustomColour[idx][7]; //custom 2
      //varName = "Custom Colors";
      break;
    }
    isInitialised[idx] = true;
  }

  int color[3];
  int color2[3];
  int i, table;

  for (table = 0; table < 2; table++) {
    for (i = 0; i < NUM_LEDS; i++) {
      fireTable[idx][table][i] = fireTable[idx][0][i] -
        (random16(((fxIntVars[idx][1] * 10) / NUM_LEDS) + 2));
      if (fireTable[idx][table][i] < 0) {
        fireTable[idx][table][i] = 0;
      }
    }

    for (int k = NUM_LEDS - 1; k >= 2; k--) {
      fireTable[idx][table][k] = (fireTable[idx][table][k - 1] + fireTable[idx][table][k - 2] + fireTable[idx][table][k - 2]) / 3;
    }

    if (random16(255) < fxIntVars[idx][2]) {
      int y = random16(NUM_LEDS);
      fireTable[idx][table][y] = fireTable[idx][table][y] + random16(255);
      if (fireTable[idx][table][y] > 255) {
        fireTable[idx][table][y] = 255;
      }
    }
  }
  for (i = 0; i < NUM_LEDS; i++) {
    int finalLevel1 = 0;
    int finalLevel2 = 0;
    switch (fxCurrentVariationNo[idx]) {
    case 0: //up
      finalLevel1 = fireTable[idx][0][(NUM_LEDS - i) - 1];
      break;
    case 1: //down
      finalLevel1 = fireTable[idx][1][i];
      break;
    case 2: //both down
      finalLevel1 = fireTable[idx][0][i]; //fireTable[idx][0][(NUM_LEDS - i) - 1];
      finalLevel2 = fireTable[idx][1][i];
      break;
    case 3: //up and down
      finalLevel1 = fireTable[idx][0][(NUM_LEDS - i) - 1];
      finalLevel2 = fireTable[idx][1][i];
      break;
    case 4: //custom
      finalLevel1 = fireTable[idx][0][(NUM_LEDS - i) - 1];
      break;
    case 5: //custom 2
      finalLevel1 = fireTable[idx][0][(NUM_LEDS - i) - 1];
      finalLevel2 = fireTable[idx][1][i];
      break;
    }

    if (fxCurrentVariationNo[idx] >= 2) {
      if (finalLevel1 > 127) {
        int satur = (finalLevel1 - 127) * 2;
        if (satur > 255) {
          satur = 255;
        }
        HSVtoRGB(fxIntVars[idx][4], 255 - satur, 255, 0, color);
      }
      else {
        int lev = finalLevel1 * 2;
        if (lev > 255) {
          lev = 255;
        }
        HSVtoRGB(fxIntVars[idx][4], 255, lev, 0, color);
      }
      if (finalLevel2 > 127) {
        int satur = (finalLevel2 - 127) * 2;
        if (satur > 255) {
          satur = 255;
        }
        HSVtoRGB(fxIntVars[idx][5], 255 - satur, 255, 0, color2);
      }
      else {
        int lev = finalLevel2 * 2;
        if (lev > 255) {
          lev = 255;
        }
        HSVtoRGB(fxIntVars[idx][5], 255, lev, 0, color2);
      }
      color[0] = color[0] + color2[0];
      if (color[0] > 255) {
        color[0] = 255;
      }
      color[1] = color[1] + color2[1];
      if (color[1] > 255) {
        color[1] = 255;
      }
      color[2] = color[2] + color2[2];
      if (color[2] > 255) {
        color[2] = 255;
      }
    }
    else {
      if (finalLevel1 > 127) {
        int satur = (finalLevel1 - 127) * 2;
        if (satur > 255) {
          satur = 255;
        }
        HSVtoRGB(fxIntVars[idx][4], 255 - satur, 255, 0, color);
      }
      else {
        int lev = finalLevel1 * 2;
        if (lev > 255) {
          lev = 255;
        }
        HSVtoRGB(fxIntVars[idx][4], 255, lev, 0, color);
      }
    }
    fakeLEDsFB[idx][i] = CRGB(color[0], color[1], color[2]);
  }
}

void HeatRamp(int hotness, int color[3]) {
  int thisThird = hotness % 85;
  thisThird = thisThird * 3;
  if (thisThird > 255) {
    thisThird = 255;
  }

  if (hotness >= 170) {
    color[0] = 255;
    color[1] = 255;
    color[2] = thisThird;
  }
  else if ((hotness < 170) && (hotness > 85)) {
    color[0] = 255;
    color[1] = thisThird;
    color[2] = 0;
  }
  else {
    color[0] = thisThird;
    color[1] = 0;
    color[2] = 0;
  }
}

// CHASERS

void ChaseSeqRotateAnalogic45(int sequence, int startColor, int color[3]) {
  int positionP1, positionP2, positionP3;
  positionP1 = (startColor + sequence) % 1536;
  positionP2 = (startColor + sequence + 192) % 1536; // + 192 = 45 degrees
  positionP3 = (startColor + sequence + 1344) % 1536; // -192 = -45
  // degrees (added
  // 1344 is equiv)
  sequence = sequence % 4;
  if (sequence == 0) {
    HSVtoRGB(positionP2, 255, 255, 0, color); // 45
    // degrees
    // anti-clockwise
  }
  else if ((sequence == 1) || (sequence == 3)) {
    HSVtoRGB(positionP3, 255, 255, 0, color); // Primary
    // colour
  }
  else {
    HSVtoRGB(positionP1, 255, 255, 0, color); // 45
    // degrees
    // clockwise
  }
}

void ChaseSeqRotateAccentedAnalogic30(int sequence, int startColor, int color[3]) {
  int positionP1, positionP2, positionP3, positionC;
  positionP1 = (startColor + sequence) % 1536;
  positionP2 = (startColor + sequence + 128) % 1536; // + 128 = 30 degrees
  positionP3 = (startColor + sequence + 1280) % 1536; // -128 = -30
  // degrees (added
  // 1280 is equiv)
  positionC = (startColor + sequence + 768) % 1536; // + 768 = 180 degrees
  sequence = sequence % 6;
  if (sequence == 0) {
    HSVtoRGB(positionP2, 255, 255, 0, color); // 30
    // degrees
    // anti-clockwise
  }
  else if ((sequence == 1) || (sequence == 5)) {
    HSVtoRGB(positionP1, 255, 255, 0, color); // Primary
    // colour
  }
  else if ((sequence == 2) || (sequence == 4)) {
    HSVtoRGB(positionP3, 255, 255, 0, color); // Primary
    // colour
  }
  else {
    HSVtoRGB(positionC, 255, 255, 0, color); // 30 degrees
    // clockwise
  }
}

void ChaseSeqRotateCompliment(int sequence, int startColor, int color[3]) {
  int positionP, positionC;
  positionP = (startColor + sequence) % 1536;
  positionC = (startColor + sequence + 768) % 1536; // + 768 = 180 degrees
  sequence = sequence % 5;
  if (sequence == 0) {
    HSVtoRGB(positionC, 255, 255, 0, color); // Complimetary
  }
  else {
    HSVtoRGB(positionP, 255, 255, 0, color); // Primary
  }
}

void ChaseSeqRGB(int sequence, int startColor, int color[3]) {
  sequence = sequence % 3;
  if (sequence == 0) {
    HSVtoRGB(0, 255, 255, 0, color); //red
  }
  else if (sequence == 1) {
    HSVtoRGB(512, 255, 255, 0, color); //green
  }
  else {
    HSVtoRGB(1024, 255, 255, 0, color); //blue
  }
}

void ChaseSeqRWB(int sequence, int startColor, int color[3]) {
  sequence = sequence % 3;
  if (sequence == 0) {
    HSVtoRGB(0, 255, 255, 0, color); //red
  }
  else if (sequence == 1) {
    HSVtoRGB(0, 0, 255, 0, color); //white
  }
  else {
    HSVtoRGB(1024, 255, 255, 0, color); //blue
  }
}

void ChaseSeqGWR(int sequence, int startColor, int color[3]) {
  sequence = sequence % 3;
  if (sequence == 0) {
    HSVtoRGB(512, 255, 255, 0, color); //green
  }
  else if (sequence == 1) {
    HSVtoRGB(0, 0, 255, 0, color); //white
  }
  else {
    HSVtoRGB(0, 255, 255, 0, color); //red
  }
}

void ChaseSeqCustom(int sequence, int startColor, int color[3]) {
  sequence = sequence % 2;
  if (sequence == 0) {
    //return (ColorConversion.HSVtoRGB(0, 0, 0, 0)); //black
    HSVtoRGB(0, 0, 0, 0, color); //black
  }
  else {
    HSVtoRGB(startColor, 255, 255, 0, color); //custom
  }
}

void ChaseSeqFadeTrail9Bright(int sequence, int startColor, int color[3]) {
  int positionC;

  positionC = (startColor + sequence) % 1536;
  sequence = sequence % 9;
  HSVtoRGB(positionC, 255, GetSmoothFade27(sequence), 0, color);
}

void ChaseSeqFadeTrail9Custom(int sequence, int startColor, int color[3]) {
  sequence = sequence % 9;
  HSVtoRGB(startColor, 255, GetSmoothFade27(sequence), 0, color);
}

void ChaseSeqFadeTrail9RBBright(int sequence, int startColor, int color[3]) {
  int positionC;
  positionC = 0; // red

  int col = sequence % 18;
  if (col < 9) {
    positionC = 1024; // blue
  }
  sequence = sequence % 9;
  HSVtoRGB(positionC, 255, GetSmoothFade27(sequence), 0, color);
}

void ChaseSeqFadeTrail9RGBright(int sequence, int startColor, int color[3]) {
  int positionC;
  positionC = 0; // red

  int col = sequence % 18;
  if (col < 9) {
    positionC = 512; // green
  }
  sequence = sequence % 9;
  HSVtoRGB(positionC, 255, GetSmoothFade27(sequence), 0, color);
}

void runSelectedPattern(byte idx) {
  switch (patternNo[idx]) {
  case 0:
    ProgramOff(idx);
    break;
  case 1:
    Program01SimplexNoise(idx);
    break;
  case 2:
    Program02LineFade(idx);
    break;
  case 3:
    Program03Chaser(idx);
    break;
  case 4:
    Program04Rainbow(idx);
    break;
  case 5:
    Program05Wave(idx);
    break;
  case 6:
    Program06Phasing(idx);
    break;
  case 7:
    Program07Plasma(idx); ////
    break;
  case 8:
    Program08StrobeFade(idx);
    break;
  case 9:
    Program09SplashFade(idx);
    break;
  case 10:
    Program10SlowFill(idx);
    break;
  case 11:
    Program11LarsonScanners(idx);
    break;
  case 12:
    Program12SolidColorPulse(idx);
    break;
  case 13:
    Program13RandomStrobe(idx);
    break;
  case 14:
    Program14LarsonHeartbeat(idx);
    break;
  case 15:
    Program15Flames(idx);
    break;
  case 16:
    Program16ComplimentaryFade(idx);
    break;
  case 17:
    Program17RandomLevels(idx);
    break;
  case 18:
    Program18GentleRandomFade(idx);
    break;
  case 19:
    Program19RandomCompLevels(idx);
    break;
  case 20:
    Program20BrightPulse(idx);
    break;
  case 21:
    Program21Stipple(idx);
    break;
  case 22:
    Program22DoubleRainbow(idx); ////flickers
    break;
  case 23:
    Program23FlagUS(idx);
    break;
  case 24:
    Program25SolidColor(idx);
    break;
  case 25:
    Program26Bonfire(idx);
    break;
  case 26:
    Program27Sparks(idx);
    break;
  case 27:
    //Program00SoftWarmTwinkles(idx);
    Program00TwinkleFox2015(idx);
    break;
  case 28: //always bump this up when put in new patterns
    Program24CreepyHalloween(idx); ////
    break;
  }
}

// skip patterns based of preferences

void skipPattern(int idx) {
  /*  int lastProg = patternNo[idx];

  if (meoprefPatternsList.isEmpty()) {
  // if none are checked, then use solid color only
  patternNo[idx] = 0;
  } else {
  // go up or down patterns depending on 'control'
  if (!meoprefPatternsList.contains(Integer.toString(patternNo[idx]))) {
  if (control == 0) {
  patternNo[idx] = patternNo[idx] + 1;
  } else if (control == 1) {
  patternNo[idx] = patternNo[idx] - 1;
  }
  }

  // check boundaries, and put back on track
  if ((patternNo[idx] == NUM_PATTERNS) && (control == 0)) {
  patternNo[idx] = 0;
  } else if ((patternNo[idx] == NUM_PATTERNS) && (control == 1)) {
  patternNo[idx] = NUM_PATTERNS - 1;
  }

  // keep running this function until settles down - i.e. doesn't
  // change program any more
  if (patternNo[idx] != lastProg) {
  skipPattern(idx);
  }
  } */
}

void SelectPattern(int frontImgIdx) {
  if (stayFrames == longFrame) {
    patternNo[frontImgIdx]++;
    if (patternNo[frontImgIdx] == NUM_PATTERNS) {
      patternNo[frontImgIdx] = 0;
    }
  }
  else {
    if (patternSelected) {
      patternNo[frontImgIdx] = nextSelectedPattern; //warm twinkle
      lastSelectedPattern = nextSelectedPattern;
      patternSelected = false;
      stayFrames == longFrame;
    }
  }
}

void runSelectedCrossfade() {
  //switch (xFadeNo) {
  //    case 0:
  crossfadeSimple(); //meoprefNumPixels);
  //        break;
  //    case 1:
  //        crossfadeSideToSideWipe(); //meoprefNumPixels);
  //        break;
  //}
}

// skip crossfades based of preferences

void skipCrossfade() {
  /*  int lastProg = xFadeNo;

  // if none are checked, then use simple fade only
  if (meoprefCrossfadesList.isEmpty()) {
  xFadeNo = 0;
  } else {
  if (!meoprefCrossfadesList.contains(Integer.toString(xFadeNo))) {
  xFadeNo = xFadeNo + 1;
  }

  if (xFadeNo == NUM_CROSSFADES) {
  xFadeNo = 0;
  }

  // keep running this function until settles down - i.e. doens't
  // change crossfade any more
  if (xFadeNo != lastProg) {
  skipCrossfade();
  }
  } */
}

void setVariation(int patternNo, int maxValue, int idx) {
  if (SETUP_VAR_NOTRANDOM) {
    // Sequential
    // increment to next
    if (varChangeUp) {
      fxNextVariationNo[patternNo]++;
      if (fxNextVariationNo[patternNo] == maxValue) {
        fxNextVariationNo[patternNo] = 0;
      }
    }
    else {
      fxNextVariationNo[patternNo]--;
      if (fxNextVariationNo[patternNo] == -1) {
        fxNextVariationNo[patternNo] = maxValue - 1;
      }
    }
  }
  else {
    // Random
    fxNextVariationNo[patternNo] = random(maxValue);
  }
  fxCurrentVariationNo[idx] = fxNextVariationNo[patternNo];
}

// ---------------------------------------------------------------------------
// Alpha channel effect rendering functions.  Like the image rendering
// effects, these are typically parametrically-generated...but unlike the
// images, there is only one alpha renderer "in flight" at any given time.
// So it would be okay to use local static variables for storing state
// information...but, given that there could end up being many more render
// functions here, and not wanting to use up all the RAM for static vars
// for each, a third row of fxIntVars is used for this information.

// Simplest alpha effect: fade entire pixelString over duration of transition.
// REMEMER - used to run from 0.0 to 1.0. Now is 255 to 0
// So if re implement other cross fades, then rework like this
void crossfadeSimple(void) {
  if (fxIntVars[2][0] == 0) {
    fxIntVars[2][0] = 1; // Transition initialized
  }
  float fade = (float)tCounter / (float)transitionTime;
  for (int i = 0; i < NUM_LEDS; i++) {
    fadeMask[i] = 255 - (int)(fade * 255.0);
  }
}

/*
void crossfadeSideToSideWipe() {
int x, y, b;
if (fxIntVars[2][0] == 0) {
fxIntVars[2][1] = 63; // random(1, NUM_LEDS); // run, in pixels
fxIntVars[2][2] = (random(2) == 1) ? 255 : -255; // direction

// Log.i("MEOTag", "Initialised Fade: Side to Side");
fxIntVars[2][0] = 1; // Transition initialized
}

if (fxIntVars[2][2] > 0) {
b = (255 + (NUM_LEDS * fxIntVars[2][2] / fxIntVars[2][1])) * tCounter / transitionTime - (NUM_LEDS * fxIntVars[2][2] / fxIntVars[2][1]);
} else {
b = (255 - (NUM_LEDS * fxIntVars[2][2] / fxIntVars[2][1])) * tCounter / transitionTime;
}

for (x = 0; x < NUM_LEDS; x++) {
y = x * fxIntVars[2][2] / fxIntVars[2][1] + b;
if (y < 0)
fadeMask[x] = 0;
else if (y >= 255)
fadeMask[x] = 1;
else
fadeMask[x] = y / 256;
}
}*/

// Fixed-point colorspace conversion: HSV (hue-saturation-value) to RGB.
// This is a bit like the 'Wheel' function from the original strandtest
// code on steroids. The angular units for the hue parameter may seem a
// bit odd: there are 1536 increments around the full color wheel here --
// not degrees, radians, gradians or any other conventional unit I'm
// aware of. These units make the conversion code simpler/faster, because
// the wheel can be divided into six sections of 256 values each, very
// easy to handle on an 8-bit microcontroller. Math is math, and the
// rendering code elsewhere in this file was written to be aware of these
// units. Saturation and value (brightness) range from 0 to 255.
// MEO: wheelLine 0: Full wheel RGB; 1: RG; 2: GB; 3: BR
void HSVtoRGB(int h, int s, int v, int wheelLine, int color[3]) {
  int r, g, b, lo;
  int s1;
  float vOne;

  // Hue
  switch (wheelLine) {
  case 0: // Full RGB Wheel (pburgess original function)
    h %= 1536; // -1535 to +1535
    if (h < 0) h += 1536; //     0 to +1535
    lo = h & 255; // Low byte  = primary/secondary color mix
    switch (h >> 8) { // High byte = sextant of colorwheel
    case 0:
      r = 255;
      g = lo;
      b = 0;
      break; // R to Y
    case 1:
      r = 255 - lo;
      g = 255;
      b = 0;
      break; // Y to G
    case 2:
      r = 0;
      g = 255;
      b = lo;
      break; // G to C
    case 3:
      r = 0;
      g = 255 - lo;
      b = 255;
      break; // C to B
    case 4:
      r = lo;
      g = 0;
      b = 255;
      break; // B to M
    default:
      r = 255;
      g = 0;
      b = 255 - lo;
      break; // M to R
    }
    break;
  case 1: //RG Line only
    h %= 1024;
    if (h < 0) h += 1024;
    lo = h & 255; // Low byte  = primary/secondary color mix
    switch (h >> 8) { // High byte = sextant of colorwheel
    case 0:
      r = 255;
      g = lo;
      b = 0;
      break; // R to Y
    case 1:
      r = 255 - lo;
      g = 255;
      b = 0;
      break; // Y to G
    case 2:
      r = lo;
      g = 255;
      b = 0;
      break; // G to Y
    default:
      r = 255;
      g = 255 - lo;
      b = 0;
      break; // Y to R
    }
    break;
  case 2: //GB Line only
    h %= 1024;
    if (h < 0) h += 1024;
    lo = h & 255; // Low byte  = primary/secondary color mix
    switch (h >> 8) { // High byte = sextant of colorwheel
    case 0:
      r = 0;
      g = 255;
      b = lo;
      break; // G to C
    case 1:
      r = 0;
      g = 255 - lo;
      b = 255;
      break; // C to B
    case 2:
      r = 0;
      g = lo;
      b = 255;
      break; // B to C
    default:
      r = 0;
      g = 255;
      b = 255 - lo;
      break; // C to G
    }
    break;
  case 3: //BR Line only
    h %= 1024;
    if (h < 0) h += 1024;
    lo = h & 255; // Low byte  = primary/secondary color mix
    switch (h >> 8) { // High byte = sextant of colorwheel
    case 0:
      r = lo;
      g = 0;
      b = 255;
      break; // B to M
    case 1:
      r = 255;
      g = 0;
      b = 255 - lo;
      break; // M to R
    case 2:
      r = 255;
      g = 0;
      b = lo;
      break; // R to M
    default:
      r = 255 - lo;
      g = 0;
      b = 255;
      break; // M to B
    }
    break;
  }

  // Saturation: add 1 so range is 1 to 256, allowig a quick shift operation
  // on the result rather than a costly divide, while the type upgrade to int
  // avoids repeated type conversions in both directions.
  s1 = s + 1;
  r = 255 - (((255 - r) * s1) >> 8);
  g = 255 - (((255 - g) * s1) >> 8);
  b = 255 - (((255 - b) * s1) >> 8);

  // Value (brightness) and 24-bit color concat merged: similar to above, add
  // 1 to allow shifts, and upgrade to int makes other conversions implicit.
  vOne = (v + 1.0) / 256.0;

  color[0] = (int)(r * vOne);
  color[1] = (int)(g * vOne);
  color[2] = (int)(b * vOne);
}

// compliment to HSVtoRGB

void RGBtoHSV(int r, int g, int b, int hsv[3]) {
  float tempR, tempG, tempB;

  tempR = r / 255.0;
  tempG = g / 255.0;
  tempB = b / 255.0;

  float minRGB = min(tempR, min(tempG, tempB));
  float maxRGB = max(tempR, max(tempG, tempB));

  // Black-gray-white
  if (minRGB == maxRGB) {
    hsv[0] = 0; // irrelevant, as saturation is nothing
    hsv[1] = 0;
    hsv[2] = (int)(255.0 * maxRGB);
  }
  else {
    // Colors other than black-gray-white:
    float d = (tempR == minRGB) ? tempG - tempB : ((tempB == minRGB) ? tempR - tempG : tempB - tempR);
    float h = (tempR == minRGB) ? 3.0 : ((tempB == minRGB) ? 1.0 : 5.0);
    hsv[0] = (int)(256.0 * (h - d / (maxRGB - minRGB)));
    hsv[1] = (int)(255.0 * (maxRGB - minRGB) / maxRGB);
    hsv[2] = (int)(255.0 * maxRGB);
  }
}

//sets to single hue, but keeps level and saturation - NOTE could just use same as in DecodeCustomColor hue part with max saturation and min level
void HueOnly(int color[3], int shift) {
  int hsv[3];
  RGBtoHSV(color[0], color[1], color[2], hsv);
  HSVtoRGB(shift, hsv[1], hsv[2], 0, color);
}

void DecodeCustomColour(int idx) {
  fxCustomColour[idx][0] = customColour1_R; //red
  fxCustomColour[idx][1] = customColour1_G; //green
  fxCustomColour[idx][2] = customColour1_B; //blue
  //customAlpha = (meoprefColor>>24)&0xFF;
  int myHSVColor[3];
  RGBtoHSV(fxCustomColour[idx][0], fxCustomColour[idx][1], fxCustomColour[idx][2], myHSVColor);
  fxCustomColour[idx][3] = myHSVColor[0]; //hue

  fxCustomColour[idx][4] = customColour2_R; //red
  fxCustomColour[idx][5] = customColour2_G; //green
  fxCustomColour[idx][6] = customColour2_B; //blue
  RGBtoHSV(fxCustomColour[idx][4], fxCustomColour[idx][5], fxCustomColour[idx][6], myHSVColor);
  fxCustomColour[idx][7] = myHSVColor[0]; //hue
}

// returns a value in the range of about [-0.347 .. 0.347]
// Simplex noise support functions:
// From an original algorithm by Ken Perlin.
// Returns a value in the range of about [-0.347 .. 0.347]

float SNSimplexNoise(float x, float y, float z) {
  // Skew input space to relative coordinate in simplex cell
  ss = (x + y + z) * ONE_THIRD;
  ii = SNfastfloor(x + ss);
  jj = SNfastfloor(y + ss);
  kk = SNfastfloor(z + ss);

  // Unskew cell origin back to (x, y , z) space
  ss = (ii + jj + kk) * ONE_SIXTH;
  uu = x - ii + ss;
  vv = y - jj + ss;
  ww = z - kk + ss;

  AA[0] = AA[1] = AA[2] = 0;

  // For 3D case, the simplex shape is a slightly irregular tetrahedron.
  // Determine which simplex we're in
  int hi = uu >= ww ? uu >= vv ? 0 : 1 :
    vv >= ww ? 1 : 2;
  int lo = uu < ww ? uu < vv ? 0 : 1 :
    vv < ww ? 1 : 2;

  return SNk_fn(hi) + SNk_fn(3 - hi - lo) + SNk_fn(lo) + SNk_fn(0);
}

int SNfastfloor(float n) {
  return n > 0 ? (int)n : (int)n - 1;
}

float SNk_fn(int a) {
  ss = (AA[0] + AA[1] + AA[2]) * ONE_SIXTH;
  float x = uu - AA[0] + ss;
  float y = vv - AA[1] + ss;
  float z = ww - AA[2] + ss;
  float t = 0.6f - x * x - y * y - z * z;
  int h = SNshuffle(ii + AA[0], jj + AA[1], kk + AA[2]);
  AA[a]++;
  if (t < 0) return 0;
  int b5 = h >> 5 & 1;
  int b4 = h >> 4 & 1;
  int b3 = h >> 3 & 1;
  int b2 = h >> 2 & 1;
  int b = h & 3;
  float p = b == 1 ? x : b == 2 ? y : z;
  float q = b == 1 ? y : b == 2 ? z : x;
  float r = b == 1 ? z : b == 2 ? x : y;
  p = b5 == b3 ? -p : p;
  q = b5 == b4 ? -q : q;
  r = b5 != (b4^b3) ? -r : r;
  t *= t;
  return 8 * t * t * (p + (b == 0 ? q + r : b2 == 0 ? q : r));
}

int SNshuffle(int i, int j, int k) {
  return SNb(i, j, k, 0) + SNb(j, k, i, 1) + SNb(k, i, j, 2) + SNb(i, j, k, 3) + SNb(j, k, i, 4) + SNb(k, i, j, 5) + SNb(i, j, k, 6) + SNb(j, k, i, 7);
}

int SNb(int i, int j, int k, int B) {
  return TT[SNb(i, B) << 2 | SNb(j, B) << 1 | SNb(k, B)];
}

int SNb(int N, int B) {
  return N >> B & 1;
}

int fadeTable27[27] = { 255, 223, 191, 159, 127, 111, 95, 79, 63, 55, 47, 39, 31, 27, 23, 19, 15, 13, 11, 9, 7, 6, 5, 4, 3, 2, 1 };

int GetSmoothFade27(int x) {
  if ((x < 27) && (x >= 0)) {
    return fadeTable27[x];
  }
  else {
    return 0;
  }
}

int GetSplash(int bulbOut, int frameOut, int bulbStart, int frameStart, float velocity) {
  // float amplitudeOut; // the return value before adjustment
  // int frameRelative; // relative time, i.e. time into the effect
  float distance; // distance effect has travelled on one side

  int frame = frameOut - frameStart;

  distance = velocity * frame;

  float level = 0.0;
  if (frame < 100) { // 100 is size of damping table
    // amplitudeOut = pow(damping, (float)frame); //pow(x,y) is really
    // really slow!
    // level = ((float)MAX_LEVEL * amplitudeOut);
    level = (MAX_LEVEL * dampingTable[frame]);
  }

  int finalOutput = 0;
  if ((bulbOut >= (bulbStart - distance)) && (bulbOut <= (bulbStart + distance))) {
    if (level > 1.0) {
      finalOutput = (int)(level - 1); // adjust to zero-based
    }
  }
  return finalOutput;
}

int GetSimpleOscillatePos(int t, int a, int b, int numberPixels) {
  int s = 0, n;
  n = t / (a + b);

  if ((n * (a + b) <= t) && (t <= ((n + 1) * a) + (n * b))) {
    s = t - (2 * n * b);
  }
  else if ((((n + 1) * a) + (n * b) <= t) && (t <= (n + 1) * (a + b))) {
    s = (2 * (n + 1) * a) - t;
  }
  return (s % numberPixels);
}

//  This function takes a time in pseudo-milliseconds,
//  figures out brightness = f( time ), and also hue = f( time )
//  The 'low digits' of the millisecond time are used as
//  input to the brightness wave function.
//  The 'high digits' are used to select a color, so that the color
//  does not change over the course of the fade-in, fade-out
//  of one cycle of the brightness wave function.
//  The 'high digits' are also used to determine whether this pixel
//  should light at all during this cycle, based on the TWINKLE_DENSITY.
//TODO: use variation to select palette, and remove auto pallete change
CRGB computeOneTwinkle(uint32_t ms, uint8_t salt, int idx, int variation)
{
  uint16_t ticks = ms >> (8 - myParam2);
  uint8_t fastcycle8 = ticks;
  uint16_t slowcycle16 = (ticks >> 8) + salt;
  slowcycle16 += sin8(slowcycle16);
  slowcycle16 = (slowcycle16 * 2053) + 1384;
  uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);

  uint8_t bright = 0;
  if (((slowcycle8 & 0x0E) / 2) < myParam1) {
    bright = attackDecayWave8(fastcycle8);
  }

  uint8_t hue = slowcycle8 - salt;
  CRGB c;

  int custCol;
  custCol = customColour1_B + (customColour1_G * 256) + (customColour1_R * 65536);

  TProgmemRGBPalette16 Custom_p FL_PROGMEM =
  { custCol, custCol, custCol, custCol,
  custCol, custCol, custCol, custCol,
  custCol, custCol, custCol, custCol,
  custCol, custCol, custCol, custCol };

  if (bright > 0) {
    if (useCustomColour) {
      //Serial.print(custCol); Serial.print(" - ");Serial.println(CRGB::Red);   
      c = ColorFromPalette(Custom_p, hue, bright, NOBLEND);
    }
    else {
      c = ColorFromPalette(gCurrentPalette, hue, bright, NOBLEND);
    }

    if (COOL_LIKE_INCANDESCENT == 1) {
      coolLikeIncandescent(c, fastcycle8);
    }
  }
  else {
    c = CRGB::Black;
  }

  return c;
}


// This function is like 'triwave8', which produces a
// symmetrical up-and-down triangle sawtooth waveform, except that this
// function produces a triangle wave with a faster attack and a slower decay:
//
//     / \
//    /     \
//   /         \
//  /             \
//

uint8_t attackDecayWave8(uint8_t i)
{
  if (i < 86) {
    return i * 3;
  }
  else {
    i -= 86;
    return 255 - (i + (i / 2));
  }
}

// This function takes a pixel, and if its in the 'fading down'
// part of the cycle, it adjusts the color a little bit like the
// way that incandescent bulbs fade toward 'red' as they dim.
void coolLikeIncandescent(CRGB& c, uint8_t phase)
{
  if (phase < 128) return;

  uint8_t cooling = (phase - 128) >> 4;
  c.g = qsub8(c.g, cooling);
  c.b = qsub8(c.b, cooling * 2);
}

// Advance to the next color palette in the list (above).
void chooseNextColorPalette(CRGBPalette16& pal)
{
  const uint8_t numberOfPalettes = sizeof(ActivePaletteList) / sizeof(ActivePaletteList[0]);
  static uint8_t whichPalette = -1;
  whichPalette = addmod8(whichPalette, 1, numberOfPalettes);

  pal = *(ActivePaletteList[whichPalette]);
}
