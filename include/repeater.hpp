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

//#include <Wire.h>
#include <atomic>
#include <AsyncTimer.h>
#include <Preferences.h>
#include <INA219.h>

#include "log.hpp"
#include "appevent.hpp"
#include "singleton.hpp"
#include "audio.hpp"
#include "rssi.hpp"
#include "webserverevent.hpp"

// I/O PINs
#define RX_LED 15
#define TX_LED 14
#define ANNONCE_BTN 11
#define PTT 12

enum Steps
{
  IDLE,
  ANNONCE_DEB,
  REPEATER,
  ANNONCE_FIN
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


    std::atomic<bool> _switch;

    CLog* _log;
    CAudio* _audio;
    uint8_t _counter;
    AsyncTimer _t1s;
    AsyncTimer _t500ms;
    INA219 _ina219;
    int _lastState;
    int _currentState;
    float _RSSI;
    int _TOT;
    int _TOT_Counter;
    bool _lastCD;
    bool _CD;
    bool _enabled;
    int _antiBounce;
    float _CD_Threshold; //RSSI threshold(Squelch)
    int _squelch;
    bool _HalfSecondBlink;
    bool _ina219_ok;
    CRSSI _rssi2signal;
    Preferences _config;

    static void OnTimer1SCB ();
    static void OnTimer500msCB ();
    void OnTimer1S ();
    void OnTimer500ms ();

};

 #endif