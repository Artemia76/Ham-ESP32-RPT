/***************************************************************
 *           FAST FOURIER TRANSFORM
 *           ======================
 *
 * This program uses the Arduino Audio Tools library to display
 * the FFT components of an input signal.
 *
 * Author : Dogan Ibrahim
 * Program: FFT
 * Date   : May, 2023
 *
 * (This program is a modified version of the program developed
 *  by Phil Pschatzmann)
 ***************************************************************/
#include "AudioTools.h"
#include <AudioTools/AudioLibs/AudioRealFFT.h>
#include <CircularBuffer.hpp>
#include <AsyncTimer.h>
#include <math.h>

// A/D Converter PINs

#define AD_MCLK 0
#define AD_LRCK 1
#define AD_SCLK 2
#define AD_SDIN 3

// D/A Converter PINs

#define DA_MCLK 4
#define DA_LRCK 5
#define DA_SCLK 6
#define DA_SDIN 7

// I/O PINs

#define RX_LED 8
#define TX_LED 9

// Définition des variables globales
uint16_t sample_rate=22050;
uint8_t channels = 1;
I2SStream in;
AudioRealFFT fft;
StreamCopy copier(fft, in);
float mag_ref = 10000000.0;
CircularBuffer <AudioFFTResult,10> FFTBuf;
AsyncTimer t;
bool TXMode = false;
bool CD = false;
float Squelch = 100000.0;
uint8_t Counter = 0;

bool Is1750Detected ()
{
  float tolerance = 10; // Tolérance de détection
  // Si il n'y a pas assez de données dans le buffer on passe
  if (FFTBuf.size() <10 ) return false;
  float avg=0.0;
  using index_t = decltype(FFTBuf)::index_t;
	for (index_t i = 0; i < FFTBuf.size(); i++) {
			avg += FFTBuf[i].frequency / (float)FFTBuf.size();
	}
  Serial.println( "Freq = " + String(avg));
  if ((avg < (1750.0 + tolerance)) && (avg > (1750.0-tolerance)))
  {
    return true;
  }
  return false;
}

void OnTimer ()
{
  Serial.println("Timer shoot");
  if (!TXMode)
  {
    digitalWrite(RX_LED, HIGH);
    digitalWrite(TX_LED,LOW);
    if (Is1750Detected() && CD)
    {
      Counter ++;
    }
    else
    {
      Counter = 0;
    }
    if (Counter >= 2)
    {
      TXMode = true;
    }
  }
  else
  {
    digitalWrite(RX_LED, LOW);
    digitalWrite(TX_LED,HIGH);
    if (!CD) Counter ++;
    else Counter = 0;
    if (Counter > 10)
    {
      TXMode = false;
    }
  }
  
}

//
// Display fft result on Serial Monitor
//
void fftResult(AudioFFTBase &fft)
{
    float diff;
    float CarriageAvg=0.0;
    AudioFFTResult Score[5];
    fft.resultArray(Score);
    Serial.println("-----------------------");
    for (AudioFFTResult Line : Score)
    {
      Serial.print(Line.frequency);
      Serial.print(" ");
      float dbm = 20.0 * log10f(Line.magnitude/mag_ref);
      Serial.println(String(Line.magnitude) + " Mag");
      CarriageAvg += Line.magnitude / 5;
    }

    if (CarriageAvg > Squelch)
    {
      CD = true;
      FFTBuf.push(Score[0]);
    }
    else{
      CD =false;
    }
    Serial.println("Cariage average = " + String(CarriageAvg));
    Serial.println("-----------------------");
}

void setup(void)
{
  Serial.begin(115200);
 
//
// Configure in stream
//
  auto configin = in.defaultConfig(RX_MODE);
  configin.sample_rate = sample_rate;
  configin.channels = channels;
  configin.bits_per_sample = 16;
  configin.i2s_format = I2S_STD_FORMAT;
  configin.is_master = true;
  configin.port_no = 0;
  configin.pin_ws = AD_LRCK;                        // LRCK
  configin.pin_bck = AD_SCLK;                       // SCLK
  configin.pin_data = AD_SDIN;                      // SDOUT
  configin.pin_mck = AD_MCLK;
  in.begin(configin);

  //
  // Configure FFT
  //
  auto tcfg = fft.defaultConfig();
  tcfg.length = 1024;
  tcfg.channels = channels;
  tcfg.sample_rate = sample_rate;
  tcfg.bits_per_sample = 16;
  tcfg.window_function = new BlackmanHarris;
  tcfg.callback = &fftResult;
  fft.begin(tcfg);

  //
  // Configure I/O
  //
  pinMode(RX_LED, OUTPUT);
  pinMode(TX_LED, OUTPUT);


  //
  // Configure Slow Timer
  //
  t.setInterval(OnTimer, 1000);
}

//
// Copy input signal to fft
//
void loop()
{
  copier.copy();
  t.handle();
}