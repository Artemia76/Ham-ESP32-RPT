#include "repeater.hpp"

CRepeater::CRepeater() :
    _step(IDLE),
    _lastStep(IDLE),
    _counter(0),
    _lastState(HIGH),
    _currentState(HIGH),
    _switch(true),
    _CD_Threshold(3000),
    _CD(false),
    _antiBounce(0),
    _TOT(180),
    _TOT_Counter(0),
    _HalfSecondBlink(false)
{
  _log = CLog::Create();
  _log->Message("Starting Repeater... ");

  _audio = CAudio::Create();
  _RSSI = analogRead(RSSI);
  _lastCD = _CD;

  //
  // Setting I/O
  //
  pinMode(RX_LED, OUTPUT);
  pinMode(TX_LED, OUTPUT);
  pinMode(ANNONCE_BTN, INPUT_PULLUP);
  pinMode(PTT, OUTPUT);
  
  //
  // Setting Slow Timer
  //
  _t1s.setInterval(CRepeater::OnTimer1SCB,1000);

  //
  // Setting Fast Timer
  //
  _t500ms.setInterval(CRepeater::OnTimer500msCB,500);

  Actions(IDLE);
  _log->Message("OK");
}

CRepeater::~CRepeater()
{

}

void CRepeater::OnTimer500msCB()
{
  CRepeater::Create()->OnTimer500ms();
}

void CRepeater::OnTimer500ms()
{
  // Animate Green leds
  // If Carriage Detect in IDLE mode , we blink
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

void CRepeater::OnTimer1SCB()
{
  CRepeater::Create()->OnTimer1S();
}

void CRepeater::OnTimer1S()
{
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
        Actions(ANNONCE_DEB);
        _counter=0;
      }
      break;
    }
    case ANNONCE_DEB:
    {
      _counter++;
      if (_counter > 2)
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
      _TOT_Counter++;

      if ((_counter > 10) || (_TOT_Counter >= _TOT))
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
  //Reset _antiBounce
  if (_antiBounce > 0)
  {
    _antiBounce--;
    if (_antiBounce==0) _audio->SetVolume(1,0.0);
  }
  _log->Message("RSSI=" + String(_RSSI) + " TOT Timer = " + String(_TOT_Counter),true, CLog::DEBUG);
}

void CRepeater::Actions(const Steps& pStep)
{
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
    case ANNONCE_DEB:
    {
      _log->Message("Annonce début");
      //mute input sound still playing welcome
      _audio->SetVolume(0,0.0);
      _audio->SetVolume(1,1.0);
      if ( _audio->IsCTCSSEnabled()) _audio->SetVolume(2,CTCSS_LVL);
      _audio->Play("welcome.wav");
      digitalWrite(PTT,HIGH);
      break;
    }
    case REPEATER:
    {
      _log->Message("Repeater");
      if (_CD) _audio->SetVolume(0,1.0);
      else _audio->SetVolume(0,0.0);
      _audio->SetVolume(1,0.0);
      if (_audio->IsCTCSSEnabled()) _audio->SetVolume(2,CTCSS_LVL);
      _TOT_Counter = 0;
      break;
    }
    case ANNONCE_FIN:
    {
      _log->Message("Annonce Fin");
      _audio->SetVolume(0,0.0);
      _audio->SetVolume(1,1.0);
      if (_audio->IsCTCSSEnabled()) _audio->SetVolume(2,CTCSS_LVL);
      _audio->Play("end.wav");
      break;
    }
  }
  _step = pStep;
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
        _audio->Play("beep.wav");
        _TOT_Counter = 0;
        _antiBounce = 2;
      } 
    }
    _lastCD = _CD;
  }
  _t1s.handle();
  _t500ms.handle();

}