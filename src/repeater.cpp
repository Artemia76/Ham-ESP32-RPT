/**
 * @file repeater.cpp
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

#include <ArduinoJson.h>

#include "repeater.hpp"

CRepeater::CRepeater() :
    _step(IDLE),
    _lastStep(IDLE),
    _counter(0),
    _lastState(HIGH),
    _currentState(HIGH),
    _switch(true),
    _CD(false),
    _antiBounce(0),
    _TOT(180),
    _TOT_Counter(0),
    _HalfSecondBlink(false),
    _enabled(true),
    _squelch(9),
    _ina219(0x40),
    _ina219_ok(false),
    _start_message("louise2.wav"),
    _end_message("end.wav"),
    _beep("beep.wav")
{
  _log = CLog::Create();
  _log->Message("Starting Repeater... ");

  _audio = CAudio::Create();
  _lastCD = _CD;

  //
  // Setting I/O
  //
  pinMode(RX_LED, OUTPUT);
  pinMode(TX_LED, OUTPUT);
  pinMode(ANNONCE_BTN, INPUT_PULLUP);
  pinMode(PTT, OUTPUT);
  
  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  Wire.begin();
  if (! _ina219.begin())
  {
    _log->Message("Failed to find INA219 chip SDA="+ String(SDA) + " SDL=" +String(SCL));
  }
  else
  {
    _ina219_ok=true;
    _RSSI = _rssi2signal.getByVoltage(_ina219.getBusVoltage());
  }
  //
  // Setting Slow Timer
  //
  _t1s.setInterval(CRepeater::OnTimer1SCB,1000);

  //
  // Setting Fast Timer
  //
  _t500ms.setInterval(CRepeater::OnTimer500msCB,500);

  // Load Config
  _config.begin("repeater",false);
  _enabled = _config.getBool("Enabled","true");

  Actions(IDLE);
  _log->Message("OK");
}

/*****************************************************************************/

CRepeater::~CRepeater()
{
  _config.end();
}

/*****************************************************************************/

void CRepeater::OnTimer500msCB()
{
  CRepeater::Create()->OnTimer500ms();
}

/*****************************************************************************/

void CRepeater::OnTimer500ms()
{
  // Animate Green leds
  // If Carriage Detect in IDLE mode , we blink Green Led
  if (_step==IDLE)
  {
    if (_CD && _step==IDLE)
    {
      digitalWrite(RX_LED, _HalfSecondBlink);
    }
    else
    {
      digitalWrite(RX_LED, HIGH);
    }
    digitalWrite(TX_LED, LOW);
  }
  //if Carriage not detected during Repeater active, we blink Red led
  else
  {
    if (!_CD || _step==ANNONCE_FIN)
    {
      digitalWrite(TX_LED, _HalfSecondBlink);
    }
    else
    {
      digitalWrite(TX_LED, HIGH);
    }
    digitalWrite(RX_LED, LOW);
  }
  _HalfSecondBlink = !_HalfSecondBlink;
}

/*****************************************************************************/

void CRepeater::OnTimer1SCB()
{
  CRepeater::Create()->OnTimer1S();
}

/*****************************************************************************/

void CRepeater::OnTimer1S()
{
  if (!_enabled) return;
  //Grafcet
  switch (_step)
  {
    case IDLE:
    {
      if (_audio->Is1750Detected() && _CD)
      {
        _counter ++;
      }
      else
      {
        _counter = 0;
      }
      if (_counter >= 1)
      {
        Actions(START_TX);
      }
      break;
    }
    case START_TX:
    {
      _counter ++;
      if ((_counter>1) && (!_audio->Is1750Detected()))
      {
        Actions(ANNONCE_DEB);
      }
      break;
    }
    case ANNONCE_DEB:
    {
      if (!_audio->IsPlaying())
      {
        Actions(REPEATER);
      }
      break;
    }
    case REPEATER:
    {
      if (! _CD) _counter ++;
      else _counter = 0;
      _TOT_Counter++;

      if ((_counter > 10) || (_TOT_Counter >= _TOT))
      {
        Actions(ANNONCE_FIN);
      }
      break;
    }
    case ANNONCE_FIN:
    {
      if (!_audio->IsPlaying())
      {
        Actions(END_TX);
      }
      break;
    }
    case END_TX:
    {
      _counter++;
      if (_counter > 1)
      {
        Actions(IDLE);
      }
    }
  }
  //Reset _antiBounce
  if (_antiBounce > 0)
  {
    _antiBounce--;
    if (_antiBounce==0) _audio->SetVolume(1,0.0);
  }
  _log->Message("RSSI dBm = " + String(_RSSI.dBm) +
    " RSSI S = " +String(_RSSI.S) +
    " RSSI V = " +String(_RSSI.V) +
    " TOT Timer = " + String(_TOT_Counter),true, CLog::DEBUG);
}

/*****************************************************************************/

void CRepeater::Actions(const Steps& pStep)
{
  _counter = 0;
  if (!_switch) return;
  switch (pStep)
  {
    case IDLE:
    {
      _log->Message("IDLE");
      _audio->SetVolume(0,0.0);
      _audio->SetVolume(1,0.0);
      _audio->SetVolume(2,0.0);
      digitalWrite(PTT,LOW);
      break;
    }
    case START_TX:
    {
      _log->Message("Switch to TX");
      digitalWrite(PTT,HIGH);
      break;
    }
    case ANNONCE_DEB:
    {
      _log->Message("Annonce début");
      //mute input sound still playing welcome
      _audio->SetVolume(0,0.0);
      _audio->SetVolume(1,1.0);
      if ( _audio->IsCTCSSEnabled()) _audio->SetVolume(2,CTCSS_LVL);
      _audio->Play(_start_message);
      break;
    }
    case REPEATER:
    {
      _log->Message("Repeater");
      _lastCD = _CD;
      if (_CD)
        _audio->SetVolume(0,1.0);
      else 
        _audio->SetVolume(0,0.0);
      _audio->SetVolume(1,0.0);
      if (_audio->IsCTCSSEnabled()) _audio->SetVolume(2,CTCSS_LVL);
      _TOT_Counter = 0;
      break;
    }
    case ANNONCE_FIN:
    {
      _log->Message("Annonce Fin");
      _audio->SetVolume(1,1.0);
      if (_audio->IsCTCSSEnabled()) _audio->SetVolume(2,CTCSS_LVL);
      _audio->Play(_end_message);
      break;
    }
    case END_TX:
    {
      _log->Message("End TX");
      break;
    }
  }
  _step = pStep;
}

/*****************************************************************************/

void CRepeater::OnUpdate()
{
  if (_enabled)
  {
    _currentState = digitalRead(ANNONCE_BTN);
    if (_lastState == LOW && _currentState == HIGH)
    {
      Actions(START_TX);
    }
    _lastState = _currentState;
    // Read RSSI and if threshold, play a K
    if (_ina219_ok)
      _RSSI = _rssi2signal.getByVoltage(_ina219.getBusVoltage());

    _CD = (_RSSI.S >= _squelch);
    if (_CD != _lastCD)
    {
      if (_step == REPEATER)
      {
        if (!_CD)
        {
          _audio->SetVolume(0,0.0);
        }
        else
        {
          _audio->SetVolume(0,1.0);
        }
        // If we loose carriage and we are in repeater mode, play RogerBeep
        // _antiBouce is to avoid bounce CD detection
        if ((!_CD) && (_antiBounce==0))
        {
          _audio->SetVolume(1,1.0);
          _audio->Play(_beep);
          _TOT_Counter = 0;
          _antiBounce = 2;
        } 
      }
      else
      {
        _audio->SetVolume(0,0.0);
      }
      _lastCD = _CD;
    }
  }
  _t1s.handle();
  _t500ms.handle();

}

/*****************************************************************************/

String CRepeater::onGet(const String& pCommand, const String& pData)
{
  String Result = "";
  if (pCommand == "RSSI")
  {
    Result = String(_RSSI.S);
  }
  else if (pCommand == "Config")
  {
    JsonDocument json;
    json["state"] =  _enabled;
    json["squelch"] = _squelch;
    json["tot"] = _TOT;
    serializeJson(json, Result);
  }
  return Result;
}

/*****************************************************************************/

void CRepeater::onSet(const String& pCommand, const String& pData)
{
  if (pCommand == "Rep")
  {
    if (pData == "true")
    {
      _enabled=true;
    }
    else if (pData == "false")
    {
      _enabled=false;
      Actions(IDLE);
    }
    _config.putBool("Enabled",_enabled);
  }
}