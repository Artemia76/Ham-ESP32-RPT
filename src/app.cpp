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
  _log = CLog::Create();
  _log->Message("Starting Repeater");
}

/*****************************************************************************/

CApp::~CApp ()
{
  _log->Message("Ending Repeater");
}

/*****************************************************************************/

void CApp::Loop ()
{
  for (auto Subcriber : _subscribers)
  {
      if (Subcriber != nullptr)
      {
          Subcriber->OnUpdate();
      }
  }
}

/*****************************************************************************/

void CApp::_subscribe(CAppEvent* pSubscriber)
{
    if (std::find(_subscribers.begin(), _subscribers.end(),pSubscriber) == _subscribers.end())
    {
        _subscribers.push_back(pSubscriber);
    }
}

/*****************************************************************************/

void CApp::_unSubscribe(CAppEvent* pSubscriber)
{
    _subscribers.erase(std::remove(_subscribers.begin(), _subscribers.end(), pSubscriber), _subscribers.end());
}