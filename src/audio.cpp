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
    _multiOutput(_mixerIn1,_fft),
    _source("/wav",".wav"),
    _player(_source,_mixerIn2,_decoder),
    _inCopier(_multiOutput, _in, 1024),
    _ctcss_copier(_mixer,_ctcss),
    _mag_threshold(30.0),
    _1750_hyst(100.0),
    _CTCSSEnabled(false),
    _audio_ok(false),
    _is_playing(false)
    
{
    _log = CLog::Create();
    _log->Message("Starting Audio... ",false);
    
    //
    // Configure I2S stream input
    //
    auto configin = _in.defaultConfig(RX_MODE); // Create audio stream input config
    configin.copyFrom(_info); // Get default audio parameters
    configin.i2s_format = I2S_STD_FORMAT; // Standard I2S format
    configin.is_master = true; // ESP32 is I2S master
    configin.port_no = 0; // I2S first channel
    configin.pin_ws = AD_LRCK; // LRCK
    configin.pin_bck = AD_SCLK; // SCLK
    configin.pin_data = AD_SDIN; // SDOUT
    configin.pin_mck = AD_MCLK; // MCLK
        if (!_in.begin(configin))
    {
      _log->Message("Failed : Unable to found I2S ADC");
      return;
    }
    
    //
    // Configure out stream
    //
    auto configout = _out.defaultConfig(TX_MODE); // Create audio stream output config
    configout.copyFrom(_info); // Get default audio parameters
    configout.i2s_format = I2S_STD_FORMAT; // Standard I2S format
    configout.is_master = true; // ESP32 is I2S master
    configout.port_no = 1; // I2S first channel
    configout.pin_ws = DA_LRCK; // LRCK
    configout.pin_bck = DA_SCLK; // SCLK
    configout.pin_data = DA_SDIN; // SDOUT
    configout.pin_mck = DA_MCLK; // MCLK
    if (!_out.begin(configout))
    {
      _log->Message("Failed : Unable to found I2S DAC");
      return;
    }

    //
    // Configure Fourrier Fast Transform
    //
    auto tcfg = _fft.defaultConfig();
    tcfg.copyFrom(_info);
    tcfg.length = 512;
    tcfg.window_function = new BufferedWindow(new Hamming());
    tcfg.callback = CAudio::fftResultCB;
    if (!_fft.begin(tcfg))
    {
      _log->Message("Failed : Unable to set FFT");
      return;
    }

    //
    // Configure Player
    //
    _player.setAudioInfo(_info);
    _player.setSilenceOnInactive(true);
    _player.setAutoFade(true);
    if (!_player.begin(0,false))
    {
      _log->Message("Failed : Unable to set Player");
      return;
    }

    //
    // CTCSS Generator
    //
    _ctcss_sine.setFrequency(62.5);
    if (!_ctcss.begin(_info))
    {
      _log->Message("Failed : Unable to set sine generator");
      return;
    }

    //
    // Configure Mixer
    //
    if (!_mixer.begin(1024))
    {
      _log->Message("Failed : Unable to set mixer");
      return;
    }

    SetVolume(1,0.0); // Set mixer volume for stream input to 0.0
    // Load Config
    _config.begin("audio",false);
    _mag_threshold = _config.getFloat("MagThreshold",30); // Loading saved magnitude threshold
    _audio_ok = true;
    _log->Message("OK");
}

/*****************************************************************************/

bool CAudio::Is1750Detected ()
{
  if (!_audio_ok) return false;

  // If there is not enough data in buffer, return false
  if (_FFTBuf.size() <5 ) return false;

  // Parsing FFT buffer to detect 1750 Hz tone
  using index_t = decltype(_FFTBuf)::index_t;
	for (index_t i = 0; i < _FFTBuf.size(); i++)
  {
    _log->Message("Freq = " + String(_FFTBuf[i].frequency) + " Magnitude = "+ String(_FFTBuf[i].magnitude),true,CLog::VERBOSE);
    if (
      (_FFTBuf[i].frequency < (1750.0 + _1750_hyst)) &&
      (_FFTBuf[i].frequency > (1750.0 - _1750_hyst)) &&
      (_FFTBuf[i].magnitude > _mag_threshold))
    {
      return true;
    }
	}
  return false;
}

/*****************************************************************************/

void CAudio::fftResultCB(AudioFFTBase &fft)
{
  // Call the signleton instance to handle the FFT result
  CAudio::Create()->fftResult(fft);
}

/*****************************************************************************/

void CAudio::fftResult(AudioFFTBase &fft)
{
  AudioFFTResult TopScore;
  TopScore.magnitude= 0.0;
  TopScore.frequency= 0.0;
  AudioFFTResult Score[5];
  fft.resultArray(Score);
  for (AudioFFTResult Line : Score)
  {
    if (Line.magnitude > TopScore.magnitude)
    {
      TopScore.frequency = Line.frequency;
      TopScore.magnitude = Line.magnitude;
    }
  }
  // Fill FFT Circular Buffer with new result
  _FFTBuf.push(TopScore);
}

/*****************************************************************************/

bool CAudio::IsCTCSSEnabled()
{
  return _CTCSSEnabled;
}

/*****************************************************************************/

void CAudio::SetVolume(int pChannel, float pValue)
{
  if (!_audio_ok) return;
  if ((pChannel < 0) || (pChannel > 2)) return;
  if ((pValue < 0.0) || (pValue > 1.0)) return;
  _mixer.setWeight(pChannel, pValue);
}

/*****************************************************************************/

void CAudio::Play(const String& pSound)
{
  if ((!_audio_ok) || (_is_playing));
  _is_playing = true;
  SetVolume(1,1.0);
  _player.setPath(String("/wav/" + pSound).c_str());
  _player.setAutoNext(false);
  SetVolume(1,1.0);
  _is_playing = true;
}

/*****************************************************************************/

bool CAudio::IsPlaying()
{
  return _player.isActive();
}

/*****************************************************************************/

void CAudio::OnUpdate()
{
    if (!_audio_ok) return;
    if (_is_playing && !IsPlaying())
    {
      _is_playing = false;
      SetVolume(1,0.0);
    }
 
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

float CAudio::Get1750Threshold()
{
  return _mag_threshold;
}

/*****************************************************************************/

void CAudio::Set1750Threshold(const float& pThreshold)
{
  _mag_threshold = pThreshold;
  if (_mag_threshold<0.0) _mag_threshold = 0.0;
  if (_mag_threshold>100.0) _mag_threshold = 100.0;
  _config.putFloat("MagThreshold",_mag_threshold);
}