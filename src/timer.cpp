/**
 * @file timer.cpp
 * @author Gianni Peschiutta (F4IKZ)
 * @brief HAM-ESP32-RPT
 * @version 0.1
 * @date 2026-02-28
 * 
 * @copyright Copyright (c) 2026
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

#include "timer.hpp"

CTimer::CTimer (unsigned long pInterval, TimerCallback pCallBack, bool pOneShoot) :
    _previousMillis(0),
    _running(false)
{
	if (pInterval <10) pInterval=10;
	_interval=pInterval;
    _oneShoot = pOneShoot;
    _Callback = pCallBack;
    Start(pInterval);
}

void CTimer::Start ()
{
    Start(_interval);
}

void CTimer::Start (unsigned long pInterval, bool pOneShoot)
{
	if (pInterval > 10) _interval=pInterval;
	_previousMillis=millis();
	_oneShoot=pOneShoot;
	_running=true;
}

void CTimer::Stop ()
{
	if(_running) _running=false;
}

void CTimer::Update ()
{
	if(_running)
	{
		if (millis() - _previousMillis >= _interval)
		{
			if (!_oneShoot) _previousMillis=millis();
			else _running=false;
			_Callback();
		}
	}
}

bool CTimer::IsRunning()
{
	return (_running);
}
