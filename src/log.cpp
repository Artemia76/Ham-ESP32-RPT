/**
 * @file log.cpp
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

#include "log.hpp"
#include "env.hpp"

/*****************************************************************************/

CLog::CLog ()
{
    Serial.begin(115200);
    //while(!Serial){} // Wait until serial monitor is online
    Message ("Starting logging");
#ifdef DBG
    _level = DEBUG;
#else
    _level = NORMAL;
#endif
}

/*****************************************************************************/

CLog::~CLog ()
{
    Serial.end();
}

/*****************************************************************************/

void CLog::Message (const String& pMessage, bool pCR, Level pLevel)
{
    std::lock_guard<std::mutex> lck(_mutex);
    if (pLevel > _level) return;
    if (Serial)
    {
        if (pCR)
        {
            Serial.println(pMessage);
        }
        else
        {
            Serial.print(pMessage);
        }
    }
}

/*****************************************************************************/

void CLog::SetLevel (Level pLevel)
{
    std::lock_guard<std::mutex> lck(_mutex);
    _level = pLevel;
}