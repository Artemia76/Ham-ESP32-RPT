/**
 * @file log.hpp
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

#ifndef LOG_HPP
#define LOG_HPP

#include <Arduino.h>
#include <mutex>

class CLog
{
public:
    CLog();
    ~CLog();

    /**
     * @brief Send message to serial port
     * 
     * @param pMessage Message to display
     * @param pCR Cariage Return if True
     */
    void Message(const String& pMessage, bool pCR=true);
private:
    /**
     * @brief mutex to protect against concurent access
     * 
     */
    std::mutex _mutex;
};
#endif