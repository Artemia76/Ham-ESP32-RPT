/**
 * @file timer.hpp
 * @author Gianni Peschiutta (F4IKZ)
 * @brief HAM-ESP32-RPT
 * @version 0.1
 * @date 2026-02-26
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

#ifndef TIMER_HPP
#define TIMER_HPP

#include <Arduino.h>
#include <functional>

class CTimer;

/*! \class CTimer
 *  \brief Timer representation
 *
 *  Timer Class for arduino board
 */

class CTimer
{
private:
    typedef std::function<void()> TimerCallback;
	TimerCallback _Callback;
	unsigned long  _interval;
	bool _oneShoot;
    bool _running;
	unsigned long _previousMillis;

public:

    /* \brief CTimer constructor
        *
        *  int Duration : Timer duration in msec
        * TimerCallback CallBack : address of callback timer function in format:
        * 	void myEventTimerFunction (CTimer* Timer)
        */
    CTimer (unsigned long pInterval, TimerCallback pCallBack,bool pOneShoot=false);

    /* \brief CTimer destructor
        */
    ~CTimer(){}

    /* \brief CTimer Updater
        *
        * This method update the timer and call the callback function if time duration elapsed
        * if OneShoot is false, it start again the timer for the same duration
        * need to be placed in loop
        */
    void	Update();

    void    Start();
    void	Start(unsigned long pInterval,bool pOneShoot=false );
    void	Stop();
    bool	IsRunning();
};


#endif //TIMER_HPP
