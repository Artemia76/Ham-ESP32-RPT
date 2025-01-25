#include <Arduino.h>
#include "AudioConfigLocal.h"
#include <AudioTools.h>
#include <AudioTools/AudioLibs/AudioRealFFT.h>
#include <AudioTools/AudioLibs/AudioSourceSPIFFS.h>
#include <AudioTools/Concurrency/RTOS.h>
#include <CircularBuffer.hpp>
#include <AsyncTimer.h>
#include <atomic>

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
#define ANNONCE_BTN 10


// Définition des variables globales
AudioInfo info(22050, 1, 16);
I2SStream in;
I2SStream out;
//OutputMixer<int16_t> mixer (out,1);
VolumeMeter VolMeter(out);
AudioRealFFT fft(VolMeter);
CircularBuffer <AudioFFTResult,10> FFTBuf;
AsyncTimer t;
WAVDecoder decoder;
AudioSourceSPIFFS source("/",".wav");
//BufferedStream BufPlayer(DEFAULT_BUFFER_SIZE);
AudioPlayer player(source,out,decoder);
StreamCopy copier(fft, in);
//StreamCopy CpyMixer1(mixer, VolMeter);
//StreamCopy CpyMixer2(mixer, BufPlayer);

Task task("fft-copy", 10000, 1, 0);

enum Mode
{
  IDLE,
  ANNONCE_DEB,
  REPEATER,
  ANNONCE_FIN
};

std::atomic<Mode> Etat(IDLE);
Mode LastEtat=IDLE;

bool CD = false;
float mag_ref = 10000000.0;
float SeuilSquelch = -30.0; //Seuil de déclenchement du squelch en dB
uint8_t Counter = 0;

// Functions Proto
void fftResult(AudioFFTBase &fft);
void OnTimer ();
bool Is1750Detected ();
int lastState = HIGH;
int currentState;

void setAudioChain(bool Flag)
{
  // Audio in active
  if (!Flag)
  {
    //
    // Configure in stream
    //
    auto configin = in.defaultConfig(RX_MODE);
    configin.copyFrom(info);
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
    tcfg.length = 512;
    tcfg.copyFrom(info);
    //tcfg.window_function = new BlackmanHarris;
    tcfg.callback = &fftResult;
    fft.begin(tcfg);

    //
    // Configure Volume Meter
    //
    VolMeter.begin(info);
  }
  else
  {
    in.end();
    fft.end();
    VolMeter.end();

  }
}

/// @brief 
/// @param  
void setup(void)
{
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);
 
  //
  // Configure in stream
  //
  auto configin = in.defaultConfig(RX_MODE);
  configin.copyFrom(info);
  configin.i2s_format = I2S_STD_FORMAT;
  configin.is_master = true;
  configin.port_no = 0;
  configin.pin_ws = AD_LRCK;                        // LRCK
  configin.pin_bck = AD_SCLK;                       // SCLK
  configin.pin_data = AD_SDIN;                      // SDOUT
  configin.pin_mck = AD_MCLK;
  in.begin(configin);
  
  //
  // Configure out stream
  //

  auto configout = out.defaultConfig(TX_MODE);
  configout.copyFrom(info);
  configout.i2s_format = I2S_STD_FORMAT;
  configout.is_master = true;
  configout.port_no = 1;
  configout.pin_ws = DA_LRCK;                        // LRCK
  configout.pin_bck = DA_SCLK;                       // SCLK
  configout.pin_data = DA_SDIN;                      // SDOUT
  configout.pin_mck = DA_MCLK;
  out.begin(configout);

  setAudioChain(true);
  //
  // Configure Player
  //
  player.setBufferSize(DEFAULT_BUFFER_SIZE);
  player.setAudioInfo(info);

  //
  // Configure I/O
  //
  pinMode(RX_LED, OUTPUT);
  pinMode(TX_LED, OUTPUT);
  pinMode(ANNONCE_BTN, INPUT_PULLUP);


  //
  // Configure Slow Timer
  //
  t.setInterval(OnTimer, 1000);

  // Configure Task
  task.begin([]()
  {
    if (LastEtat != Etat)
    {
      switch (Etat)
      {
        case IDLE:
        case REPEATER:
        {
          player.end();
          setAudioChain(true);
          break;
        }
        case ANNONCE_DEB:
        {
          setAudioChain(false);
          player.begin(1);
          player.setAutoNext(false);
          break;
        }
        case ANNONCE_FIN:
        {
          setAudioChain(false);
          player.begin(0);
          player.setAutoNext(false);
          break;
        }
      }
      LastEtat = Etat;
    }
    switch (Etat)
    {
      case IDLE:
      case REPEATER:
      {
        copier.copy();
        break;
      }
      case ANNONCE_DEB:
      case ANNONCE_FIN:
      {
        player.copy();
        break;
      }
    }
  });

}

bool Is1750Detected ()
{
  float tolerance = 100; // Tolérance de détection
  // Si il n'y a pas assez de données dans le buffer on passe
  if (FFTBuf.size() <10 ) return false;
  using index_t = decltype(FFTBuf)::index_t;
	for (index_t i = 0; i < FFTBuf.size(); i++)
  {
      //Serial.println( "Freq = " + String(FFTBuf[i].frequency) + " Magnitude = "+ String(FFTBuf[i].magnitude));
			if (
        (FFTBuf[i].frequency < (1750.0 + tolerance)) &&
        (FFTBuf[i].frequency > (1750.0-tolerance)) &&
        (FFTBuf[i].magnitude > 100000.0))
      {
        return true;
      }
	}
  return false;
}

void OnTimer ()
{
  if (VolMeter.volumeDB() > SeuilSquelch)
  {
    Serial.println("Volume = " + String(VolMeter.volumeDB()));
    CD = true;
  }
  else
  {
    CD =false;
  }
  //Grafcet
  switch (Etat)
  {
    case IDLE:
    {
      LOGW("IDLE");
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
      if (Counter >= 1)
      {
        Etat = ANNONCE_DEB;
        Counter=0;
      }
      break;
    }
    case ANNONCE_DEB:
    {
      LOGW("Annonce début");
      digitalWrite(RX_LED, LOW);
      digitalWrite(TX_LED,HIGH);
      Counter++;
      if (Counter > 3)
      {
        Etat = REPEATER;
        Counter=0;
      }
      break;
    }
    case REPEATER:
    {
      LOGW("Repeater");
      digitalWrite(RX_LED, LOW);
      digitalWrite(TX_LED,HIGH);
      if (!CD) Counter ++;
      else Counter = 0;
      if (Counter > 10)
      {
        Etat = ANNONCE_FIN;
        Counter=0;
      }
      break;
    }
    case ANNONCE_FIN:
    {
      LOGW("Annonce Fin");
      digitalWrite(RX_LED, LOW);
      digitalWrite(TX_LED,HIGH);
      Counter++;
      if (Counter > 3)
      {
        Etat = IDLE;
        Counter=0;
      }
      break;
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
    //Serial.println("-----------------------");
    for (AudioFFTResult Line : Score)
    {
      //Serial.print(Line.frequency);
      //Serial.print(" ");
      //Serial.println(String(Line.magnitude) + " Mag");
      CarriageAvg += Line.magnitude / 5;
      FFTBuf.push(Line);
    }

    //Serial.println("Cariage average = " + String(CarriageAvg));
    //Serial.println("-----------------------");
}



//
// Copy input signal to fft
//
void loop()
{
  currentState = digitalRead(ANNONCE_BTN);
  if (lastState == LOW && currentState == HIGH)
  {
    Etat = ANNONCE_DEB;
    LOGW("Playing Annonce...");
  }
  lastState = currentState;
  t.handle();
}