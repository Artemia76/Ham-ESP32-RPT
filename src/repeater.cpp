#include "repeater.hpp"

CRepeater::CRepeater() :
    _etat(IDLE),
    _lastEtat(IDLE),
    _counter(0),
    _lastState(HIGH),
    _currentState(HIGH),
    _switch(true),
    _CD_Threshold(3000),
    _CD(false)
{
  _log = CLog::Create();
  _log->Message("Starting Repeater... ");

  _audio = CAudio::Create();
  _RSSI = analogRead(RSSI);
  _lastCD = _CD;

  //
  // Configure I/O
  //
  pinMode(RX_LED, OUTPUT);
  pinMode(TX_LED, OUTPUT);
  pinMode(ANNONCE_BTN, INPUT_PULLUP);
  
  //
  // Configure Slow Timer
  //
  _t.setInterval(CRepeater::OnTimerCB,1000);

  Actions(IDLE);
  _log->Message("OK");
}

CRepeater::~CRepeater()
{

}

void CRepeater::OnTimerCB()
{
  CRepeater::Create()->OnTimer();
}

void CRepeater::OnTimer()
{
  //Grafcet
  switch (_etat)
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
      if (! _CD) _counter ++;
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
  _log->Message("RSSI=" + String(_RSSI),true, CLog::DEBUG);
}

void CRepeater::Actions(const Mode& pState)
{
  if (!_switch) return;
  switch (pState)
  {
    case IDLE:
    {
      _log->Message("IDLE");
      digitalWrite(RX_LED, HIGH);
      digitalWrite(TX_LED,LOW);
      _audio->SetVolume(0,0.0);
      _audio->SetVolume(1,0.0);
      _audio->SetVolume(2,0.0);
      break;
    }
    case ANNONCE_DEB:
    {
      _log->Message("Annonce début");
      digitalWrite(RX_LED, LOW);
      digitalWrite(TX_LED,HIGH);
      //mute input sound still playing welcome
      _audio->SetVolume(0,0.0);
      _audio->SetVolume(1,1.0);
      if ( _audio->IsCTCSSEnabled()) _audio->SetVolume(2,CTCSS_LVL);
      _audio->Play("welcome.wav");
      break;
    }
    case REPEATER:
    {
      _log->Message("Repeater");
      digitalWrite(RX_LED, LOW);
      digitalWrite(TX_LED,HIGH);
      _audio->SetVolume(0,1.0);
      _audio->SetVolume(1,0.0);
      if (_audio->IsCTCSSEnabled()) _audio->SetVolume(2,CTCSS_LVL);
      break;
    }
    case ANNONCE_FIN:
    {
      _log->Message("Annonce Fin");
      digitalWrite(RX_LED, LOW);
      digitalWrite(TX_LED,HIGH);
      _audio->SetVolume(0,0.0);
      _audio->SetVolume(1,1.0);
      if (_audio->IsCTCSSEnabled()) _audio->SetVolume(2,CTCSS_LVL);
      _audio->Play("end.wav");
      break;
    }
  }
  _etat = pState;
}

void CRepeater::OnUpdate()
{
  _currentState = digitalRead(ANNONCE_BTN);
  if (_lastState == LOW && _currentState == HIGH)
  {
    Actions(ANNONCE_DEB);
  }
  _lastState = _currentState;
  // Read RSSI and if threshold, play a K
  _RSSI = analogRead(RSSI);

  _CD = (_RSSI > _CD_Threshold);
  if (_CD != _lastCD)
  {
    _lastCD = _CD;
    if ((!_CD) && (_etat == REPEATER)) _audio->Play("beep.wav");
  }
  _t.handle();
}