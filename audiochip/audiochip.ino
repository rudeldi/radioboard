#include <MozziGuts.h>             // Mozzi main library
#include <Oscil.h>                 // Mozzi template for an oscillator
#include <tables/sin2048_int8.h>   // Mozzi wavetable holding a sine wave
#include <twi_nonblock.h>          // Mozzi non-blocking I2C Library

// --- Mozzi definitions ---
Oscil <2048, AUDIO_RATE> aSin(SIN2048_DATA); // defines an sine tone oscillator with the sin2048 wavetable. Structure: Oscil <table_size, update_rate> name(table_data);
#define CONTROL_RATE 64 // number of control updates, only powers of 2
int audioFrequency = 440;

// --- Audio Amplifier I2C definitions ---
#define AMP_ADDRESS 0x60       // TPA6130A2 device address
// Register 1: HP_EN_L, HP_EN_R, Mode[1], Mode[0], Reserved, Reserved, Reserved, Thermal, SWS
#define AMP_REG1 0x1           // TPA6130A2 register 1 address
#define AMP_REG1_SETUP 0xc0    // default configuration: 11000000 - both channels enabled
// Register 2: Mute_L, Mute_R, Volume[5-0]
#define AMP_REG2 0x2           // TPA6130A2 register 2 address
#define AMP_REG2_SETUP 0x34        // default configuration: 00110100 - both channels on -0.3 dB Gain




void setup() {
  startMozzi(CONTROL_RATE);
  initialize_twi_nonblock(); // initialize I2C library and join I2C-Bus
  initializeAmp();
}

void initializeAmp() {
    amp_writeTo(AMP_REG1, AMP_REG1_SETUP);
    amp_writeTo(AMP_REG2, AMP_REG2_SETUP);
}

void updateControl(){
  aSin.setFreq(audioFrequency);
}

int updateAudio(){
  return aSin.next();
}

void amp_writeTo(byte address, byte val) {
  twowire_beginTransmission(AMP_ADDRESS);  // start transmission to device   
  twowire_send( address );                 // send register address
  twowire_send( val );                     // send value to write
  twowire_endTransmission();               // end transmission
}

void loop() {
  audioHook();
}
