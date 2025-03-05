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

#include "log.hpp"
#include "app.hpp"
#include "singleton.hpp"
#include "audio.hpp"

#include <AsyncTimer.h>
#include <atomic>

// I/O PINs
#define RX_LED 8
#define TX_LED 9
#define ANNONCE_BTN 10

enum Mode
{
  IDLE,
  ANNONCE_DEB,
  REPEATER,
  ANNONCE_FIN
};

class CRepeater : public CSingleTon<CRepeater>, CAppEvent
{
  friend class CSingleTon<CRepeater>;
public:
    void Actions (const Mode& pState);

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
  
  /**
   * @brief Background Update event 
   * 
   */
  void OnUpdate();

  private:
    CRepeater ();
    ~CRepeater ();

    Mode _etat;
    Mode _lastEtat;
    std::atomic<bool> _switch;

    CLog* _log;
    CAudio* _audio;
    uint8_t _counter;
    AsyncTimer _t;
    int _lastState;
    int _currentState;

    static void OnTimerCB ();
    void OnTimer ();

};

 #endif