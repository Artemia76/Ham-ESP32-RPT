/**
 * @file audio.cpp
 * @author Gianni Peschiutta (F4IKZ)
 * @brief HAM-ESP32-RPT
 * @version 0.1
 * @date 2025-02-13
 * 
 * @copyright Copyright (c) 2025
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

 #include "audio.hpp"

 CAudio::CAudio () :
    _info(22050, 1, 16),
    _ctcss(_ctcss_sine),
    _mixer(_out, 3),
    _mixerIn1(1024,_mixer),
    _mixerIn2(1024,_mixer),
    //_volumeMeter(_mixerIn1),
    _multiOutput(_mixerIn1,_fft),
    _source("/wav",".wav"),
    _player(_source,_mixerIn2,_decoder),
    _inCopier(_multiOutput, _in, 1024),
    //_wavCopier(_decoder,_audioFile),
    _ctcss_copier(_mixer,_ctcss),
    _mag_ref(10000000.0),
    _CTCSSEnabled(false)
{
    _log = CLog::Create();
    _log->Message("Starting Audio... ",false);
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
    tcfg.callback = CAudio::fftResultCB;
    _fft.begin(tcfg);

    //
    // Configure Volume Meter
    //
    //_volumeMeter.begin(_info);

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

    _log->Message("OK");
}

/// @brief Detect a 1750 Hz tone 
/// @return true is 1750 Hz is detected
bool CAudio::Is1750Detected ()
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

void CAudio::fftResultCB(AudioFFTBase &fft)
{
  CAudio::Create()->fftResult(fft);
}

void CAudio::fftResult(AudioFFTBase &fft)
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

bool CAudio::IsCTCSSEnabled()
{
  return _CTCSSEnabled;
}

void CAudio::SetVolume(int pChannel, float pValue)
{
  _mixer.setWeight(pChannel, pValue);
}

void CAudio::Play(const String& pSound)
{
  //_player.begin(pTrack);
  _player.setPath(pSound.c_str());
  _player.setAutoNext(false);
}


void CAudio::OnUpdate()
{
    // Audio Processing
    _inCopier.copy();
    _player.copy();
    _ctcss_copier.copy();
    //_wavCopier.copy();
    if (_mixer.size() > 0)
    {
      _mixer.flushMixer();
    }
}