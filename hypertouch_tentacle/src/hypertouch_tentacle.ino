#include <SPI.h>
#include <CapacitiveSensor.h>
#include <Wire.h>
#include "Adafruit_DRV2605.h"
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

// touching
#define LEDPIN 2
#define THRESHOLD 50
byte touched = 0;
CapacitiveSensor sensor = CapacitiveSensor(8, 7);

// vibration
Adafruit_DRV2605 vib;
int lastVib = 0;
void vibInit(void) {
  vib.begin();
  vib.selectLibrary(1);
  vib.setMode(DRV2605_MODE_INTTRIG);
  vib.setWaveform(0, 1);  // play effect
  vib.setWaveform(1, 7);
  vib.setWaveform(2, 0);       // end waveform
}

const uint16_t PixelCount = 16; // make sure to set this to the number of pixels in your strip
const uint16_t PixelPin = 4;  // make sure to set this to the correct pin, ignored for Esp8266
const uint16_t AnimCount = 17; // we only need enough animations for the tail and one extra 16/5 = 3*2 = 6 + 1 = 7
const uint16_t PixelFadeDuration = 1000; // third of a second
// one second divide by the number of pixels = loop once a second
const uint16_t NextPixelMoveDuration = 1000 / PixelCount; // how fast we move through the pixels
NeoGamma<NeoGammaTableMethod> colorGamma; // for any fade animations, best to correct gamma
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

struct MyAnimationState
{
  RgbColor StartingColor;
  RgbColor EndingColor;
  uint16_t IndexPixel; // which pixel this animation is effecting
};

NeoPixelAnimator animations(AnimCount); // NeoPixel animation management object
MyAnimationState animationState[AnimCount];
uint16_t frontPixel = 0;  // the front of the loop
RgbColor frontColor;  // the color at the front of the loop

void SetRandomSeed()
{
  uint32_t seed;

  // random works best with a seed that can use 31 bits
  // analogRead on a unconnected pin tends toward less than four bits
  seed = analogRead(0);
  delay(1);

  for (int shifts = 3; shifts < 31; shifts += 3)
  {
    seed ^= analogRead(0) << shifts;
    delay(1);
  }

  // Serial.println(seed);
  randomSeed(seed);
}

void FadeOutAnimUpdate(const AnimationParam& param)
{
  // this gets called for each animation on every time step
  // progress will start at 0.0 and end at 1.0
  // we use the blend function on the RgbColor to mix
  // color based on the progress given to us in the animation
  RgbColor updatedColor = RgbColor::LinearBlend(
                            animationState[param.index].StartingColor,
                            animationState[param.index].EndingColor,
                            param.progress);
  // apply the color to the strip
  strip.SetPixelColor(animationState[param.index].IndexPixel,
                      colorGamma.Correct(updatedColor));
}
int loops = 0;
int prevFront = 0;
int idlePixel = 0;
RgbColor prevColor;
void LoopAnimUpdate(const AnimationParam& param)
{
  // wait for this animation to complete,
  // we are using it as a timer of sorts
  if (param.state == AnimationState_Completed)
    //  if (param.state == AnimationState_Completed && touched || !touched)
  {
    
    // done, time to restart this position tracking animation/timer
    animations.RestartAnimation(param.index);

    // pick the next pixel inline to start animating
    //
    if (frontPixel != prevFront) {
      if (prevFront == 14) loops++;
      prevFront = frontPixel;
    }

    if (loops < 1) {
      frontPixel = (frontPixel + 1) % PixelCount; // increment and wrap

      if (frontPixel == 0)
      {
        // we looped, lets pick a new front color
        frontColor = HslColor(random(30,90) / 360.0f, 1.0f, 0.25f);
      }
    } else {
      frontPixel = -1;
    }

    uint16_t indexAnim;
    // do we have an animation available to use to animate the next front pixel?
    // if you see skipping, then either you are going to fast or need to increase
    // the number of animation channels
    if (animations.NextAvailableAnimation(&indexAnim, 1))
    {
      Serial.println(indexAnim);
      
      if (!touched) {
        int pixel = idlePixel++  % (PixelCount);
        animationState[indexAnim].StartingColor = prevColor;
        animationState[indexAnim].EndingColor = RgbColor(random(60, 90) / (pixel * 0.5), random(60, 90) / (pixel * 0.5), random(60, 90) / (pixel * 0.5));
        prevColor = animationState[indexAnim].EndingColor;
        animationState[indexAnim].IndexPixel = pixel;
      } else {
        idlePixel = 0;
        animationState[indexAnim].StartingColor = frontColor;
        animationState[indexAnim].EndingColor = RgbColor(0, 0, 0);
        animationState[indexAnim].IndexPixel = frontPixel;
      }
      
      animations.StartAnimation(indexAnim, PixelFadeDuration, FadeOutAnimUpdate);
    }
  }
}

//Initialize SPI slave.
void slaveInit(void) {
  // Initialize SPI pins.
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(MISO, OUTPUT);
  pinMode(SS, INPUT);
  // Enable SPI as slave.
  SPCR = (1 << SPE);
  // turn on interrupts
  SPCR |= bit(SPIE);
}

// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;

  if (c == 1)  // starting new sequence?
  {
        SPDR = touched;   // send byte
//    SPDR = 0;   // send byte
  }
}  // end of interrupt service routine (ISR) SPI_STC_vect

void setup()
{
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(9600);
  strip.Begin();
  strip.Show();
  // Initialize SPI Slave.
  slaveInit();
  // Initialize vibration motor.
  vibInit();
  SetRandomSeed();

  // we use the index 0 animation to time how often we move to the next
  // pixel in the strip
  animations.StartAnimation(0, NextPixelMoveDuration, LoopAnimUpdate);
}


void loop()
{
  long sensorVal =  sensor.capacitiveSensor(30);
  //  Serial.println(sensorVal);
  if (sensorVal > THRESHOLD) {
    if (loops != 0) {
      loops = 0;
      frontPixel = 15;
    }
    digitalWrite(LEDPIN, HIGH);
    if (abs(millis() - lastVib) > 300) {
      vib.go();
      lastVib = millis();
    }
    touched = 1;
    //    animations.RestartAnimation(0);
  } else {
    touched = 0;
    digitalWrite(LEDPIN, LOW);
    //    animations.StopAnimation(0);
  }

  animations.UpdateAnimations();
  strip.Show();
}