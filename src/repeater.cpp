#include "repeater.hpp"

CRepeater::CRepeater() :
    _etat(IDLE),
    _lastEtat(IDLE),
    _counter(0),
    _lastState(HIGH),
    _currentState(HIGH),
    _switch(false)
{
  _log = CLog::Create();
  _log->Message("Starting Repeater... ",false);
  //
  // Configure I/O
  //
  pinMode(RX_LED, OUTPUT);
  pinMode(TX_LED, OUTPUT);
  pinMode(ANNONCE_BTN, INPUT_PULLUP);

  Actions(IDLE);
  _log->Message("OK");
}

CRepeater::~CRepeater()
{

}

void CRepeater::OnTimerCB()
{

}

void CRepeater::OnTimer()
{

}

void CRepeater::OnUpdate()
{
    
}