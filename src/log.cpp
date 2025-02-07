/**
 * @file log.cpp
 * @author Artemia
 * @brief Test-ESP32 Project
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

#include "log.hpp"

/*****************************************************************************/

CLog::CLog ()
{
    Serial.begin(115200);
    //while(!Serial){} // Wait until serial monitor is online
    Message ("Starting logging");
}

/*****************************************************************************/

CLog::~CLog ()
{
    Serial.end();
}

/*****************************************************************************/

void CLog::Message (const String& pMessage, bool pCR)
{
    std::lock_guard<std::mutex> lck(_mutex);
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