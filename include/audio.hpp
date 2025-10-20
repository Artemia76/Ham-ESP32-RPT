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

#include <Preferences.h>
#include <AudioTools.h>
#include <AudioTools/AudioLibs/AudioRealFFT.h>
#include <AudioTools/Disk/AudioSourceLittleFS.h>
#include <map>
#include <CircularBuffer.hpp>

#include "log.hpp"
#include "app.hpp"
#include "singleton.hpp"
#include "AudioConfigLocal.hpp"


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
 * @brief CAudio class manage audio processing
 * @details This class manage input and output audio for repeater
 *          It also manage CTSS sine wave generation and 1750Hz detection
 *          by using FFT
 */
class CAudio : public CSingleTon<CAudio>, CAppEvent
{
    /**
     * @brief Ensure this class is the only existing one on system
     * 
     */
    friend class CSingleTon <CAudio>;
public:

    /**
     * @brief Analyse the FFT sampling. The target frequency is 1750Hz tone
     * 
     * @return true if 1750 Hz is detected as main frequency
     * @return false if no 1750Hz with significative magnitude 
     */
    bool Is1750Detected ();

    /**
     * @brief Get CCTCSS Status : CTCSS is a low frequency tone to trigger remote Radio Receiver
     * 
     * @return true : CTCSS running
     * @return false : CTCSS disabled
     */
    bool IsCTCSSEnabled();

    /**
     * @brief Set the Mixer Volume track
     * 
     * @param pChannel the track number
     * @param pValue  the volume value (0.0 to 1.0)
     */
    void SetVolume(int pChannel, float pValue);

    /**
     * @brief Play the wav sound in /wav folder on SD Memory 
     * 
     * @param pSound : the sound name
     */
    void Play(const String& pSound);

    /**
     * @brief Return the wav player status
     * 
     * @return true if currently playing Wav
     * @return false if player is idle
     */
    bool IsPlaying ();

    float Get1750Threshold();

    void Set1750Threshold(const float& pThreshold);

protected:

    /**
     * @brief Main loop callback from app
     * 
     */
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
    AudioRealFFT _fft;
    MultiOutput _multiOutput;
    CircularBuffer <AudioFFTResult,5> _FFTBuf;
    WAVDecoder _decoder;
    AudioSourceLittleFS _source;
    AudioPlayer _player;
    StreamCopy _inCopier;
    StreamCopy _ctcss_copier;
    Preferences _config;

    bool _CTCSSEnabled;
    bool _audio_ok;
    float _mag_threshold;
    float _1750_hyst;
    bool _is_playing;
    std::map <String, File> _catalog;
    static void fftResultCB (AudioFFTBase &fft);
    void fftResult(AudioFFTBase &fft);
};

#endif //AUDIO_HPP