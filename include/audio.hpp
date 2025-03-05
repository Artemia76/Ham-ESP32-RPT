/**
 * @file audio.hpp
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

#ifndef AUDIO_HPP
#define AUDIO_HPP

#include "log.hpp"
#include "app.hpp"
#include "singleton.hpp"
#include "AudioConfigLocal.h"
#include <AudioTools.h>
#include <AudioTools/AudioLibs/AudioRealFFT.h>
#include <AudioTools/AudioLibs/AudioSourceSPIFFS.h>
#include <CircularBuffer.hpp>

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

// CONST
#define CTCSS_LVL 0.2

/**
 * @brief 
 * 
 */

class CAudio : public CSingleTon<CAudio>, CAppEvent
{
    friend class CSingleTon <CAudio>;
public:

    bool Is1750Detected ();
    bool IsCarriageDetected();
    bool IsCTCSSEnabled();
    void SetVolume(int pChannel, float pValue);
    void Play(int pTrack);

protected:
    void OnUpdate();

private:
    CAudio();
    ~CAudio();

    CLog* _log;
    AudioInfo _info;
    I2SStream _in;
    I2SStream _out;
    SineWaveGenerator<int16_t> _ctcss_sine;
    GeneratedSoundStream<int16_t> _ctcss; 
    OutputMixer<int16_t> _mixer; 
    BufferedStream _mixerIn1;
    BufferedStream _mixerIn2;
    VolumeMeter _volumeMeter;
    AudioRealFFT _fft;
    MultiOutput _multiOutput;
    CircularBuffer <AudioFFTResult,10> _FFTBuf;
    WAVDecoder _decoder;
    AudioSourceSPIFFS _source;
    AudioPlayer _player;
    StreamCopy _inCopier;
    StreamCopy _ctcss_copier;

    bool _CTCSSEnabled;
    bool _CD;
    float _mag_ref;
    float _seuilSquelch; //Squelch threshold
    
    static void fftResultCB (AudioFFTBase &fft);
    void fftResult(AudioFFTBase &fft);
};

#endif //AUDIO_HPP