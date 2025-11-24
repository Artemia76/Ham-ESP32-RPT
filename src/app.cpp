/**
 * @file app.cpp
 * @author Gianni Peschiutta (F4IKZ)
 * @brief HAM-ESP32-RPT
 * @version 0.1
 * @date 2024-11-21
 * 
 * @copyright Copyright (c) 2024
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

#include "app.hpp"

/*****************************************************************************/

CApp::CApp ()
{
  _log = CLog::Create(); // Get Log singleton
  _log->Message("Starting Application... OK");
}

/*****************************************************************************/

CApp::~CApp ()
{
  _log->Message("Ending Application");
}

/*****************************************************************************/

void CApp::Loop ()
{
  for (auto Subscriber : _subscribers) //Call main loop for each subscriber
  {
      if (Subscriber != nullptr) // Be sure Subscriber is not null to avoid crash
      {
          Subscriber->OnUpdate(); // Call the virtual OnUpdate method of subscriber
      }
  }
}

/*****************************************************************************/

void CApp::_subscribe(CAppEvent* pSubscriber)
{
    // Ensure subscriber is not already in the list to avoid duplicates
    // Then add it
    if (std::find(_subscribers.begin(), _subscribers.end(),pSubscriber) == _subscribers.end())
    {
        _subscribers.push_back(pSubscriber);
    }
}

/*****************************************************************************/

void CApp::_unSubscribe(CAppEvent* pSubscriber)
{
    // Remove subscriber from the list if exists
    _subscribers.erase(std::remove(_subscribers.begin(), _subscribers.end(), pSubscriber), _subscribers.end());
}