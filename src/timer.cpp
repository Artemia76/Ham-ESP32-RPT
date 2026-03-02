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

/*****************************************************************************/

CTimer::CTimer (unsigned long pInterval, TimerCallback pCallBack, bool pOneShot) :
    _previousMillis(0),
    _running(false)
{
	// Ensure the Interval is at least greater than 10 Milliseconds
	if (pInterval <10) pInterval=10;
	_interval=pInterval;
  _oneShot = pOneShot;
  _Callback = pCallBack;
	// Start the timer by default
  Start(pInterval);
}

/*****************************************************************************/

void CTimer::Start ()
{
	Start(_interval);
}

/*****************************************************************************/

void CTimer::Start (unsigned long pInterval, bool pOneShot)
{
	// Ensure the interval is at least greater than 10 Milliseconds
	if (pInterval > 10) _interval=pInterval;
	// Store the start value of millis
	_previousMillis=millis();
	_oneShot=pOneShot;
	_running=true;
}

/*****************************************************************************/

void CTimer::Stop ()
{
	// Return the running state
	if(_running) _running=false;
}

/*****************************************************************************/

void CTimer::Update ()
{
	// If running we check if timer reach Interval
	if(_running)
	{
		// Statement to check if we reached the interval
		if (millis() - _previousMillis >= _interval)
		{
			// If not oneShot
			if (_oneShot) _running=false;
			else _previousMillis=millis();
			// Calling the callback function
			_Callback();
		}
	}
}

/*****************************************************************************/

bool CTimer::IsRunning()
{
	// Return timer state
	return (_running);
}
