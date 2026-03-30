/**
 * @file logic.hpp
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

#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <Preferences.h>
#include <INA219.h>
#include <mutex>

#include "log.hpp"
#include "appevent.hpp"
#include "timer.hpp"
#include "singleton.hpp"
#include "audio.hpp"
#include "rssi.hpp"
#include "webserverevent.hpp"

// I/O PINs
#define RX_LED 15
#define TX_LED 14
#define ANNONCE_BTN 11
#define PTT 12

/**
 * @brief Sequence step of repeater Grafcet
 * 
 */
enum Steps
{
    IDLE,
    START_TX,
    ANNONCE_DEB,
    REPEATER,
    ANNONCE_FIN,
    END_TX
};

class CRepeater : public CSingleTon<CRepeater>, CAppEvent, CWebServerEvent
{
    friend class CSingleTon<CRepeater>;
public:

    void Actions (const Steps& pStep);

protected:

    /**
    * @brief Event from web server on GET command
    * 
    * @param pCommand 
    * @return String 
    */
    String onGet(const String& pCommand, const String& pData);

    /**
    * @brief Event from web server on POST command
    * 
    * @param pCommand 
    * @param pData 
    */
    void onSet(const String& pCommand, const String& pData);
  
    /**
     * @brief Background Update event 
     * 
     */
    void OnUpdate();

private:
    /**
     * @brief Construct a new CRepeater object
     * 
     */
    CRepeater ();

    /**
     * @brief Destroy the CRepeater object
     * 
     */
    ~CRepeater ();

    /**
     * @brief Current step of repeater sequence
     * 
     */
    Steps _step;

    /**
     * @brief Previous step of repeater sequence
     * 
     */
    Steps _lastStep;

    CLog* _log;
    CAudio* _audio;
    uint8_t _counter;
    CTimer _t1s;
    CTimer _t500ms;
    INA219 _ina219;
    int _lastState;
    int _currentState;
    RSSIData _RSSI;
    int _TOT;
    int _TOT_Counter;
    float _StartVol;
    float _RBVol;
    float _EndVol;
    bool _lastCD;
    bool _CD;
    bool _playingRogerBeep;
    int _squelch;
    bool _HalfSecondBlink;
    bool _ina219_ok;
    CRSSI _rssi2signal;
    Preferences _config;
    String _start_message;
    String _end_message;
    String _beep;
    bool _enabled;
    std::recursive_mutex _mutex;

    void OnTimer1S ();
    void OnTimer500ms ();
};

 #endif