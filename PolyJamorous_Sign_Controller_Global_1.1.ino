#include <FastLED.h>
#include <avr/wdt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// DEFINE GLOBALS //////////////////////////////////////////////
//////////////////////////////////////////////// ///////////////////////////////////////////////////

#define DATA_PIN     2
#define NUM_LEDS    62  //each "LED" is actually 3 physical LEDs or one segment
#define BRIGHTNESS  255//Max is 255
#define LED_TYPE    WS2811
#define COLOR_ORDER BRG
#define FRAMES_PER_SECOND 120
CRGBPalette16 palettes[] = {RainbowColors_p, LavaColors_p, OceanColors_p, ForestColors_p, PartyColors_p, HeatColors_p};
uint8_t rando = 0; //use as a semi-random number
uint8_t randoDelay = 10;//ms between rando shift
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint8_t gHueFunc = 0; // same as above but only incremented within functions
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gCurrentLetter = 0;
uint8_t gCurrentPaletteTop = 0;
uint8_t gCurrentPaletteBottom = 0;
uint8_t gCurrentShiftBottom = 150;
uint8_t gPaletteTopTime = 9;//seconds
uint8_t gPaletteBottomTime = 9;//seconds
uint8_t gPaletteIndex = 0; //index for the color palettes to continue cycling through
uint8_t gPaletteIndexFunc = 0; //same as above but only modified within functions
uint8_t gCCW = 10;
uint8_t gCW = 10;
uint8_t gCounter = 0;
uint8_t gCounterFunc = 0;

/////////////// MASTER BPM /////////////////////
uint8_t BPM = 120; // beats per minute for functions that use BPM
////////////////////////////////////////////////

CRGB rawLeds[NUM_LEDS];
CRGBSet leds(rawLeds, NUM_LEDS);


struct Letter
{
  CRGBSet top;
  CRGBSet bottom;
};

uint8_t Top[28] =    {60,61,0 ,1 , 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,23}; //checked
uint8_t Bottom[31] = {55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,42,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29}; //checked
uint8_t Left[] = {59,58,57,56}; //checked
uint8_t Right[] = {25,26,27,28}; //checked

Letter P = {leds(60,61),leds(54,55)};//Checked
Letter O1 = {leds(0,1),leds(52,53)};//Checked
Letter L = {leds(2,3),leds(50,51)};//Checked
Letter Y = {leds(4,5),leds(48,49)};//Checked
Letter J = {leds(6,7),leds(46,47)};//Checked
Letter A = {leds(9,10),leds(43,44)};//Checked
Letter M = {leds(11,13),leds(40,42)};//Checked
Letter O2 = {leds(14,15),leds(38,39)};//Checked
Letter R = {leds(16,17),leds(36,37)};//Checked
Letter O3 = {leds(19,19),leds(34,34)};//Checked
Letter U = {leds(21,22),leds(31,32)};//Checked
Letter S = {leds(23,24),leds(29,30)};//Checked

Letter PolyJamorous[12] = {P,O1,L,Y,J,A,M,O2,R,O3,U,S};



////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// FUNCTIONS /////////////////////////////////////////////////
//////////////////////////////////////////////// ///////////////////////////////////////////////////

// all color doLetterPalette(PolyJamorous[i], palettes[gCurrentPaletteTop], palettes[gCurrentPaletteBottom] ): CloudColors_p LavaColors_p OceanColors_p ForestColors_p RainbowColors_p RainbowStripeColors_p PartyColors_p HeatColors_p


void doLetterPalette(struct Letter letter, CRGBPalette16 paletteTop, CRGBPalette16 paletteBottom, uint8_t index)
{
  for (uint8_t x=0; x<sizeof(letter.top); x++){
    for (uint8_t y=0; y<sizeof(letter.bottom); y++){
      letter.top = ColorFromPalette(paletteTop, gPaletteIndex, 255); //palette, index, brightness
      letter.bottom = ColorFromPalette(paletteBottom, gPaletteIndex + gCurrentShiftBottom, 255);
    }
  }
}

void polyRhythm(uint8_t numer, uint8_t numerLeds[], uint8_t numerLength, uint8_t numerHue, uint8_t denom, uint8_t denomLeds[], uint8_t denomLength, uint8_t denomHue)
{
  fadeToBlackBy(leds, NUM_LEDS, 10);
  //Numerator
  EVERY_N_MILLISECONDS ( 60000 / (BPM / numer) / 2 ) 
  {
    for(uint8_t i=0 ; i < numerLength ; i++)
    {
    leds[ numerLeds[i] ] = CHSV (numerHue, 255, 255); // ColorFromPalette(numerPalette, gPaletteIndex, 255);
    }
  }
  //Denominator
  EVERY_N_MILLISECONDS ( 60000/(BPM / denom) / 2) 
  {
    for(uint8_t i=0 ; i < denomLength ; i++)
    {
    leds[ denomLeds[i] ] =  CHSV (denomHue, 255, 255); // ColorFromPalette(denomPalette,  gPaletteIndex, 255); 
    }
  }
}

void polyRhythm_2_3()
{
  polyRhythm (2, Top,sizeof(Top), 171, 3, Bottom, sizeof(Bottom), 0);
}

void polyRhythm_3_4()
{
  polyRhythm (3, Top,sizeof(Top), 85, 4, Bottom, sizeof(Bottom), 200);
}

void POLYJAMOROUS_SIMPLE()
{
      leds.fadeToBlackBy(1);
      doLetterPalette(PolyJamorous[gCurrentLetter], palettes[gCurrentPaletteTop], palettes[gCurrentPaletteBottom],gPaletteIndex);
      doLetterPalette(PolyJamorous[ (gCurrentLetter+6) % 12 ], palettes[gCurrentPaletteTop], palettes[gCurrentPaletteBottom],gPaletteIndex);
      EVERY_N_MILLISECONDS (60000/BPM) {gCurrentLetter = ( gCurrentLetter+1 ) % ( sizeof(PolyJamorous) / sizeof(PolyJamorous[0]));}
}

void POLYJAMOROUS_CHANT()
{

  for(uint8_t i = 0 ; i < 4 ; i++)
  {
    FastLED.clear();
    FastLED.show(); 
    doLetterPalette(PolyJamorous[i], palettes[gCurrentPaletteTop], palettes[gCurrentPaletteBottom] ,gPaletteIndex);
    FastLED.delay(60000/BPM); // quarter notes
  }
  
  for(uint8_t i = 4 ; i < 12 ; i++)
  {
    FastLED.clear();
       FastLED.show(); 
    doLetterPalette(PolyJamorous[i], palettes[gCurrentPaletteTop], palettes[gCurrentPaletteBottom] ,gPaletteIndex);
    FastLED.delay((60000/BPM)/2); // 8th notes
  }
}

void POLY_JAM_R_US_2()
{
    uint8_t sine = beatsin16( 50, 0, 5 ); //BEAT (bpm), MIN, MAX
    uint8_t index = gPaletteIndexFunc + sine;
    leds.fadeToBlackBy(10);
    EVERY_N_MILLISECONDS (60000/BPM) {gCurrentLetter = ( gCurrentLetter+1 ) % 4;}
    switch (gCurrentLetter) {
    case 0: //POLY
    doLetterPalette(P, palettes[gCurrentPaletteTop], palettes[gCurrentPaletteBottom],index);
    doLetterPalette(O1, palettes[gCurrentPaletteTop], palettes[gCurrentPaletteBottom],index);
    doLetterPalette(L, palettes[gCurrentPaletteTop], palettes[gCurrentPaletteBottom],index);
    doLetterPalette(Y, palettes[gCurrentPaletteTop], palettes[gCurrentPaletteBottom],index);
    break;
    
    case 1: //JAM
    doLetterPalette(J, palettes[gCurrentPaletteTop], palettes[gCurrentPaletteBottom],index);
    doLetterPalette(A, palettes[gCurrentPaletteTop], palettes[gCurrentPaletteBottom],index);
    doLetterPalette(M, palettes[gCurrentPaletteTop], palettes[gCurrentPaletteBottom],index);
    break;

    case 2: //R
    doLetterPalette(R, palettes[gCurrentPaletteTop], palettes[gCurrentPaletteBottom],index);
    break;

    case 3: //US
    doLetterPalette(U, palettes[gCurrentPaletteTop], palettes[gCurrentPaletteBottom],index);
    doLetterPalette(S, palettes[gCurrentPaletteTop], palettes[gCurrentPaletteBottom],index);
    break;

    default: //if gCurrentLetter isn't any of these cases, set it to 0
    gCurrentLetter = 0;
    break;
    }
}

void RainbowCircle()
{   
  for(uint8_t i=0 ; i<NUM_LEDS ; i++)
  {
      fadeToBlackBy(leds, NUM_LEDS, 10);
      leds[i] = CHSV(rando,255,255);
      FastLED.delay(30);
      rando = rando +5; 
  }
}

void RainbowCircleDouble()
{
    fadeToBlackBy(leds, NUM_LEDS, 10);
    leds[gCCW] = CHSV(gHue*6+150,255,255);
    leds[gCW] = CHSV(gHue*6,255,255);
    if (gCCW == 0)
    {
      gCCW = 61;
    }
    else
    {
        gCCW--; 
    }

    if (gCW == 61)
    {
      gCW = 0;
    }
    else
    {
        gCW++; 
    }
    FastLED.delay(30);
}

void matrix()
{
  uint8_t currentLetter = random(0,11);
  EVERY_N_MILLISECONDS (400){currentLetter = random(0,11);}
  leds.fadeToBlackBy(10);
  doLetterPalette(PolyJamorous[currentLetter], ForestColors_p, ForestColors_p, gPaletteIndex);
}

void sineCircle() //TODO remove function
{
  uint8_t pos = beatsin16( 50, -2, 1 ); //BEAT (bpm), MIN, MAX
  fadeToBlackBy(leds, NUM_LEDS, 10);
  leds[(gCounterFunc+pos) % NUM_LEDS] = CHSV(gHueFunc,255,255);
  FastLED.delay(60);
  gHueFunc++;
  gCounterFunc++;
}

void sinePalette()
{
  fadeToBlackBy(leds, NUM_LEDS, 20);
  uint8_t sine = beatsin16(BPM, 0, 10);
  uint8_t index = gPaletteIndexFunc + sine;
  leds(0,NUM_LEDS) = ColorFromPalette(palettes [gCurrentPaletteTop], index, 255);
  gPaletteIndexFunc = gPaletteIndexFunc + 2;
  leds[ random16(NUM_LEDS) ] += ColorFromPalette(palettes [gCurrentPaletteTop], random16(255), 255);
}

void Bounce()
{
  uint8_t pos = beatsin16( BPM/2, 0, sizeof(Top) -1 );
  leds.fadeToBlackBy(10);
  leds[ Top[pos] ] += CHSV(gHue*10,255,255);

  uint8_t posBottom = beatsin16( BPM/2, 0, sizeof(Bottom) -1 );
  leds[ Bottom[pos] ] += CHSV(gHue*10,255,255);
}

void ZoomIn()
{
    fadeToBlackBy(leds, NUM_LEDS, 25);
    uint8_t center = 15;
    uint8_t topCenter = sizeof(Top)/2 -1;
    uint8_t bottomCenter = sizeof(Bottom)/2 + 2;
    uint8_t width = 12;

    //light up LEDS with colors
    leds[ Top[topCenter + gCounter] ] = CHSV(gHueFunc,255,255);
    leds[ Bottom[bottomCenter + gCounter] ] = CHSV(gHueFunc,255,255);
    leds[ Top[topCenter - gCounter -1 ] ]  = CHSV(gHueFunc,255,255);
    leds[ Bottom[bottomCenter - gCounter - 5] ]  = CHSV(gHueFunc,255,255);
  
    //first need to rein in gCounter, should only happen once
    if (gCounter > width)
    {
      gCounter = 0;
    }
    
    //now set the pattern. gCounter is incremented until it is equal to width
    if (gCounter == width)
    {
      gCounter = 0;
      gHueFunc = gHueFunc + 8; //change color drastically between zooms
    }
    else
    {
      gCounter++;
    }
    gHueFunc = gHueFunc + 5; //change color slightly between LEDS
    FastLED.delay(60000/BPM/width);
}

void ResetLEDs() // Sets all LEDs to black for a short time to try to fix the freezing issue
{
  for(int i=0; i<NUM_LEDS; i++){
    leds[i] = CRGB::Black;
  }
  delay(5000); // Wait 5 seconds
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// SETUP ///////////////////////////////////////////////////
//////////////////////////////////////////////// ///////////////////////////////////////////////////
void setup() {
  // put your setup code here, to run once:
  delay(3000); // 3 second delay for recovery
  FastLED.clear();
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  ResetLEDs();
//  wdt_enable(WDTO_2S);
}


////////////////////////////////////// POST-FUNCTION DECLARATIONS //////////////////////////////////
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {polyRhythm_2_3, POLY_JAM_R_US_2, ZoomIn, polyRhythm_3_4, Bounce, matrix,  sinePalette, POLYJAMOROUS_SIMPLE, RainbowCircle, RainbowCircleDouble, ResetLEDs}; //POLYJAMOROUS_SIMPLE

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % (sizeof(gPatterns)/sizeof(gPatterns[0]) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// MAIN LOOP //////////////////////////////////////////////////
//////////////////////////////////////////////// ///////////////////////////////////////////////////
void loop() {
    FastLED.show();
    FastLED.delay(1000/FRAMES_PER_SECOND); 
    EVERY_N_MILLISECONDS( 51 ) { gHue++; }; // slowly cycle the "base color" through the rainbow
    EVERY_N_MILLISECONDS( 2 ) { gPaletteIndex++; }; // slowly cycle the "base color" through the rainbow
    EVERY_N_SECONDS (gPaletteTopTime) {gCurrentPaletteTop = (gCurrentPaletteTop + 1) % (sizeof(palettes) / sizeof(palettes[0]) );}
    EVERY_N_SECONDS (gPaletteBottomTime) {gCurrentPaletteBottom = (gCurrentPaletteBottom + 1) % (sizeof(palettes) / sizeof(palettes[0]) );}
    EVERY_N_SECONDS( 19 ) { nextPattern(); } // change patterns periodically
//    wdt_reset();
    gPatterns[gCurrentPatternNumber]();


//    test top and bottom rows
//    for(uint8_t i=0;i < sizeof(Top);i++)
//    {
//      leds[Top[i]] = CRGB(255,255,255);
//    }

//    for(uint8_t i=0;i < sizeof(Bottom);i++)
//    {
//      leds[Bottom[i]] = CRGB(255,255,255);
//    }
    
}
