/**
 * @file app.cpp
 * @author Gianni Peschiutta (F4IKZ)
 * @brief HAM-ESP32-RPT
 * @version 0.1
 * @date 2024-11-21
 * 
 * @copyright Copyright (c) 2024
 *
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *    This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>
 */

#include "app.hpp"

/*****************************************************************************/

CApp::CApp () :
    _switch(false),
    _info(22050, 1, 16),
    _ctcss(_ctcss_sine),
    _mixer(_out, 3),
    _mixerIn1(1024,_mixer),
    _mixerIn2(1024,_mixer),
    _volumeMeter(_mixerIn1),
    _multiOutput(_volumeMeter,_fft),
    _source("/wav",".wav"),
    _player(_source,_mixerIn2,_decoder),
    _inCopier(_multiOutput, _in, 1024),
    _ctcss_copier(_mixer,_ctcss),
    _etat(IDLE),
    _lastEtat(IDLE),
    _CD(false),
    _mag_ref(10000000.0),
    _seuilSquelch(-30.0),
    _counter(0),
    _lastState(HIGH),
    _currentState(HIGH),
    _CTCSSEnabled(false)
{
  _log = CLog::Create();
  _log->Message("Starting Repeater");
  _switch =true;
  //
  // Configure in stream
  //
  auto configin = _in.defaultConfig(RX_MODE);
  configin.copyFrom(_info);
  configin.i2s_format = I2S_STD_FORMAT;
  configin.is_master = true;
  configin.port_no = 0;
  configin.pin_ws = AD_LRCK;                        // LRCK
  configin.pin_bck = AD_SCLK;                       // SCLK
  configin.pin_data = AD_SDIN;                      // SDOUT
  configin.pin_mck = AD_MCLK;
  _in.begin(configin);
  
  //
  // Configure out stream
  //
  auto configout = _out.defaultConfig(TX_MODE);
  configout.copyFrom(_info);
  configout.i2s_format = I2S_STD_FORMAT;
  configout.is_master = true;
  configout.port_no = 1;
  configout.pin_ws = DA_LRCK;                        // LRCK
  configout.pin_bck = DA_SCLK;                       // SCLK
  configout.pin_data = DA_SDIN;                      // SDOUT
  configout.pin_mck = DA_MCLK;
  _out.begin(configout);

  //
  // Configure FFT
  //
  auto tcfg = _fft.defaultConfig();
  tcfg.length = 512;
  tcfg.copyFrom(_info);
  //tcfg.window_function = new BlackmanHarris;
  tcfg.callback = CApp::fftResultCB;
  _fft.begin(tcfg);

  //
  // Configure Volume Meter
  //
  _volumeMeter.begin(_info);

  //
  // Configure Player
  //
  _player.setAudioInfo(_info);
  _player.setSilenceOnInactive(true);
  _player.begin(0,false);


  //
  // CTCSS Generator
  //
  _ctcss_sine.setFrequency(62.5);
  _ctcss.begin(_info);
  //
  // Configure Mixer
  //
  _mixer.begin(1024);

  //
  // Configure I/O
  //
  pinMode(RX_LED, OUTPUT);
  pinMode(TX_LED, OUTPUT);
  pinMode(ANNONCE_BTN, INPUT_PULLUP);


  //
  // Configure Slow Timer
  //
  _t.setInterval(CApp::OnTimerCB,1000);

  Actions(IDLE);

}

/*****************************************************************************/

CApp::~CApp ()
{
  _log->Message("Ending Repeater");
}

/*****************************************************************************/

void CApp::Loop ()
{
  _currentState = digitalRead(ANNONCE_BTN);
  if (_lastState == LOW && _currentState == HIGH)
  {
    Actions(ANNONCE_DEB);
  }
  _lastState = _currentState;
  _t.handle();
  // Audio Processing
  _inCopier.copy();
  _player.copy();
  _ctcss_copier.copy();
  if (_mixer.size() > 0)
  {
    _mixer.flushMixer();
  }
}

/*****************************************************************************/

String CApp::onGET (const String& pCommand)
{
  String Response;
  if (pCommand=="RepOn")
  {
    _log->Message("RepOn");
    _switch = true;
  } else if (pCommand=="RepOff")
  {
    _log->Message("RepOff");
    _switch = false;
    Actions(IDLE);
  } else if (pCommand=="CTCSSOn")
  {
    _log->Message("CTCSSOn");
    _CTCSSEnabled = true;
  } else if (pCommand=="CTCSSOff")
  {
    _log->Message("CTCSSOff");
    _CTCSSEnabled = false;
    _mixer.setWeight(2,0);
  } else if (pCommand=="lireLuminosite")
  {
    Response = String(_volumeMeter.volumeDB());
  }
  return Response;
}

/*****************************************************************************/

void CApp::onPOST (const String& pCommand, const String& pData)
{
  if (pCommand == "set")
  {
    //_get_RGB(pData);
  }
}

/// @brief Detect a 1750 Hz tone 
/// @return true is 1750 Hz is detected
bool CApp::Is1750Detected ()
{
  float tolerance = 100; // Tolérance de détection
  // Si il n'y a pas assez de données dans le buffer on passe
  if (_FFTBuf.size() <10 ) return false;
  using index_t = decltype(_FFTBuf)::index_t;
	for (index_t i = 0; i < _FFTBuf.size(); i++)
  {

      //Serial.println( "Freq = " + String(FFTBuf[i].frequency) + " Magnitude = "+ String(FFTBuf[i].magnitude));
			if (
        (_FFTBuf[i].frequency < (1750.0 + tolerance)) &&
        (_FFTBuf[i].frequency > (1750.0-tolerance)) &&
        (_FFTBuf[i].magnitude > 100000.0))
      {
        return true;
      }
	}
  return false;
}

void CApp::Actions (const Mode& pState)
{
  if (!_switch) return;
  switch (pState)
  {
    case IDLE:
    {
      _log->Message("IDLE");
      digitalWrite(RX_LED, HIGH);
      digitalWrite(TX_LED,LOW);
      _mixer.setWeight(0,0.0);
      _mixer.setWeight(1,0.0);
      _mixer.setWeight(2,0.0);
      break;
    }
    case ANNONCE_DEB:
    {
      _log->Message("Annonce début");
      digitalWrite(RX_LED, LOW);
      digitalWrite(TX_LED,HIGH);
      //mute input sound still playing welcome
      _mixer.setWeight(0,0.0);
      _mixer.setWeight(1,1.0);
      if (_CTCSSEnabled) _mixer.setWeight(2,CTCSS_LVL);
      _player.begin(1);
      _player.setAutoNext(false);
      break;
    }
    case REPEATER:
    {
      _log->Message("Repeater");
      digitalWrite(RX_LED, LOW);
      digitalWrite(TX_LED,HIGH);
      _mixer.setWeight(0,1.0);
      _mixer.setWeight(1,0.0);
      if (_CTCSSEnabled) _mixer.setWeight(2,CTCSS_LVL);
      break;
    }
    case ANNONCE_FIN:
    {
      _log->Message("Annonce Fin");
      digitalWrite(RX_LED, LOW);
      digitalWrite(TX_LED,HIGH);
      _mixer.setWeight(0,0.0);
      _mixer.setWeight(1,1.0);
      if (_CTCSSEnabled) _mixer.setWeight(2,CTCSS_LVL);
      _player.begin(0);
      _player.setAutoNext(false);
      break;
    }
  }
  _etat = pState;
}

void CApp::OnTimerCB ()
{
  CApp::Create()->OnTimer();
}

/// @brief Sequence is reviewed each second
void CApp::OnTimer ()
{
  if (_volumeMeter.volumeDB() > _seuilSquelch)
  {
    Serial.println("Volume = " + String(_volumeMeter.volumeDB()));
    _CD = true;
  }
  else
  {
    _CD =false;
  }
  //Grafcet
  switch (_etat)
  {
    case IDLE:
    {
      if (Is1750Detected() && _CD)
      {
        _counter ++;
      }
      else
      {
        _counter = 0;
      }
      if (_counter >= 2)
      {
        Actions(ANNONCE_DEB);
        _counter=0;
      }
      break;
    }
    case ANNONCE_DEB:
    {
      _counter++;
      if (_counter > 3)
      {
        Actions(REPEATER);
        _counter=0;
      }
      break;
    }
    case REPEATER:
    {
      if (!_CD) _counter ++;
      else _counter = 0;
      if (_counter > 10)
      {
        Actions(ANNONCE_FIN);
        _counter=0;
      }
      break;
    }
    case ANNONCE_FIN:
    {
      _counter++;
      if (_counter > 3)
      {
        Actions(IDLE);
        _counter=0;
      }
      break;
    }
  }
}

void CApp::fftResultCB(AudioFFTBase &fft)
{
  CApp::Create()->fftResult(fft);
}

//
// Fill a circular buffer to analyse freq
//
void CApp::fftResult(AudioFFTBase &fft)
{
    float diff;
    float CarriageAvg=0.0;
    AudioFFTResult Score[5];
    fft.resultArray(Score);
    for (AudioFFTResult Line : Score)
    {
      CarriageAvg += Line.magnitude / 5;
      _FFTBuf.push(Line);
    }
}
