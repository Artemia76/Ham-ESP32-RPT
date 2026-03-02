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

/**
 * @brief Handle Timer with callback
 * @details This class purpose simple timer with preselection to callback user
 *          class Method with possibility to one shot or repeat mode 
 * 
 */
class CTimer
{
private:
  /**
   * @brief Callback type method
   * 
   */
  typedef std::function<void()> TimerCallback;

  /**
   * @brief Memory to Callback User class method
   * 
   */
  TimerCallback _Callback;
  uint32_t _interval;
  bool _oneShot;
  bool _running;
	uint32_t _previousMillis;

public:

  /**
   * @brief CTimer constructor
   *
   * @param pInterval : Timer duration in msec
   * @param pCallBack : CallBack method address :
   *            Usage exemple : std::bind(&Class::CallbackMethod,this)
   * @param pOneShot : (Optional) If true , trig once a time the timer
   *                  If False, by default restart timer after each callback
   */
  CTimer (unsigned long pInterval, TimerCallback pCallBack,bool pOneShot=false);

  /**
   * @brief Destroy the CTimer object
   * 
   */
  ~CTimer(){}

  /**
   * @brief Update the timer and trig the callback if Interval reached
   * 
   */
  void Update();

  /**
   * @brief Start the timer for the last duration Interval
   * 
   */
  void Start();

  /**
   * @brief Start the timer for the interval in parameters and recall mode
   * 
   * @param pInterval : Interval duration ine millisecond
   * @param pOneShot : Call one shot the callback if true
   */
  void Start(unsigned long pInterval,bool pOneShot=false );

  /**
   * @brief Stop the timer
   * 
   */
  void Stop();

  /**
   * @brief Return Timer State
   * 
   * @return true : The timer running
   * @return false : The timer stopped
   */
  bool IsRunning();
};

#endif //TIMER_HPP
