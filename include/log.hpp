/**
 * @file log.hpp
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

#ifndef LOG_HPP
#define LOG_HPP

#include <Arduino.h>
#include "singleton.hpp"
#include <mutex>

class CLog : public CSingleTon<CLog>
{
    friend class CSingleTon <CLog>;
public:
    /**
     * @brief Enumaration of critical level
     * 
     */
     enum Level
     {
        NORMAL = 0,
        WARNING,
        ERROR,
        DEBUG
     };

    /**
     * @brief Send message to serial port
     * 
     * @param pMessage Message to display
     * @param pCR Cariage Return if True
     */
    void Message(const String& pMessage, bool pCR=true, Level pLevel = NORMAL);


    void SetLevel (Level pLevel);
private:
    CLog();
    ~CLog();

    /**
     * @brief mutex to protect against concurent access
     * 
     */
    std::mutex _mutex;

    /**
     * @brief Level of debug message
     * 
     */
    Level _level;
};
#endif