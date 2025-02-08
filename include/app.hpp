/**
 * @file app.hpp
 * @author Artemia
 * @brief Test-ESP32 Project
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

#ifndef APP_HPP
#define APP_HPP

#include <Arduino.h>
#include <atomic>

#include "webserver.hpp"
#include "log.hpp"

#include "AudioConfigLocal.h"
#include <AudioTools.h>
#include <AudioTools/AudioLibs/AudioRealFFT.h>
#include <AudioTools/AudioLibs/AudioSourceSPIFFS.h>
#include <CircularBuffer.hpp>
#include <AsyncTimer.h>
#include "singleton.hpp"


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

// CONST
#define CTCSS_LVL 0.2

enum Mode
{
  IDLE,
  ANNONCE_DEB,
  REPEATER,
  ANNONCE_FIN
};

/**
 * @brief Main application class
 * 
 */

class CApp : public CSingleTon<CApp>, CWebServerEvent 
{
    friend class CWebServerEvent;
    friend class CSingleTon<CApp>;
public:

    void Loop ();

protected:

     /**
     * @brief Event from web server on GET command
     * 
     * @param pCommand 
     * @return String 
     */
    String onGET(const String& pCommand);

    /**
     * @brief Event from web server on POST command
     * 
     * @param pCommand 
     * @param pData 
     */
    void onPOST(const String& pCommand, const String& pData);

private:
    /**
     * @brief Construct a new Application object
     * 
     * @param pLog Text log handler
     * @param pwebServer Web Server to subscribe events
     */
    CApp();

    /**
     * @brief Destroy the main application object
     * 
     */
    ~CApp();

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
    AsyncTimer _t;
    WAVDecoder _decoder;
    AudioSourceSPIFFS _source;
    AudioPlayer _player;
    StreamCopy _inCopier;
    StreamCopy _ctcss_copier;
    std::atomic<bool> _switch;
    
    bool _CTCSSEnabled;

    Mode _etat;
    Mode _lastEtat;

    bool _CD;
    float _mag_ref;
    float _seuilSquelch; //Squelch threshold
    uint8_t _counter;
    int _lastState;
    int _currentState;

static void fftResultCB (AudioFFTBase &fft);
    void fftResult(AudioFFTBase &fft);
static void OnTimerCB ();
    void OnTimer ();
    bool Is1750Detected ();
    void Actions (const Mode& pState);

    
    /**
     * @brief Log handler to write message on terminal
     * 
     */
    CLog* _log;
};

#endif